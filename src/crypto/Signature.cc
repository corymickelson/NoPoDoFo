//
// Created by red on 4/12/17.
//

#include "Signature.h"
#include "../ValidateArguments.h"

using namespace PoDoFo;
using namespace Napi;
using namespace std;

Napi::Value
NPDFSignatureData(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 3, { napi_string, napi_string, napi_string });

  auto signerPEMPath = info[0].As<String>().Utf8Value();
  auto privateKeyPemPath = info[1].As<String>().Utf8Value();
  auto pkeyPassword = info[2].As<String>().Utf8Value();

  X509* cert = nullptr;
  EVP_PKEY* pkey = nullptr;
  long signatureSize = 0;

  const auto flags = PKCS7_DETACHED | PKCS7_BINARY;

  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();
  ERR_load_PEM_strings();
  ERR_load_ASN1_strings();
  ERR_load_EVP_strings();

  FILE* fp;
  fp = fopen(signerPEMPath.c_str(), "rb");
  if (!fp) {
    throw Error::New(info.Env(), "Failed to open certificate file");
  }

  cert = PEM_read_X509(fp, nullptr, nullptr, nullptr);

  if (fseek(fp, 0, SEEK_END) != -1) {
    signatureSize += ftell(fp);
  } else {
    signatureSize += 3072;
  }

  fclose(fp);

  if (!cert) {
    throw Error::New(info.Env(), "Failed to decode certificate file");
  }
  fp = fopen(privateKeyPemPath.c_str(), "rb");
  if (!fp) {
    X509_free(cert);
    throw Error::New(info.Env(), "Failed to open private key file");
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
    const_cast<char*>(pkeyPassword.c_str()));

  if (fseek(fp, 0, SEEK_END) != -1) {
    signatureSize += ftell(fp);
  } else {
    signatureSize += 1024;
  }

  fclose(fp);

  if (!pkey) {
    X509_free(cert);
    cert = nullptr;
    throw Error::New(info.Env(), "Failed to decode private key");
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
    throw Error::New(info.Env(), "Out of memory");
  }

  BIO* mem = BIO_new(BIO_s_mem());
  if (!mem) {
    podofo_free(pBuffer);
    throw Error::New(info.Env(), "Failed to create input BIO");
  }

  PKCS7* p7 = PKCS7_sign(cert, pkey, nullptr, mem, flags);
  if (!p7) {
    BIO_free(mem);
    podofo_free(pBuffer);
    throw Error::New(info.Env(), "PKCS7 Sign Failed");
  }

  if (PKCS7_final(p7, mem, flags) <= 0) {
    PKCS7_free(p7);
    X509_free(cert);
    BIO_free(mem);
    throw Error::New(info.Env(), "Failed PKCS7 final");
  }

  auto out = BIO_new(BIO_s_mem());
  if (!out) {
    PKCS7_free(p7);
    X509_free(cert);
    BIO_free(mem);
    throw Error::New(info.Env(), "Failed to create output BIO");
  }

  char* outBuffer = nullptr;

  i2d_PKCS7_bio(out, p7);
  const auto outLen = BIO_get_mem_data(out, &outBuffer);
  Napi::Value data;
  if (outLen > 0 && outBuffer) {
    data = String::New(info.Env(), outBuffer);
    PKCS7_free(p7);
    BIO_free(out);
    BIO_free(mem);
  } else {
    PKCS7_free(p7);
    BIO_free(out);
    BIO_free(mem);
    throw Error::New(info.Env(), "Failed to get data from output BIO");
  }
  return data;
}
