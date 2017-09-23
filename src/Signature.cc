//
// Created by red on 9/11/17.
// Using podofosign functions
//

#include "Signature.h"

Signature::Signature(const CallbackInfo& info)
  : ObjectWrap(info)
{
  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();
  ERR_load_PEM_strings();
  ERR_load_ASN1_strings();
  ERR_load_EVP_strings();
}

static int
privateKeyPasswordCB(char* buf,
                     int bufsize,
                     int PODOFO_UNUSED_PARAM(rwflag),
                     void* userdata)
{
  const char* password = reinterpret_cast<const char*>(userdata);

  if (!password)
    return 0;

  int res = strlen(password);

  if (res > bufsize)
    res = bufsize;

  memcpy(buf, password, res);

  return res;
}

static void
signWithSigner(PdfSignOutputDevice& signer, X509* cert, EVP_PKEY* pkey)
{
  if (!cert)
    PODOFO_RAISE_ERROR_INFO(ePdfError_InvalidHandle, "cert == NULL");
  if (!pkey)
    PODOFO_RAISE_ERROR_INFO(ePdfError_InvalidHandle, "pkey == NULL");

  unsigned int uBufferLen = 65535, len;
  char* pBuffer;

  while (static_cast<void>(pBuffer = reinterpret_cast<char*>(
                             podofo_malloc(sizeof(char) * uBufferLen))),
         !pBuffer) {
    uBufferLen = uBufferLen / 2;
    if (!uBufferLen)
      break;
  }

  if (!pBuffer)
    PODOFO_RAISE_ERROR(ePdfError_OutOfMemory);

  int rc;
  BIO* mem = BIO_new(BIO_s_mem());
  if (!mem) {
    podofo_free(pBuffer);
    //    raise_podofo_error_with_opensslerror("Failed to create input BIO");
  }

  unsigned int flags = PKCS7_DETACHED | PKCS7_BINARY;
  PKCS7* pkcs7 = PKCS7_sign(cert, pkey, NULL, mem, flags);
  if (!pkcs7) {
    BIO_free(mem);
    podofo_free(pBuffer);
    //    raise_podofo_error_with_opensslerror("PKCS7_sign failed");
  }

  while (len = signer.ReadForSignature(pBuffer, uBufferLen), len > 0) {
    rc = BIO_write(mem, pBuffer, len);
    if (static_cast<unsigned int>(rc) != len) {
      PKCS7_free(pkcs7);
      BIO_free(mem);
      podofo_free(pBuffer);
      //      raise_podofo_error_with_opensslerror("BIO_write failed");
    }
  }

  podofo_free(pBuffer);

  if (PKCS7_final(pkcs7, mem, flags) <= 0) {
    PKCS7_free(pkcs7);
    BIO_free(mem);
    //    raise_podofo_error_with_opensslerror("PKCS7_final failed");
  }

  bool success = false;
  BIO* out = BIO_new(BIO_s_mem());
  if (!out) {
    PKCS7_free(pkcs7);
    BIO_free(mem);
    //    raise_podofo_error_with_opensslerror("Failed to create output BIO");
  }

  char* outBuff = NULL;
  long outLen;

  i2d_PKCS7_bio(out, pkcs7);

  outLen = BIO_get_mem_data(out, &outBuff);

  if (outLen > 0 && outBuff) {
    if (static_cast<size_t>(outLen) > signer.GetSignatureSize()) {
      PKCS7_free(pkcs7);
      BIO_free(out);
      BIO_free(mem);

      std::ostringstream oss;
      oss << "Requires at least " << outLen
          << " bytes for the signature, but reserved is only "
          << signer.GetSignatureSize() << " bytes";
      PODOFO_RAISE_ERROR_INFO(ePdfError_ValueOutOfRange, oss.str().c_str());
    }

    PdfData signature(outBuff, outLen);
    signer.SetSignature(signature);
    success = true;
  }

  PKCS7_free(pkcs7);
  BIO_free(out);
  BIO_free(mem);

  //  if (!success) raise_podofo_error_with_opensslerror( "Failed to get data
  //  from the output BIO");
}

static PdfObject*
findExistingSignatureField(PdfAcroForm* pAcroForm, const PdfString& name)
{
  if (!pAcroForm)
    PODOFO_RAISE_ERROR(ePdfError_InvalidHandle);

  PdfObject* pFields =
    pAcroForm->GetObject()->GetDictionary().GetKey(PdfName("Fields"));
  if (pFields) {
    if (pFields->GetDataType() == ePdfDataType_Reference)
      pFields = pAcroForm->GetDocument()->GetObjects()->GetObject(
        pFields->GetReference());

    if (pFields && pFields->GetDataType() == ePdfDataType_Array) {
      PdfArray& rArray = pFields->GetArray();
      PdfArray::iterator it, end = rArray.end();
      for (it = rArray.begin(); it != end; it++) {
        // require references in the Fields array
        if (it->GetDataType() == ePdfDataType_Reference) {
          PdfObject* item = pAcroForm->GetDocument()->GetObjects()->GetObject(
            it->GetReference());

          if (item && item->GetDictionary().HasKey(PdfName("T")) &&
              item->GetDictionary().GetKey(PdfName("T"))->GetString() == name) {
            // found a field with the same name
            const PdfObject* pFT = item->GetDictionary().GetKey(PdfName("FT"));
            if (!pFT && item->GetDictionary().HasKey(PdfName("Parent"))) {
              const PdfObject* pTemp = item->GetIndirectKey(PdfName("Parent"));
              if (!pTemp) {
                PODOFO_RAISE_ERROR(ePdfError_InvalidDataType);
              }

              pFT = pTemp->GetDictionary().GetKey(PdfName("FT"));
            }

            if (!pFT) {
              PODOFO_RAISE_ERROR(ePdfError_NoObject);
            }

            const PdfName fieldType = pFT->GetName();
            if (fieldType != PdfName("Sig")) {
              std::string err = "Existing field '";
              err += name.GetString();
              err += "' isn't of a signature type, but '";
              err += fieldType.GetName().c_str();
              err += "' instead";

              PODOFO_RAISE_ERROR_INFO(ePdfError_InvalidName, err.c_str());
            }

            return item;
          }
        }
      }
    }
  }

  return NULL;
}

void
Signature::ApplySignature(const CallbackInfo& info)
{
  string outFile;
  string reason;
  int sigPageIndex;
  int sigSize = -1;
  int result = 0;
  FILE* fp;
  X509** outCert;
  EVP_PKEY** outPKey;
  pdf_int32 minSize = 0;
  PdfMemDocument* doc;
  fp = fopen(certFile.c_str(), "rb");
  if (!fp) {
    throw Napi::Error::New(info.Env(), "");
  }

  *outCert = PEM_read_X509(fp, NULL, NULL, NULL);
  if (fseeko(fp, 0, SEEK_END) != -1) {
    minSize += ftello(fp);
  } else
    minSize += 3072;
  fclose(fp);
  if (!*outCert) {
    throw Napi::Error::New(info.Env(), "Failed to decode cert file");
  }
  fp = fopen(pKeyFile.c_str(), "rb");
  if (!fp) {
    X509_free(*outCert);
    *outCert = nullptr;
    throw Napi::Error::New(info.Env(), "Failed to open private key file");
  }
  *outPKey = PEM_read_PrivateKey(
    fp, nullptr, privateKeyPasswordCB, const_cast<char*>(password.c_str()));
  if (fseeko(fp, 0, SEEK_END) != -1)
    minSize += ftello(fp);
  else
    minSize += 1024;
  fclose(fp);
  if (!*outPKey) {
    X509_free(*outCert);
    *outCert = nullptr;
    throw Napi::Error::New(info.Env(), "Failed to decode private key");
  }
  if (sigSize > 0)
    minSize = sigSize;

  PdfSignatureField* signField = NULL;
  PdfAnnotation* pTemporaryAnnot = NULL; // for existing signature fields

  try {
    PdfAcroForm* acroForm = doc->GetAcroForm();
    PdfDictionary acroDict = acroForm->GetObject()->GetDictionary();

    if (!acroDict.HasKey(PdfName("SigFlags")) ||
        !acroDict.GetKey("SigFlags")->IsNumber() ||
        acroDict.GetKeyAsLong(PdfName("SigFlags")) != 3)
      if (acroDict.HasKey(PdfName("SigFlags")))
        acroDict.RemoveKey(PdfName("SigFlags"));

    pdf_int64 val = 3;
    acroDict.AddKey(PdfName("SigFlags"), PdfObject(val));

    if (acroForm->GetNeedAppearances())
      acroForm->SetNeedAppearances(false);

    PdfOutputDevice device(outFile.c_str());
    PdfSignOutputDevice signer(&device);

    PdfString name;
    PdfObject* existingField;
    char
      fldName[96]; // use bigger buffer to make sure sprintf does not overflow
    sprintf(fldName,
            "PodofoSignatureField%" PDF_FORMAT_INT64,
            static_cast<pdf_int64>(doc->GetObjects().GetObjectCount()));

    name = PdfString(fldName);
    //    if( existingField)
    //        {
    //            if( !existingField->GetDictionary().HasKey( "P" ) )
    //            {
    //                std::string err = "Signature field named '";
    //                err += name.GetString();
    //                err += "' doesn't have a page reference";

    //                PODOFO_RAISE_ERROR_INFO( ePdfError_PageNotFound,
    //                err.c_str() );
    //            }

    //            PdfPage* pPage;

    //            pPage = doc->GetPagesTree()->GetPage(
    //            existingField->GetDictionary().GetKey( "P" )->GetReference()
    //            ); if( !pPage )
    //                PODOFO_RAISE_ERROR( ePdfError_PageNotFound );

    //            pTemporaryAnnot = new PdfAnnotation( existingField, pPage );
    //            if( !pTemporaryAnnot )
    //                PODOFO_RAISE_ERROR_INFO( ePdfError_OutOfMemory, "Cannot
    //                allocate annotation object for existing signature field"
    //                );

    //            pSignField = new PdfSignatureField( pTemporaryAnnot );
    //            if( !pSignField )
    //                PODOFO_RAISE_ERROR_INFO( ePdfError_OutOfMemory, "Cannot
    //                allocate existing signature field object" );

    //            pSignField->EnsureSignatureObject();
    //        }
    PdfPage* page = doc->GetPage(sigPageIndex);
    PdfRect annotRect;
    PdfAnnotation* annot =
      page->CreateAnnotation(EPdfAnnotation::ePdfAnnotation_Widget, annotRect);

    annot->SetFlags(ePdfAnnotationFlags_Invisible | ePdfAnnotationFlags_Hidden);

    signField = new PdfSignatureField(annot, acroForm, doc);

    signer.SetSignatureSize(static_cast<size_t>(minSize));
    signField->SetFieldName(name);
    signField->SetSignatureReason(reason);
    signField->SetSignatureDate(PdfDate());
    signField->SetSignature(*signer.GetSignatureBeacon());

    doc->WriteUpdate(&signer, outFile.c_str() != nullptr);

    if (!signer.HasSignaturePosition())
      PODOFO_RAISE_ERROR_INFO(
        ePdfError_SignatureError,
        "Cannot find signature position in the document data");

    signer.AdjustByteRange();
    signer.Seek(0);
    signWithSigner(signer, *outCert, *outPKey);
    signer.Flush();

  } catch (PdfError& e) {
    std::cerr << "Error: An error " << e.GetError()
              << " occurred during the sign of the pdf file:" << std::endl;
    e.PrintErrorMsg();

    result = e.GetError();
  }
  ERR_free_strings();

  if (signField)
    delete signField;

  if (outPKey)
    EVP_PKEY_free(*outPKey);

  if (outCert)
    X509_free(*outCert);
}
