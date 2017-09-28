//
// Created by red on 9/26/17
//

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class SignatureField : public ObjectWrap<SignatureField>
{
public:
  explicit SignatureField(const CallbackInfo&);
  static void Initialize(Napi::Env& env, Napi::Object& target) {}

private:
};
