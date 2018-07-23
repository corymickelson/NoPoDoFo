/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Signer.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Names.h"
#include "Document.h"
#include "SignatureField.h"
#include "StreamDocument.h"
#include <map>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/ssl.h>
#include <sstream>
#include <vector>

using namespace Napi;
using namespace PoDoFo;

using tl::nullopt;
using std::string;
using std::stringstream;

namespace NoPoDoFo {

FunctionReference Signer::constructor; // NOLINT

/**
 * @note JS new Signer(doc: IDocument, output?: string)
 * @param info
 */
Signer::Signer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , doc(Document::Unwrap(info[0].As<Object>())->GetDocument())
{
  if (info.Length() < 1) {
    Error::New(info.Env(), "Document required to construct Signer")
      .ThrowAsJavaScriptException();
    return;
  }
  if (!info[0].IsObject()) {
    TypeError::New(info.Env(), "Requires Document to construct Signer")
      .ThrowAsJavaScriptException();
    return;
  }
  if (info.Length() >= 2 && info[1].IsString()) {
    output = info[1].As<String>().Utf8Value();
  }
}

Signer::~Signer()
{
  HandleScope scope(Env());
  if (cert != nullptr) {
    X509_free(cert);
  }
  if (pkey) {
    EVP_PKEY_free(pkey);
  }
}

void
Signer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Signer",
    {
      InstanceAccessor("signatureField", &Signer::GetField, &Signer::SetField),
      InstanceMethod("write", &Signer::SignWorker),
      InstanceMethod("loadCertificateAndKey", &Signer::LoadCertificateAndKey),
    });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Signer", ctor);
}

void
Signer::SetField(const CallbackInfo& info, const Napi::Value& value)
{
  field = SignatureField::Unwrap(value.As<Object>())->GetField();
  try {
    field->EnsureSignatureObject();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
Signer::GetField(const CallbackInfo& info)
{
  return SignatureField::constructor.New(
    { External<PdfSignatureField>::New(info.Env(), field.get()) });
}

class SignAsync : public AsyncWorker
{
public:
  SignAsync(Function& cb, Signer& self, pdf_int32 minSigSize)
    : AsyncWorker(cb)
    , self(self)
    , minSigSize(minSigSize)
  {}

private:
  Signer& self;
  PdfRefCountedBuffer buffer;
  pdf_int32 minSigSize;

  // AsyncWorker interface
protected:
  void Execute() override
  {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    ERR_load_PEM_strings();
    ERR_load_ASN1_strings();
    ERR_load_EVP_strings();
#else
    OPENSSL_init_ssl(0, nullptr);
    OPENSSL_init();
#endif
    try {
      PdfObject* acroform = self.doc.GetAcroForm(false)->GetObject();
      size_t sigBuffer = 65535, sigBufferLen;
      int rc;
      char* sigData;
      char* outBuffer;
      long outBufferLen;
      BIO* memory;
      BIO* out;
      PKCS7* p7;

      // Set SigFlags in AcroForm as Signed in AppendOnly mode (3)

      if (acroform->GetDictionary().HasKey(PdfName(Name::SIG_FLAGS))) {
        acroform->GetDictionary().RemoveKey(PdfName(Name::SIG_FLAGS));
      }
      pdf_int64 signedAppendModeFlag = 3;
      acroform->GetDictionary().AddKey(PdfName(Name::SIG_FLAGS),
                                       PdfObject(signedAppendModeFlag));

      // Create an output device for the signed document
      PdfOutputDevice outputDevice =
        self.output.empty() ? PdfOutputDevice(&buffer)
                            : PdfOutputDevice(self.output.c_str(), true);
      PdfSignOutputDevice signer(&outputDevice);

      // minimally ensure signature field name property is filled, defaults to
      // "NoPoDoFo.SignatureField"
      if (self.field->GetFieldName().GetStringUtf8().empty()) {
        self.field->SetFieldName("NoPoDoFo.SignatureField");
      }
      /**
       * todo: Add user facing support for creating/modifying PdfDate objects.
       */
      self.field->SetSignatureDate(PdfDate());

      // Set output device to write signature to designated area.
      signer.SetSignatureSize(static_cast<size_t>(minSigSize));

      self.field->SetSignature(*signer.GetSignatureBeacon());
      self.doc.WriteUpdate(&signer, true);

      if (!signer.HasSignaturePosition()) {
        SetError("Cannot find signature position in the document data");
        return;
      }

      signer.AdjustByteRange();
      signer.Seek(0);

      // Create signature
      while (static_cast<void>(sigData = reinterpret_cast<char*>(
                                 podofo_malloc(sizeof(char) * sigBuffer))),
             !sigData) {
        sigBuffer = sigBuffer / 2;
        if (!sigBuffer)
          break;
      }
      if (!sigData) {
        SetError("PdfError: Out of Memory.");
        return;
      }

      memory = BIO_new(BIO_s_mem());
      if (!memory) {
        podofo_free(sigData);
        SetError("Failed to create input BIO");
        return;
      }
      p7 = PKCS7_sign(
        self.cert, self.pkey, nullptr, memory, PKCS7_DETACHED | PKCS7_BINARY);
      if (!p7) {
        BIO_free(memory);
        podofo_free(sigData);
        SetError("PKCS7 Sign failed");
        return;
      }
      while (sigBufferLen = signer.ReadForSignature(sigData, sigBuffer),
             sigBufferLen > 0) {
        rc = BIO_write(memory, sigData, static_cast<int>(sigBufferLen));
        if (static_cast<unsigned int>(rc) != sigBufferLen) {
          PKCS7_free(p7);
          BIO_free(memory);
          podofo_free(sigData);
          SetError("BIO write failed");
          return;
        }
      }

      podofo_free(sigData);
      if (PKCS7_final(p7, memory, PKCS7_DETACHED | PKCS7_BINARY) <= 0) {
        PKCS7_free(p7);
        BIO_free(memory);
        SetError("pkcs7 final failed");
        return;
      }

      out = BIO_new(BIO_s_mem());
      if (!out) {
        PKCS7_free(p7);
        BIO_free(memory);
        SetError("Failed to create output bio");
        return;
      }
      i2d_PKCS7_bio(memory, p7);
      outBufferLen = BIO_get_mem_data(memory, &outBuffer);
      if (outBufferLen > 0 && outBuffer) {
        if (static_cast<size_t>(outBufferLen) > signer.GetSignatureSize()) {
          PKCS7_free(p7);
          BIO_free(memory);
          BIO_free(out);
          SetError("Signature value out of prescribed range");
          return;
        }
        PdfData signature(outBuffer, static_cast<size_t>(outBufferLen));
        signer.SetSignature(signature);
        signer.Flush();
        return;
      } else {
        SetError("Invalid Signature was generated");
        return;
      }
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Error& err) {
      SetError(ErrorHandler::WriteMsg(err));
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    if (!self.output.empty()) {
      if (FILE* file = fopen(self.output.c_str(), "r")) {
        fclose(file);
        Callback().Call({ Env().Undefined() });
      } else {
        stringstream msg;
        msg << "Failed to write to: " << self.output << endl;
        Callback().Call({ String::New(Env(), msg.str()) });
      }
    } else {
      Callback().Call(
        { Env().Undefined(),
          Buffer<char>::Copy(Env(), buffer.GetBuffer(), buffer.GetSize()) });
    }
  }
};

/**
 * @note JS sign(signatureSize: int, cb: Function)
 * @param info
 * @return
 */
Value
Signer::SignWorker(const CallbackInfo& info)
{
  pdf_int32 minSigSize = info[0].As<Number>();
  Function cb = info[1].As<Function>();
  SignAsync* worker = new SignAsync(cb, *this, minSigSize);
  worker->Queue();
  return info.Env().Undefined();
}

/**
 * @brief The LoadCertificateAndKeyWorker class
 * Load the private key and certificate and set as properties of the Signer
 * object as EVP_PKEY and X509 respectively.
 */
class LoadCertificateAndKeyWorker : public AsyncWorker
{
public:
  Signer& signer;
  string cert;
  string key;
  string pwd;
  pdf_int32 minSigSize = 0;

  LoadCertificateAndKeyWorker(Function& cb,
                              Signer& signer,
                              string cert,
                              string key,
                              string pwd)
    : AsyncWorker(cb)
    , signer(signer)
    , cert(std::move(cert))
    , key(std::move(key))
    , pwd(std::move(pwd))
  {}

protected:
  pdf_int32 minimumSignerSize(FILE* file)
  {
    pdf_int32 size = 0;
    if (fseeko(file, 0, SEEK_END) != -1) {
      size += ftello(file);
    } else {
      size += 3072;
    }
    return size;
  }
  void Execute() override
  {
    FILE* file;

    // Load Certificate
    if (!(file = fopen(cert.c_str(), "rb"))) {
      SetError("Failed to open Certificate file");
      return;
    }
    signer.cert = PEM_read_X509(file, nullptr, nullptr, nullptr);
    minSigSize += minimumSignerSize(file);
    if (!signer.cert) {
      SetError("Failed to decode Certificate file");
    }
    fclose(file);

    // Load Private key
    if (!(file = fopen(key.c_str(), "rb"))) {
      SetError("Failed to open Private key file");
      X509_free(signer.cert);
      return;
    }
    signer.pkey =
      PEM_read_PrivateKey(file,
                          nullptr,
                          [](char* buf,
                             int bufSize,
                             int PODOFO_UNUSED_PARAM(rwflag),
                             void* userData) {
                            const char* password = static_cast<char*>(userData);
                            if (!password)
                              return 0;
                            auto res = static_cast<int>(strlen(password));
                            if (res > bufSize)
                              res = bufSize;
                            memcpy(buf, password, static_cast<size_t>(res));
                            return res;
                          },
                          static_cast<void*>(&pwd));
    if (!signer.pkey) {
      SetError("Failed to decode Private key file");
      X509_free(signer.cert);
      return;
    }
    minSigSize += minimumSignerSize(file);
    fclose(file);
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Undefined(), Number::New(Env(), minSigSize) });
  }
};

Value
Signer::LoadCertificateAndKey(const CallbackInfo& info)
{
  string cert, key, pwd;
  Function cb;

  vector<int> optsIndices = AssertCallbackInfo(
    info,
    { { 0, { option(napi_string) } },
      { 1, { option(napi_string) } },
      { 2, { option(napi_string), option(napi_function) } },
      { 3, { nullopt, option(napi_function) } } });

  cert = info[0].As<String>().Utf8Value();
  key = info[1].As<String>().Utf8Value();
  pwd = optsIndices[2] == 0 ? info[2].As<String>().Utf8Value() : std::string();
  cb = optsIndices[3] == 0 ? info[2].As<Function>() : info[3].As<Function>();

  AsyncWorker* worker =
    new LoadCertificateAndKeyWorker(cb, *this, cert, key, pwd);
  worker->Queue();
  return info.Env().Undefined();
}
}
