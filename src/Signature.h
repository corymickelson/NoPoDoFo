//
// Created by red on 9/11/17.
//

#ifndef NPDF_SIGNATURE_H
#define NPDF_SIGNATURE_H

#include <boost/filesystem.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <napi.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <podofo/podofo.h>
#include <string>

#if defined(_WIN64)
#define fseeko _fseeki64
#define ftello _ftelli64
#else
#define fseeko fseek
#define ftello ftell
#endif

using namespace boost;
using namespace Napi;
using namespace PoDoFo;
using namespace std;

class Signature : public Napi::ObjectWrap<Signature>
{
public:
  Signature(const CallbackInfo& callbackInfo);
  ~Signature();
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor = DefineClass(
      env,
      "Signature",
      { InstanceMethod("applySignature", &Signature::ApplySignature) });

    target.Set("Signature", ctor);
  }

  void ApplySignature(const CallbackInfo&);

private:
  const string certFile;
  const string pKeyFile;
  const string password;
  const string reason;
};

#endif // NPDF_SIGNATURE_H
