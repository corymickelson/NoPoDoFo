#include "Signer.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <string>

using namespace Napi;
using namespace PoDoFo;

Signer::Signer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_object, napi_valuetype::napi_object });
  doc = Document::Unwrap(info[0].As<Object>());
  field = SignatureField::Unwrap(info[1].As<Object>());
  if (!doc->LoadedForIncrementalUpdates()) {
    throw Napi::Error::New(info.Env(),
                           "Please reload Document with forUpdates = true.");
  }
  if (info.Length() == 3 && info[2].IsString()) {
    PdfOutputDevice device(info[2].As<String>().Utf8Value().c_str(), true);
    signer = new PdfSignOutputDevice(&device);
    buffer = nullptr;
  } else {
    buffer = new PdfRefCountedBuffer();
    PdfOutputDevice device(buffer);
    signer = new PdfSignOutputDevice(&device);
  }
}

void
Signer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Signer",
    { InstanceMethod("sign", &Signer::Sign),
      InstanceMethod("signWithCertificateAndKey", &Signer::PoDoFoSign) });
  target.Set("Signer", ctor);
}

class SignAsync : public AsyncWorker
{
public:
  SignAsync(Function& cb, Signer* self, Buffer<char>& data)
    : AsyncWorker(cb)
    , self(self)
    , input(data)
  {}

private:
  Signer* self;
  bool outBuffer = false;
  Buffer<char> input;

  // AsyncWorker interface
protected:
  void Execute()
  {
    try {
      self->signer->SetSignatureSize(input.Length());
      PdfData signature(input.Data());
      self->field->GetField()->SetSignature(
        *self->signer->GetSignatureBeacon());
      self->doc->GetDocument()->WriteUpdate(self->signer, true);

      if (!self->signer->HasSignaturePosition())
        PODOFO_RAISE_ERROR_INFO(
          ePdfError_SignatureError,
          "Cannot find signature position in the document data");

      self->signer->AdjustByteRange();
      self->signer->Seek(0);
      self->signer->SetSignature(input.Data());
      self->signer->Flush();
      if (self->buffer)
        outBuffer = true;
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    }
  }
  void OnOK()
  {
    HandleScope scope(Env());
    auto value = Buffer<char>::Copy(
      Env(), self->buffer->GetBuffer(), self->buffer->GetSize());
    Callback().Call({ Env().Null(), value });
  }
};

Value
Signer::PoDoFoSign(const CallbackInfo& info)
{
  if (info.Length() < 2 || !info[0].IsFunction() || !info[1].IsBuffer()) {
    throw Error::New(info.Env(),
                     "SetSignature requires a single argument of type buffer");
  }
  Buffer<char> sig = info[1].As<Buffer<char>>();
  auto cb = info[0].As<Function>();
  auto worker = new SignAsync(cb, this, sig);
  worker->Queue();
  return info.Env().Undefined();
}

Value
Signer::Sign(const CallbackInfo& info)
{
  try {
    //    OPENSSL_init();
    //    if (info.Length() < 3) {
    //      throw Napi::Error::New(info.Env(),
    //                             "requires a minimum certificate and key");
    //    }
    //    string certfile = info[0].As<String>().Utf8Value();
    //    string keyfile = info[1].As<String>().Utf8Value();
    Buffer<char> signatureData = info[0].As<Buffer<char>>();
    //    string password = "";
    //    if (info.Length() == 4 && info[3].IsString()) {
    //      password = info[2].As<String>().Utf8Value();
    //    }
    //    X509* cert = NULL;
    //    EVP_PKEY* pkey = NULL;
    //    pdf_int32 min_signature_size = 0;
    //    min_signature_size = 0;
    auto pSignField = field->GetField();
    //    FILE* fp;

    //    fp = fopen(certfile.c_str(), "rb");

    //    if (!fp) {
    //      throw Error::New(info.Env(), "Failed to open certificate file");
    //    }

    //    cert = PEM_read_X509(fp, NULL, NULL, NULL);

    //    if (fseeko(fp, 0, SEEK_END) != -1)
    //      min_signature_size += ftello(fp);
    //    else
    //      min_signature_size += 3072;

    //    fclose(fp);

    //    if (!cert) {
    //      throw Error::New(info.Env(), "Failed to decode certificate file");
    //    }

    //    fp = fopen(keyfile.c_str(), "rb");

    //    if (!fp) {
    //      X509_free(cert);
    //      cert = NULL;

    //      throw Error::New(info.Env(), "Failed to private key file");
    //    }

    //    pkey = PEM_read_PrivateKey(fp,
    //                               NULL,
    //                               [](char* buf,
    //                                  int bufsize,
    //                                  int PODOFO_UNUSED_PARAM(rwflag),
    //                                  void* userdata) {
    //                                 const char* password =
    //                                   reinterpret_cast<const
    //                                   char*>(userdata);

    //                                 if (!password)
    //                                   return 0;

    //                                 int res = strlen(password);

    //                                 if (res > bufsize)
    //                                   res = bufsize;

    //                                 memcpy(buf, password, res);

    //                                 return res;
    //                               },
    //                               const_cast<char*>(password.c_str()));

    //    if (fseeko(fp, 0, SEEK_END) != -1)
    //      min_signature_size += ftello(fp);
    //    else
    //      min_signature_size += 1024;

    //    fclose(fp);

    //    if (!pkey) {
    //      X509_free(cert);
    //      cert = NULL;

    //      throw Error::New(info.Env(), "Failed to decode private key file");
    //    }

    //    int result = 0;
    auto document = doc->GetDocument();
    PdfRefCountedBuffer r;
    PdfOutputDevice outputDevice("/tmp/test.pdf", true /*&r*/);
    PdfSignOutputDevice signer(&outputDevice);
    //    signer.SetSignatureSize(min_signature_size);
    signer.SetSignatureSize(signatureData.Length());

    pSignField->SetSignatureDate(PdfDate());
    pSignField->SetSignature(*signer.GetSignatureBeacon());
    document->WriteUpdate(&signer, true);

    if (!signer.HasSignaturePosition())
      throw Error::New(info.Env(),
                       "Cannot find signature position in the document data");

    signer.AdjustByteRange();
    signer.Seek(0);
    //    if (!cert)
    //      PODOFO_RAISE_ERROR_INFO(ePdfError_InvalidHandle, "cert == NULL");
    //    if (!pkey)
    //      PODOFO_RAISE_ERROR_INFO(ePdfError_InvalidHandle, "pkey == NULL");

    //    unsigned int uBufferLen = 65535, len;
    //    char* pBuffer;

    //    while (pBuffer =
    //             reinterpret_cast<char*>(podofo_malloc(sizeof(char) *
    //             uBufferLen)),
    //           !pBuffer) {
    //      uBufferLen = uBufferLen / 2;
    //      if (!uBufferLen)
    //        break;
    //    }

    //    if (!pBuffer)
    //      PODOFO_RAISE_ERROR(ePdfError_OutOfMemory);

    //    int rc;
    //    BIO* mem = BIO_new(BIO_s_mem());
    //    if (!mem) {
    //      podofo_free(pBuffer);
    //      throw Error::New(info.Env(), "Failed to create input BIO");
    //    }

    //    unsigned int flags = PKCS7_DETACHED | PKCS7_BINARY;
    //    PKCS7* pkcs7 = PKCS7_sign(cert, pkey, NULL, mem, flags);
    //    if (!pkcs7) {
    //      BIO_free(mem);
    //      podofo_free(pBuffer);
    //      throw Error::New(info.Env(), "PKCS7_sign failed");
    //    }

    //    while (len = signer.ReadForSignature(pBuffer, uBufferLen), len > 0) {
    //      rc = BIO_write(mem, pBuffer, len);
    //      if (static_cast<unsigned int>(rc) != len) {
    //        PKCS7_free(pkcs7);
    //        BIO_free(mem);
    //        podofo_free(pBuffer);
    //        throw Error::New(info.Env(), "BIO_write failed");
    //      }
    //    }

    //    podofo_free(pBuffer);

    //    if (PKCS7_final(pkcs7, mem, flags) <= 0) {
    //      PKCS7_free(pkcs7);
    //      BIO_free(mem);
    //      throw Error::New(info.Env(), "PKCS7_final failed");
    //    }

    //    bool success = false;
    //    BIO* out = BIO_new(BIO_s_mem());
    //    if (!out) {
    //      PKCS7_free(pkcs7);
    //      BIO_free(mem);
    //      throw Error::New(info.Env(), "Failed to create output BIO");
    //    }

    //    char* outBuff = NULL;
    //    long outLen;

    //    i2d_PKCS7_bio(out, pkcs7);

    //    outLen = BIO_get_mem_data(out, &outBuff);

    //    if (outLen > 0 && outBuff) {
    //      if (static_cast<size_t>(outLen) > signer.GetSignatureSize()) {
    //        PKCS7_free(pkcs7);
    //        BIO_free(out);
    //        BIO_free(mem);

    //        std::ostringstream oss;
    //        oss << "Requires at least " << outLen
    //            << " bytes for the signature, but reserved is only "
    //            << signer.GetSignatureSize() << " bytes";
    //        throw Error::New(info.Env(), oss.str().c_str());
    //      }

    PdfData signature(signatureData.Data(), signatureData.Length());
    //      PdfData signature(outBuff, outLen);
    signer.SetSignature(signature);
    //      success = true;
    //    }

    signer.Flush();
    //    auto signedDocBuffer =
    //      Buffer<char>::Copy(info.Env(), r.GetBuffer(), r.GetSize());

    //    if (pkey)
    //      EVP_PKEY_free(pkey);

    //    if (cert)
    //      X509_free(cert);

    //    return signedDocBuffer;
    return info.Env().Undefined();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
    ErrorHandler(err, info);
  }
}
