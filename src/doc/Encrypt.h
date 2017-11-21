//
// Created by red on 10/04/17
//

#ifndef NPDF_ENCRYPT_H
#define NPDF_ENCRYPT_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Encrypt : public Napi::ObjectWrap<Encrypt>
{
public:
  explicit Encrypt(const Napi::CallbackInfo&);
  ~Encrypt();

  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  Napi::Value IsAllowed(const Napi::CallbackInfo&);
  Napi::Value Authenticate(const Napi::CallbackInfo&);
  Napi::Value GetOwnerValue(const Napi::CallbackInfo&);
  Napi::Value GetUserValue(const Napi::CallbackInfo&);
  Napi::Value GetPermissionValue(const Napi::CallbackInfo&);
  Napi::Value GetEncryptionKey(const Napi::CallbackInfo&);
  Napi::Value GetKeyLength(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfEncrypt* encrypt;
  PoDoFo::PdfMemDocument* document;
};
}
#endif
