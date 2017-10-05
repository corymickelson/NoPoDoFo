//
// Created by red on 9/26/17
//

#include <napi.h>
#include <podofo/podofo.h>

class SignatureField : public Napi::ObjectWrap<SignatureField>
{
public:
  explicit SignatureField(const Napi::CallbackInfo&);
  static void Initialize(Napi::Env& env, Napi::Object& target);

private:
};
