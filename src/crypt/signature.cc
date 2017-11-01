#include "signature.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../doc/Document.h"
#include "../doc/SignatureField.h"
#include <napi.h>

using namespace Napi;
using namespace PoDoFo;

/**
 * NPDF is assumed to have been compiled using PoDoFo compiled with OpenSSl
 * v1.1.x
 * @brief Signature::Signature
 * @param info
 */
Signature::Signature(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto pdfdoc = Document::Unwrap(info[0].As<Object>());
  if (!pdfdoc->LoadedForIncrementalUpdates()) {
    throw Napi::Error::New(info.Env(),
                           "Document must be loaded for incremental updates. "
                           "Please use Document({filepath}, update: true).");
  }
  if (pdfdoc->GetDocument()->GetPageCount() == 0) {
    throw Napi::Error::New(info.Env(), "document has no pages");
  }
  if (!pdfdoc->GetDocument()->GetAcroForm()) {
    throw Napi::Error::New(info.Env(), "Invalid handle, AcroForm is null");
  }
}

void
Signature::Initialize(Napi::Env& env, Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Signature",
    { InstanceAccessor("page", &Signature::GetPage, &Signature::SetPage),
      InstanceAccessor("field", &Signature::GetField, &Signature::SetField),
      InstanceMethod("loadCert", &Signature::LoadCert),
      InstanceMethod("loadPKey", &Signature::LoadPKey),
      InstanceMethod("sign", &Signature::Sign),
      InstanceMethod("hasField", &Signature::HasExistingField) });
  target.Set("Signature", ctor);
}

void
Signature::LoadCert(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  const string certFile = info[0].As<String>().Utf8Value();
  FILE* fp;
  if ((fp = std::fopen(certFile.c_str(), "rb")) == nullptr) {
    fclose(fp);
    throw Napi::Error::New(info.Env(), "Failed to load cert file");
  }
  cert = PEM_read_X509(fp, nullptr, nullptr, nullptr);
  if (std::fseek(fp, 0, SEEK_END) != -1)
    signatureSize += ftello(fp);
  else
    signatureSize += 3072;
  if (!cert) {
    fclose(fp);
    throw Napi::Error::New(info.Env(), "Failed to decode certificate");
  }
  fclose(fp);
}

void
Signature::LoadPKey(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  const string keyFile = info[0].As<String>().Utf8Value();
  FILE* fp;
  if ((fp = std::fopen(keyFile.c_str(), "rb")) == nullptr) {
    fclose(fp);
    throw Napi::Error::New(info.Env(), "Failed to load private key file");
  }
  key = PEM_read_PrivateKey(
    fp,
    nullptr,
    [](char* buf, int size, int PODOFO_UNUSED_PARAM(rwflag), void* data) {
      const char* password = reinterpret_cast<const char*>(data);
      if (!password)
        return 0;

      int res = static_cast<int>(strlen(password));
      if (res > size)
        res = size;

      memcpy(buf, password, static_cast<size_t>(res));

      return res;

    },
    const_cast<char*>(password.c_str()));

  if (fseeko(fp, 0, SEEK_END) != -1)
    signatureSize += ftello(fp);
  else
    signatureSize += 1024;

  if (!key) {
    X509_free(cert);
    cert = nullptr;
    throw Napi::Error::New(info.Env(), "Failed to decode private key");
  }
  fclose(fp);
}

Napi::Value
Signature::Sign(const CallbackInfo& info)
{
  try {
    if (!cert || !key) {
      throw Napi::Error::New(info.Env(), "Key and Cert must be loaded first");
    }
    PdfRefCountedBuffer refBuffer;
    if (info[0].IsNull()) {
      PdfOutputDevice device(&refBuffer);
      signer = new PdfSignOutputDevice(&device);
    } else if (info[0].IsString()) {
      PdfOutputDevice device(info[0].As<String>().Utf8Value().c_str());
      signer = new PdfSignOutputDevice(&device);
    }
    if (!signer) {
      throw Napi::Error::New(info.Env(),
                             "Sign requires file path {string} or null");
    }

    signer->SetSignatureSize(static_cast<size_t>(signatureSize));

    doc->WriteUpdate(signer);

    if (!signer->HasSignaturePosition())
      throw Napi::Error::New(
        info.Env(), "Cannot find signature position in the document data");

    signer->AdjustByteRange();
    signer->Seek(0);

    unsigned int uBufferLen = 65535, len;
    char* buffer;
    while (static_cast<void>(buffer = reinterpret_cast<char*>(
                               podofo_malloc(sizeof(char) * uBufferLen))),
           !buffer) {
      uBufferLen = uBufferLen / 2;
      if (!uBufferLen)
        break;
    }
    if (!buffer)
      PODOFO_RAISE_ERROR(ePdfError_OutOfMemory);
    int rc;
    BIO* mem = BIO_new(BIO_s_mem());
    if (!mem) {
      podofo_free(buffer);
      throw Napi::Error::New(info.Env(), "Failed to create input BIO");
    }

    unsigned int flags = PKCS7_DETACHED | PKCS7_BINARY;
    PKCS7* pkcs7 = PKCS7_sign(cert, key, NULL, mem, static_cast<int>(flags));
    if (!pkcs7) {
      BIO_free(mem);
      podofo_free(buffer);
      throw Napi::Error::New(info.Env(), "PKCS7_sign failed");
    }

    while (len = signer->ReadForSignature(buffer, uBufferLen), len > 0) {
      rc = BIO_write(mem, buffer, len);
      if (static_cast<unsigned int>(rc) != len) {
        PKCS7_free(pkcs7);
        BIO_free(mem);
        podofo_free(buffer);
        throw Napi::Error::New(info.Env(), "BIO_write failed");
      }
    }

    podofo_free(buffer);

    if (PKCS7_final(pkcs7, mem, static_cast<int>(flags)) <= 0) {
      PKCS7_free(pkcs7);
      BIO_free(mem);
      throw Napi::Error::New(info.Env(), "PKCS7_final failed");
    }

    bool success = false;
    BIO* out = BIO_new(BIO_s_mem());
    if (!out) {
      PKCS7_free(pkcs7);
      BIO_free(mem);
      throw Napi::Error::New(info.Env(), "Failed to create output BIO");
    }

    char* outBuff = NULL;
    long outLen;

    i2d_PKCS7_bio(out, pkcs7);

    outLen = BIO_get_mem_data(out, &outBuff);

    if (outLen > 0 && outBuff) {
      if (static_cast<size_t>(outLen) > signer->GetSignatureSize()) {
        PKCS7_free(pkcs7);
        BIO_free(out);
        BIO_free(mem);

        std::ostringstream oss;
        oss << "Requires at least " << outLen
            << " bytes for the signature, but reserved is only "
            << signer->GetSignatureSize() << " bytes";
        PODOFO_RAISE_ERROR_INFO(ePdfError_ValueOutOfRange, oss.str().c_str());
      }

      PdfData signature(outBuff, static_cast<size_t>(outLen));
      signer->SetSignature(signature);
      success = true;
    }

    PKCS7_free(pkcs7);
    BIO_free(out);
    BIO_free(mem);

    if (!success)
      throw Napi::Error::New(info.Env(),
                             "Failed to get data from the output BIO");
    signer->Flush();

    if (info[0].IsNull()) {
      return Buffer<char>::Copy(
        info.Env(), refBuffer.GetBuffer(), refBuffer.GetSize());
    } else
      return info[0].As<String>();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

void
Signature::SetField(const CallbackInfo& info, const Napi::Value& value)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto sig = SignatureField::Unwrap(value.As<Object>());
  signatureField = sig->GetField();
}

Value
Signature::GetField(const CallbackInfo& info)
{
  return !signatureField
           ? info.Env().Null()
           : Napi::External<PdfSignatureField>::New(info.Env(), signatureField);
}
