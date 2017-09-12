//
// Created by red on 9/11/17.
//

#ifndef NPDF_SIGNATURE_H
#define NPDF_SIGNATURE_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;
class Signature : public Napi::ObjectWrap<Signature>
{
public:
  Signature(const CallbackInfo &callbackInfo);
  ~Signature();
  static Napi::FunctionReference constructor;
  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor = DefineClass(env,
                                      "Signature",
                                      {});
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Signature", constructor);
  }

};

#endif //NPDF_SIGNATURE_H
