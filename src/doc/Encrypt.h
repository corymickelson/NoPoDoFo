//
// Created by red on 10/04/17
//

#include <napi.h>
#include <podofo/podofo.h>

class Encrypt : public Napi::ObjectWrap<Encrypt>
{
public:
  explicit Encrypt(const Napi::CallbackInfo&);
  ~Encrypt()
  {
    delete encrypt;
    delete document;
  }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  Napi::Value IsAllowed(const Napi::CallbackInfo&);
  Napi::Value Authenticate(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfEncrypt* encrypt;
  PoDoFo::PdfMemDocument* document;
};
