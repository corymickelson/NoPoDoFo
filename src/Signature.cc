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

void
Signature::ApplySignature(const CallbackInfo& info)
{
  FILE* fp;
  X509** outCert;
  EVP_PKEY** outPKey;
  pdf_int32 minSize = 0;
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
}
