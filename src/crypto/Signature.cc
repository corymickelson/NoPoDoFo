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

#include "Signature.h"
#include <utility> #include "../ValidateArguments.h"

using namespace PoDoFo;
using namespace Napi;

using std::string;

std::string
NPDFSignatureData(Napi::Env env,
                  const std::string pub,
                  const std::string priv,
                  const std::string pwd)
{
  X509* cert = nullptr;
  EVP_PKEY* pkey = nullptr;
  long signatureSize = 0;

  const auto flags = PKCS7_DETACHED | PKCS7_BINARY;

#if OPENSSL_VERSION_NUMBER < 0x10100000L
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();
  ERR_load_PEM_strings();
  ERR_load_ASN1_strings();
  ERR_load_EVP_strings();
#else
  OPENSSL_init_ssl(0, NULL);
  OPENSSL_init();
#endif

  FILE* fp;
  fp = fopen(pub.c_str(), "rb");
  if (!fp) {
    throw Error::New(env, "Failed to open certificate file");
  }

  cert = PEM_read_X509(fp, nullptr, nullptr, nullptr);

  if (fseek(fp, 0, SEEK_END) != -1) {
    signatureSize += ftell(fp);
  } else {
    signatureSize += 3072;
  }

  fclose(fp);

  if (!cert) {
    throw Error::New(env, "Failed to decode certificate file");
  }
  fp = fopen(priv.c_str(), "rb");
  if (!fp) {
    X509_free(cert);
    throw Error::New(env, "Failed to open private key file");
  }

  pkey = PEM_read_PrivateKey(
    fp,
    nullptr,
    [](
      char* buf, int bufSize, int PODOFO_UNUSED_PARAM(rwflag), void* userData) {
      const auto* password = reinterpret_cast<const char*>(userData);
      if (!password)
        return 0;
      auto res = static_cast<int>(strlen(password));
      if (res > bufSize)
        res = bufSize;
      memcpy(buf, password, static_cast<size_t>(res));
      return res;
    },
    const_cast<char*>(pwd.c_str()));

  if (fseek(fp, 0, SEEK_END) != -1) {
    signatureSize += ftell(fp);
  } else {
    signatureSize += 1024;
  }

  fclose(fp);

  if (!pkey) {
    X509_free(cert);
    cert = nullptr;
    throw Error::New(env, "Failed to decode private key");
  }

  unsigned int uBufferLen = 65535, len;
  char* pBuffer;

  while (pBuffer =
           reinterpret_cast<char*>(podofo_malloc(sizeof(char) * uBufferLen)),
         !pBuffer) {
    uBufferLen = uBufferLen / 2;
    if (!uBufferLen) {
      break;
    }
  }

  if (!pBuffer) {
    throw Error::New(env, "Out of memory");
  }

  BIO* mem = BIO_new(BIO_s_mem());
  if (!mem) {
    podofo_free(pBuffer);
    throw Error::New(env, "Failed to create input BIO");
  }

  PKCS7* p7 = PKCS7_sign(cert, pkey, nullptr, mem, flags);
  if (!p7) {
    BIO_free(mem);
    podofo_free(pBuffer);
    throw Error::New(env, "PKCS7 Sign Failed");
  }

  if (PKCS7_final(p7, mem, flags) <= 0) {
    PKCS7_free(p7);
    X509_free(cert);
    BIO_free(mem);
    throw Error::New(env, "Failed PKCS7 final");
  }

  auto out = BIO_new(BIO_s_mem());
  if (!out) {
    PKCS7_free(p7);
    X509_free(cert);
    BIO_free(mem);
    throw Error::New(env, "Failed to create output BIO");
  }

  char* outBuffer = nullptr;

  i2d_PKCS7_bio(out, p7);
  const auto outLen = BIO_get_mem_data(out, &outBuffer);
  if (outLen > 0 && outBuffer) {
  } else {
    Error::New(env, "Failed to get data from output BIO")
      .ThrowAsJavaScriptException();
  }
  std::string sign(outBuffer);
  PKCS7_free(p7);
  BIO_free(out);
  BIO_free(mem);
  return sign;
}

Napi::Value
NPDFSignatureDataSync(const Napi::CallbackInfo& info)
{
  auto signerPEMPath = info[0].As<String>().Utf8Value();
  auto privateKeyPemPath = info[1].As<String>().Utf8Value();
  auto pkeyPassword = info[2].As<String>().Utf8Value();
  auto value = NPDFSignatureData(
    info.Env(), signerPEMPath, privateKeyPemPath, pkeyPassword);
  return String::New(info.Env(), value);
}

class SignatureWorker : public AsyncWorker
{
public:
  SignatureWorker(Function& cb, string pub, string priv, string pwd)
    : AsyncWorker(cb)
    , signerPEMPath(std::move(pub))
    , privateKeyPemPath(std::move(priv))
    , pkeyPassword(std::move(pwd))
  {}

private:
  string signerPEMPath;
  string privateKeyPemPath;
  string pkeyPassword;
  string value;

  // AsyncWorker interface
protected:
  void Execute() override
  {
    try {
      value = NPDFSignatureData(
        Env(), signerPEMPath, privateKeyPemPath, pkeyPassword);
    } catch (...) {
      Error::New(Env(), "Async Signature failed.").ThrowAsJavaScriptException();
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), value) });
  }
};

Napi::Value
NPDFSignatureDataAsync(const CallbackInfo& info)
{
  auto signerPEMPath = info[0].As<String>().Utf8Value();
  auto privateKeyPemPath = info[1].As<String>().Utf8Value();
  auto pkeyPassword = info[2].As<String>().Utf8Value();
  auto cb = info[3].As<Function>();
  auto worker =
    new SignatureWorker(cb, signerPEMPath, privateKeyPemPath, pkeyPassword);
  worker->Queue();
  return info.Env().Undefined();
}
