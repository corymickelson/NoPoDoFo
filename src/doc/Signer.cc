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
#include <map>
#include <sstream>
#include <vector>

using namespace Napi;
using namespace PoDoFo;

using std::string;
using std::stringstream;
using tl::nullopt;

#if defined(_WIN64)
#define fseeko _fseeki64
#define ftello _ftelli64
#else
#define fseeko fseek
#define ftello ftell
#endif

namespace NoPoDoFo {

FunctionReference Signer::Constructor; // NOLINT

/**
 * todo: StreamDocument support
 * @note JS new Signer(doc: Document, output?: string)
 * @param info
 */
Signer::Signer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , Doc(Document::Unwrap(info[0].As<Object>())->GetDocument())
{
  DbgLog = spdlog::get("DbgLog");
  if (info.Length() < 1) {
    Error::New(info.Env(), "Document required to construct Signer")
      .ThrowAsJavaScriptException();
    return;
  }
  if (!info[0].IsObject()) {
    TypeError::New(info.Env(), "Document required to construct Signer")
      .ThrowAsJavaScriptException();
    return;
  }
  if (info.Length() >= 2 && info[1].IsString()) {
    Output = info[1].As<String>().Utf8Value();
  }
}

Signer::~Signer()
{
  DbgLog->debug("Signer Cleanup");
  HandleScope scope(Env());
  if (Cert != nullptr) {
    X509_free(Cert);
  }
  if (Pkey) {
    EVP_PKEY_free(Pkey);
  }
  if (Chain) {
    sk_X509_free(Chain);
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
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Signer", ctor);
}

void
Signer::SetField(const CallbackInfo& info, const Napi::Value& value)
{
  Field = SignatureField::Unwrap(value.As<Object>())->GetField();
  try {
    Field->EnsureSignatureObject();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
Signer::GetField(const CallbackInfo& info)
{
  return SignatureField::Constructor.New(
    { External<PdfSignatureField>::New(info.Env(), Field.get()) });
}

class SignAsync : public AsyncWorker
{
public:
  SignAsync(Function& cb, Signer& self, pdf_int32 minSigSize)
    : AsyncWorker(cb)
    , Self(self)
    , MinSigSize(minSigSize)
  {}

private:
  Signer& Self;
  PdfRefCountedBuffer Buffer;
  pdf_int32 MinSigSize;

  // AsyncWorker interface
protected:
  void AppendData(vector<pdf_uint8> data)
  {
    SHA1_Update(&Self.ShaCtx, (const void*)&(data[0]), data.size());
  }
  void CreateSignature()
  {
    if (Self.Digest.empty()) {
      Self.Digest.resize(SHA_DIGEST_LENGTH);
      SHA1_Final(&(Self.Digest[0]), &Self.ShaCtx);
    }

    PKCS7* p7 = PKCS7_new();
    PKCS7_set_type(p7, NID_pkcs7_signed);

    PKCS7_SIGNER_INFO* p7Si =
      PKCS7_add_signature(p7, Self.Cert, Self.Pkey, EVP_sha1());
    PKCS7_add_attrib_content_type(p7Si, OBJ_nid2obj(NID_pkcs7_data));
    PKCS7_add0_attrib_signing_time(p7Si, nullptr);
    PKCS7_add1_attrib_digest(p7Si, &(Self.Digest[0]), Self.Digest.size());
    PKCS7_add_certificate(p7, Self.Cert);

    for (int c = 0; c < sk_X509_num(Self.Chain); c++) {
      X509* cert = sk_X509_value(Self.Chain, c);
      PKCS7_add_certificate(p7, cert);
    }
    PKCS7_set_detached(p7, 1);
    PKCS7_content_new(p7, NID_pkcs7_data);

    PKCS7_SIGNER_INFO_sign(p7Si);

    int p7Len = i2d_PKCS7(p7, nullptr);
    vector<unsigned char> result = std::vector<unsigned char>(p7Len);
    pdf_uint8* pP7Buf = &(result[0]);
    i2d_PKCS7(p7, &pP7Buf);

    PKCS7_free(p7);
  }
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
      PdfObject* acroform = Self.Doc.GetAcroForm(false)->GetObject();
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
        Self.Output.empty() ? PdfOutputDevice(&Buffer)
                            : PdfOutputDevice(Self.Output.c_str(), true);
      PdfSignOutputDevice signer(&outputDevice);

      // minimally ensure signature field name property is filled, defaults to
      // "NoPoDoFo.SignatureField"
      if (Self.Field->GetFieldName().GetStringUtf8().empty()) {
        Self.Field->SetFieldName("NoPoDoFo.SignatureField");
      }

      // Set Signing Date if empty
      if (!Self.Field->GetSignatureObject()->GetDictionary().HasKey(Name::M)) {
        PdfDate now;
        PdfString str;
        now.ToString(str);
        Self.DbgLog->debug("Signer::SignAsync::Execute SignatureField Date "
                           "Null, setting to now: {}",
                           str.GetStringUtf8());
        Self.Field->SetSignatureDate(now);
      }

      // Set output device to write signature to designated area.
      signer.SetSignatureSize(static_cast<size_t>(MinSigSize));

      Self.Field->SetSignature(*signer.GetSignatureBeacon());
      Self.Doc.WriteUpdate(&signer, true);

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
        if (!sigBuffer) {
          break;
        }
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
        Self.Cert, Self.Pkey, nullptr, memory, PKCS7_DETACHED | PKCS7_BINARY);
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
    if (!Self.Output.empty()) {
      if (FILE* file = fopen(Self.Output.c_str(), "rb")) {
        fclose(file);
        Callback().Call({ Env().Undefined() });
      } else {
        stringstream msg;
        msg << "Failed to write to: " << Self.Output << endl;
        Callback().Call({ String::New(Env(), msg.str()) });
      }
    } else {
      Callback().Call({ Env().Undefined(),
                        Napi::Buffer<char>::Copy(
                          Env(), Buffer.GetBuffer(), Buffer.GetSize()) });
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
  auto* worker = new SignAsync(cb, *this, minSigSize);
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
  Signer& Self;
  string CertFile{};
  string KeyFile{};
  string Pwd{};
  char* CertBuffer = nullptr;
  char* KeyBuffer = nullptr;
  size_t CertLength = 0;
  size_t KeyLength = 0;
  pdf_int32 MinSigSize = 0;

  LoadCertificateAndKeyWorker(Function& cb,
                              Signer& signer,
                              string cert,
                              string key,
                              string pwd)
    : AsyncWorker(cb)
    , Self(signer)
    , CertFile(std::move(cert))
    , KeyFile(std::move(key))
    , Pwd(std::move(pwd))
  {}
  LoadCertificateAndKeyWorker(Function& cb,
                              Signer& signer,
                              char* cert,
                              char* key,
                              size_t certLength,
                              size_t keyLength,
                              string pwd)
    : AsyncWorker(cb, "load-certificate-and-key")
    , Self(signer)
    , CertBuffer(cert)
    , KeyBuffer(key)
    , CertLength(certLength)
    , KeyLength(keyLength)
    , Pwd(std::move(pwd))
  {}
  LoadCertificateAndKeyWorker(Function& cb,
                              Signer& signer,
                              char* cert,
                              size_t certLength,
                              string pwd)
    : AsyncWorker(cb, "load-certificate-and-key")
    , Self(signer)
    , CertBuffer(cert)
    , CertLength(certLength)
    , Pwd(std::move(pwd))
  {}
  LoadCertificateAndKeyWorker(Function& cb,
                              Signer& signer,
                              string certFile,
                              string pwd)
    : AsyncWorker(cb, "load-certificate-and-key")
    , Self(signer)
    , CertFile(std::move(certFile))
    , Pwd(std::move(pwd))
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
    if (CertLength > 0 &&
        KeyLength > 0) { // PKCS7 Certificate File and Private Key File
      LoadPKCS7();
    } else if (!CertFile.empty() &&
               !KeyFile
                  .empty()) { // PKCS7 Certificate Buffer and Private Key Buffer
      LoadPKCS7();
    } else if (CertLength > 0 && KeyLength == 0) { // PKCS12 Certificate Buffer
      LoadPKCS12();
    } else if (!CertFile.empty() &&
               KeyFile.empty()) { // PKCS12 Certificate File
      LoadPKCS12();
    } else {
      SetError("Signature does not match any Signer loanCertificateAndKey "
               "options, please see the docs");
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Undefined(), Number::New(Env(), MinSigSize) });
  }

private:
  std::vector<unsigned char> result;
  void LoadPKCS7()
  {
    FILE* file = nullptr;
    BIO* buffer = nullptr;

    // Load Certificate
    if (CertLength > 0) {
      buffer = BIO_new_mem_buf(CertBuffer, CertLength);
      Self.Cert = PEM_read_bio_X509(buffer, nullptr, nullptr, nullptr);
      MinSigSize += CertLength;
    } else {
      if (!(file = fopen(CertFile.c_str(), "rb"))) {
        SetError("Failed to open Certificate file");
        return;
      }
      Self.Cert = PEM_read_X509(file, nullptr, nullptr, nullptr);
      MinSigSize += minimumSignerSize(file);
    }

    if (!Self.Cert) {
      SetError("Failed to decode Certificate file");
    }
    if (file != nullptr) {
      fclose(file);
    } else {
      BIO_free(buffer);
    }

    // Load Private key
    auto pkeyCallback = [](char* buf,
                           int bufSize,
                           int PODOFO_UNUSED_PARAM(rwflag),
                           void* userData) {
      const char* password = static_cast<char*>(userData);
      if (!password) {
        return 0;
      }
      auto res = static_cast<int>(strlen(password));
      if (res > bufSize) {
        res = bufSize;
      }
      memcpy(buf, password, static_cast<size_t>(res));
      return res;
    };
    if (KeyLength > 0) {
      buffer = BIO_new(BIO_s_mem());
      int l = BIO_write(buffer, KeyBuffer, KeyLength);
      //			buffer = BIO_new_mem_buf(KeyBuffer, KeyLength);
      Self.Pkey = PEM_read_bio_PrivateKey(
        buffer, nullptr, pkeyCallback, static_cast<void*>(&Pwd));
      MinSigSize += KeyLength;
    } else {
      if (!(file = fopen(KeyFile.c_str(), "rb"))) {
        SetError("Failed to open Private key file");
        X509_free(Self.Cert);
        return;
      }
      Self.Pkey = PEM_read_PrivateKey(
        file, nullptr, pkeyCallback, static_cast<void*>(&Pwd));
      MinSigSize += minimumSignerSize(file);
    }

    if (!Self.Pkey) {
      SetError("Failed to decode Private key file");
      X509_free(Self.Cert);
      return;
    }
    if (file != nullptr) {
      fclose(file);
    } else {
      BIO_free(buffer);
    }
  }
  void LoadPKCS12()
  {
    FILE* file = nullptr;
    BIO* buffer = nullptr;
    PKCS12* p12 = nullptr;
    PKCS7* p7 = nullptr;
    // Load Certificate
    if (CertLength > 0) {
      buffer = BIO_new_mem_buf(CertBuffer, CertLength);
      p12 = d2i_PKCS12_bio(buffer, nullptr);
      MinSigSize += CertLength;
    } else {
      if (!(file = fopen(CertFile.c_str(), "rb"))) {
        SetError("Failed to open Certificate file");
        return;
      }
      p12 = d2i_PKCS12_fp(file, nullptr);
      MinSigSize += minimumSignerSize(file);
      fclose(file);
    }
    if (!p12) {
      SetError("Can not parse private key");
    }
    int parseResult =
      PKCS12_parse(p12, Pwd.c_str(), &Self.Pkey, &Self.Cert, &Self.Chain);
    PKCS12_free(p12);
    if (!parseResult) {
      SetError("Can not parse private key");
    }
    Self.Digest.resize(0);
    Self.Digest.clear();
    SHA1_Init(&Self.ShaCtx);
  }
};

Value
Signer::LoadCertificateAndKey(const CallbackInfo& info)
{
  string certFile, keyFile, pwd;
  Napi::Buffer<char> certBuffer, keyBuffer;
  auto useBuffers = false;

  Function cb;
  Napi::Object opt;
  bool isP12 = false;
  const auto optCertificateType = "certificateType";
  const auto optPassword = "password";
  vector<int> optsIndices =
    AssertCallbackInfo(info,
                       { { 0, // certificate
                           { option(napi_string), option(napi_object) } },
                         { 1, // private key or options object
                           { option(napi_string), option(napi_object) } },
                         { 2, // opts object or callback
                           { option(napi_object), option(napi_function) } },
                         { 3, // callback function
                           { nullopt, option(napi_function) } } });

  if (optsIndices[0] == 0) {
    certFile = info[0].As<String>().Utf8Value();
  } else if (info[0].IsBuffer()) {
    certBuffer = info[0].As<Buffer<char>>();
  } else {
    TypeError::New(info.Env(), "A string(filepath) or buffer is required")
      .ThrowAsJavaScriptException();
    return {};
  }
  if (optsIndices[1] == 0) {
    keyFile = info[1].As<String>().Utf8Value();
  } else if (info[1].IsBuffer()) {
    keyBuffer = info[1].As<Buffer<char>>();
    useBuffers = true;
  } else if (info[0].IsObject() &&
             (info[0].As<Object>().Has(optCertificateType) ||
              info[0].As<Object>().Has(optPassword))) {
    opt = info[0].As<Object>();
  } else {
    TypeError::New(info.Env(), "A string(filepath) or buffer is required")
      .ThrowAsJavaScriptException();
    return {};
  }
  if (optsIndices[2] == 0 &&
      (info[0].IsObject() && (info[0].As<Object>().Has(optCertificateType) ||
                              info[0].As<Object>().Has(optPassword)))) {
    opt = info[0].As<Object>();
  }
  if (!opt.IsEmpty()) {
    if (opt.Has(optPassword)) {
      pwd = opt.Get(optPassword).As<String>().Utf8Value();
    }
    if (opt.Has(optCertificateType)) {
      switch (opt.Get(optCertificateType).As<Number>().Uint32Value()) {
        case 0: // PKCS7
          isP12 = false;
          break;
        case 1: // PKCS12
          isP12 = true;
          break;
        default:
          isP12 = false;
      }
    }
  }
  cb = optsIndices[3] == 0 ? info[2].As<Function>() : info[3].As<Function>();

  AsyncWorker* worker;
  if (!certFile.empty() && !keyFile.empty()) {
    // PKCS7 cert and private key files
    worker = new LoadCertificateAndKeyWorker(cb, *this, certFile, keyFile, pwd);
  } else if (!isP12 && useBuffers && !certBuffer.IsEmpty() &&
             !keyBuffer.IsEmpty()) {
    // PKCS7 cert and private key buffer
    worker = new LoadCertificateAndKeyWorker(cb,
                                             *this,
                                             certBuffer.Data(),
                                             keyBuffer.Data(),
                                             certBuffer.Length(),
                                             keyBuffer.Length(),
                                             pwd);
  } else if (isP12 && useBuffers) {
    // PKCS12 certificate buffer
    worker = new LoadCertificateAndKeyWorker(
      cb, *this, certBuffer.Data(), certBuffer.Length(), pwd);
  } else if (isP12) {
    // PKCS12 certificate file
    worker = new LoadCertificateAndKeyWorker(cb, *this, certFile, pwd);
  } else {
    Error::New(info.Env(),
               "Parameters do not match function signature. Please view "
               "signer.loadCertAndKey docs")
      .ThrowAsJavaScriptException();
    return {};
  }
  worker->Queue();
  return info.Env().Undefined();
}

}

/**
 * @brief
 * #if (!USE_STD_SIGNATURE_HANDLER)
// OpenSSL includes
#include &lt;openssl/err.h&gt;
#include &lt;openssl/evp.h&gt;
#include &lt;openssl/pkcs12.h&gt;
#include &lt;openssl/pkcs7.h&gt;
#include &lt;openssl/rsa.h&gt;
#include &lt;openssl/sha.h&gt;
#endif // (!USE_STD_SIGNATURE_HANDLER)

using namespace std;
using namespace pdftron;
using namespace pdftron::SDF;
using namespace pdftron::PDF::Annots;
using namespace pdftron::PDF;

#if (!USE_STD_SIGNATURE_HANDLER)
//
// Extend SignatureHandler by using OpenSSL signing utilities.
//
class OpenSSLSignatureHandler : public SignatureHandler
{
public:
    OpenSSLSignatureHandler(const char* in_pfxfile, const char* in_password) :
m_pfxfile(in_pfxfile), m_password(in_password)
    {
        FILE* fp = fopen(in_pfxfile, "rb");
        if (fp == NULL)
            throw (Common::Exception("Cannot open private key.", __LINE__,
__FILE__, "PKCS7Signature::PKCS7Signature", "Cannot open private key."));

        PKCS12* p12 = d2i_PKCS12_fp(fp, NULL);
        fclose(fp);

        if (p12 == NULL)
            throw (Common::Exception("Cannot parse private key.", __LINE__,
__FILE__, "PKCS7Signature::PKCS7Signature", "Cannot parse private key."));

        mp_pkey = NULL;
        mp_x509 = NULL;
        mp_ca = NULL;
        int parseResult = PKCS12_parse(p12, in_password, &mp_pkey, &mp_x509,
&mp_ca); PKCS12_free(p12);

        if (parseResult == 0)
            throw (Common::Exception("Cannot parse private key.", __LINE__,
__FILE__, "PKCS7Signature::PKCS7Signature", "Cannot parse private key."));

        Reset();
    }

    virtual UString GetName() const
    {
        return (UString("Adobe.PPKLite"));
    }

    virtual void AppendData(const std::vector&lt;pdftron::UInt8&gt;& in_data)
    {
        SHA1_Update(&m_sha_ctx, (const void*) &(in_data[0]), in_data.size());
        return;
    }

    virtual bool Reset()
    {
        m_digest.resize(0);
        m_digest.clear();
        SHA1_Init(&m_sha_ctx);
        return (true);
    }

    virtual std::vector&lt;pdftron::UInt8&gt; CreateSignature()
    {
        if (m_digest.size() == 0) {
            m_digest.resize(SHA_DIGEST_LENGTH);
            SHA1_Final(&(m_digest[0]), &m_sha_ctx);
        }

        PKCS7* p7 = PKCS7_new();
        PKCS7_set_type(p7, NID_pkcs7_signed);

        PKCS7_SIGNER_INFO* p7Si = PKCS7_add_signature(p7, mp_x509, mp_pkey,
EVP_sha1()); PKCS7_add_attrib_content_type(p7Si, OBJ_nid2obj(NID_pkcs7_data));
        PKCS7_add0_attrib_signing_time(p7Si, NULL);
        PKCS7_add1_attrib_digest(p7Si, &(m_digest[0]), m_digest.size());
        PKCS7_add_certificate(p7, mp_x509);

        for (int c = 0; c &lt; sk_X509_num(mp_ca); c++) {
            X509* cert = sk_X509_value(mp_ca, c);
            PKCS7_add_certificate(p7, cert);
        }
        PKCS7_set_detached(p7, 1);
        PKCS7_content_new(p7, NID_pkcs7_data);

        PKCS7_SIGNER_INFO_sign(p7Si);

        int p7Len = i2d_PKCS7(p7, NULL);
        std::vector&lt;unsigned char&gt; result(p7Len);
        UInt8* pP7Buf = &(result[0]);
        i2d_PKCS7(p7, &pP7Buf);

        PKCS7_free(p7);

        return (result);
    }

    virtual OpenSSLSignatureHandler* Clone() const
    {
        return (new OpenSSLSignatureHandler(m_pfxfile.c_str(),
m_password.c_str()));
    }

    virtual ~OpenSSLSignatureHandler()
    {
        sk_X509_free(mp_ca);
        X509_free(mp_x509);
        EVP_PKEY_free(mp_pkey);
    }

private:
    std::vector&lt;UInt8&gt; m_digest;
    std::string m_pfxfile;
    std::string m_password;

    SHA_CTX m_sha_ctx;
    EVP_PKEY* mp_pkey;      // private key
    X509* mp_x509;          // signing certificate
    STACK_OF(X509)* mp_ca;  // certificate chain up to the CA
}; // class OpenSSLSignatureHandler
#endif // (!USE_STD_SIGNATURE_HANDLER)

//
// This functions add an approval signature to the PDF document. The original
PDF document contains a blank form field
// that is prepared for a user to sign. The following code demonstrate how to
sign this document using PDFNet.
//
bool SignPDF(void)
{
    UString infile("../../TestFiles/doc_to_sign.pdf");
    UString outfile("../../TestFiles/Output/signed_doc.pdf");
    UString certfile("../../TestFiles/pdftron.pfx");
    UString imagefile("../../TestFiles/signature.jpg");
    bool result = true;

    try {
        cout &lt;&lt; "Signing PDF document: \"" &lt;&lt; infile &lt;&lt;
"\".\n";
        // Open an existing PDF
        PDFDoc doc(infile);

        // Add an StdSignatureHandler instance to PDFDoc, making sure to keep
track of it using the ID returned. SDF::SignatureHandlerId sigHandlerId =
doc.AddStdSignatureHandler(certfile, UString("password"));
        // When using OpenSSLSignatureHandler class, uncomment the following
lines and comment the line above.
        // Create a new instance of the SignatureHandler.
        //OpenSSLSignatureHandler sigHandler(certfile.ConvertToUtf8().c_str(),
"password");
        // Add the SignatureHandler instance to PDFDoc, making sure to keep
track of it using the ID returned.
        //SDF::SignatureHandlerId sigHandlerId =
doc.AddSignatureHandler(sigHandler);

        // Obtain the signature form field from the PDFDoc via Annotation.
        Field sigField = doc.GetField(UString("Signature1"));
        Widget widgetAnnot(sigField.GetSDFObj());

        // Tell PDFNetC to use the SignatureHandler created to sign the new
signature form field. SDF::Obj sigDict =
sigField.UseSignatureHandler(sigHandlerId);

        // Add more information to the signature dictionary.
        //sigDict.PutName("SubFilter", "adbe.pkcs7.detached");
        sigDict.PutString("Name", "PDFTron");
        sigDict.PutString("Location", "Vancouver, BC");
        sigDict.PutString("Reason", "Document verification.");

        // Add the signature appearance.
        ElementWriter apWriter;
        ElementBuilder apBuilder;
        apWriter.Begin(doc);
        Image sigImg = Image::Create(doc, imagefile);
        double w = sigImg.GetImageWidth(), h = sigImg.GetImageHeight();
        Element apElement =  apBuilder.CreateImage(sigImg, 0, 0, w, h);
        apWriter.WritePlacedElement(apElement);
        SDF::Obj apObj = apWriter.End();
        apObj.PutRect("BBox", 0, 0, w, h);
        apObj.PutName("Subtype", "Form");
        apObj.PutName("Type", "XObject");
        apWriter.Begin(doc);
        apElement = apBuilder.CreateForm(apObj);
        apWriter.WritePlacedElement(apElement);
        apObj = apWriter.End();
        apObj.PutRect("BBox", 0, 0, w, h);
        apObj.PutName("Subtype", "Form");
        apObj.PutName("Type", "XObject");

        widgetAnnot.SetAppearance(apObj);
        widgetAnnot.RefreshAppearance();

        // Save the PDFDoc. Once the method below is called, PDFNetC will also
sign the document using the information
        // provided.
        doc.Save(outfile, 0, NULL);

        cout &lt;&lt; "Finished signing PDF document.\n";
    }
    catch (Common::Exception& e) {
        cerr &lt;&lt; e &lt;&lt; "\n";
        result = false;
    }
    catch (exception& e) {
        cerr &lt;&lt; e.what() &lt;&lt; "\n";
        result = false;
    }
    catch (...) {
        cerr &lt;&lt; "Unknown exception.\n";
        result = false;
    }

    return (result);
}

//
// Adds a certification signature to the PDF document. Certifying a document is
like notarizing a document. Unlike
// approval signatures, there can be only one certification per PDF document.
Only the first signature in the PDF
// document can be used as the certification signature. The process of
certifying a document is almost exactly the same
// as adding approval signatures with the exception of certification signatures
requires an entry in the "Perms"
// dictionary.
//
bool CertifyPDF(void)
{
    UString infile("../../TestFiles/newsletter.pdf");
    UString outfile("../../TestFiles/Output/newsletter_certified.pdf");
    UString certfile("../../TestFiles/pdftron.pfx");
    bool result = true;

    try {
        cout &lt;&lt; "Certifying PDF document: \"" &lt;&lt; infile &lt;&lt;
"\".\n";
        // Open an existing PDF
        PDFDoc doc(infile);

        // Add an StdSignatureHandler instance to PDFDoc, making sure to keep
track of it using the ID returned. SDF::SignatureHandlerId sigHandlerId =
doc.AddStdSignatureHandler(certfile, UString("password"));
        // When using OpenSSLSignatureHandler class, uncomment the following
lines and comment the line above.
        // Create a new instance of the SignatureHandler.
        //OpenSSLSignatureHandler sigHandler(certfile.ConvertToUtf8().c_str(),
"password");
        // Add the SignatureHandler instance to PDFDoc, making sure to keep
track of it using the ID returned.
        //SDF::SignatureHandlerId sigHandlerId =
doc.AddSignatureHandler(sigHandler);

        // Create new signature form field in the PDFDoc.
        Field sigField = doc.FieldCreate(UString("Signature1"),
Field::e_signature);

        Page page1 = doc.GetPage(1);
        Widget widgetAnnot =
pdftron::PDF::Annots::Widget::Create(doc.GetSDFDoc(), Rect(0, 0, 0, 0),
sigField); page1.AnnotPushBack(widgetAnnot); widgetAnnot.SetPage(page1);
        SDF::Obj widgetObj = widgetAnnot.GetSDFObj();
        widgetObj.PutNumber("F", 132);
        widgetObj.PutName("Type", "Annot");

        // Tell PDFNetC to use the SignatureHandler created to sign the new
signature form field. SDF::Obj sigDict =
sigField.UseSignatureHandler(sigHandlerId);

        // Add more information to the signature dictionary.
        sigDict.PutName("SubFilter", "adbe.pkcs7.detached");
        sigDict.PutString("Name", "PDFTron");
        sigDict.PutString("Location", "Vancouver, BC");
        sigDict.PutString("Reason", "Document verification.");

        // Appearance can be added to the widget annotation. Please see the
"SignPDF()" function for details.

        // Add this sigDict as DocMDP in Perms dictionary from root
        SDF::Obj root = doc.GetRoot();
        SDF::Obj perms = root.PutDict("Perms");
        // add the sigDict as DocMDP (indirect) in Perms
        perms.Put("DocMDP", sigDict);

        // add the additional DocMDP transform params
        SDF::Obj refObj = sigDict.PutArray("Reference");
        SDF::Obj transform = refObj.PushBackDict();
        transform.PutName("TransformMethod", "DocMDP");
        transform.PutName("Type", "SigRef");
        SDF::Obj transformParams = transform.PutDict("TransformParams");
        transformParams.PutNumber("P", 1); // Set permissions as necessary.
        transformParams.PutName("Type", "TransformParams");
        transformParams.PutName("V", "1.2");

        // Save the PDFDoc. Once the method below is called, PDFNetC will also
sign the document using the information
        // provided.
        doc.Save(outfile, 0, NULL);

        cout &lt;&lt; "Finished certifying PDF document.\n";
    }
    catch (Common::Exception& e) {
        cerr &lt;&lt; e &lt;&lt; "\n";
        result = false;
    }
    catch (exception& e) {
        cerr &lt;&lt; e.what() &lt;&lt; "\n";
        result = false;
    }
    catch (...) {
        cerr &lt;&lt; "Unknown exception.\n";
        result = false;
    }

    return (result);
}

int main(int argc, char** argv)
{
    // Initialize PDFNetC
    PDFNet::Initialize();

#if (!USE_STD_SIGNATURE_HANDLER)
    // Initialize OpenSSL library
    CRYPTO_malloc_init();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
#endif // (!USE_STD_SIGNATURE_HANDLER)

    bool result = true;

    if (!SignPDF())
        result = false;

    if (!CertifyPDF())
        result = false;

    // Clean up code
    PDFNet::Terminate();

#if (!USE_STD_SIGNATURE_HANDLER)
    ERR_free_strings();
    EVP_cleanup();
#endif // (!USE_STD_SIGNATURE_HANDLER)

    if (!result) {
        cout &lt;&lt; "\nTests failed.\n";
        return 1;
    }

    cout &lt;&lt; "\nAll tests passed.\n";

    return 0;
}
 */
