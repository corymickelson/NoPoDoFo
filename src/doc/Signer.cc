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
#include "../base/Names.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Document.h"
#include "SignatureField.h"
#include "StreamDocument.h"
#include <map>
#include <vector>

using namespace Napi;
using namespace PoDoFo;

using std::string;

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
  delete pkey;
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
      InstanceMethod("sign", &Signer::SignWorker),
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
  SignAsync(Function& cb, Signer& self, string data)
    : AsyncWorker(cb)
    , self(self)
    , signature(std::move(data))
  {}

private:
  Signer& self;
  string signature;
  PdfRefCountedBuffer buffer;

  // AsyncWorker interface
protected:
  void Execute() override
  {
    try {
      PdfObject* acroform = self.doc.GetAcroForm(false)->GetObject();
      if(acroform->GetDictionary().HasKey(PdfName(Name::SIG_FLAGS))) {

      }
      const auto flags = PKCS7_DETACHED | PKCS7_BINARY;

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

      auto sigStrLength = static_cast<pdf_long>(signature.size());
      if (self.field->GetFieldName().GetStringUtf8().empty()) {
        self.field->SetFieldName("NoPoDoFo.SignatureField");
      }
      PdfOutputDevice outputDevice =
        self.output.empty() ? PdfOutputDevice(&buffer)
                            : PdfOutputDevice(self.output.c_str(), true);
      PdfSignOutputDevice signer(&outputDevice);
      signer.SetSignatureSize(static_cast<size_t>(sigStrLength));

      self.field->SetSignatureDate(PdfDate());
      self.field->SetSignature(*signer.GetSignatureBeacon());
      self.doc.WriteUpdate(&signer, true);

      if (!signer.HasSignaturePosition())
        throw Error::New(Env(),
                         "Cannot find signature position in the document data");

      signer.AdjustByteRange();
      signer.Seek(0);

      PdfData data(signature.c_str());
      signer.SetSignature(data);
      signer.Flush();
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
        msg << "Failed to write to: " << self.output
            << ", file existence check failed." << endl;
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
 * @note JS sign(signature:string|Buffer, cb: Function)
 * @param info
 * @return
 */
Value
Signer::SignWorker(const CallbackInfo& info)
{
  string data, output;
  Function cb = info[1].As<Function>();
  data = info[0].As<String>().Utf8Value();
  SignAsync* worker = new SignAsync(cb, *this, data);
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
    Callback().Call({ Env().Undefined(), Number::New(Env(), minSigSize) });
  }
};

Value
Signer::LoadCertificateAndKey(const CallbackInfo& info)
{
  string cert, key, pwd;
  Function cb;
  if (info.Length() == 4 && info[3].IsFunction()) {
    cb = info[3].As<Function>();
    pwd = info[2].As<String>().Utf8Value();
  } else if (info.Length() < 4 && info[2].IsFunction()) {
    cb = info[2].As<Function>();
  }
  cert = info[0].As<String>().Utf8Value();
  key = info[1].As<String>().Utf8Value();
  AsyncWorker* worker =
    new LoadCertificateAndKeyWorker(cb, *this, cert, key, pwd);
  worker->Queue();
  return info.Env().Undefined();
}
}
