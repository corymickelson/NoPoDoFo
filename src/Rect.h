//
// Created by red on 9/11/17.
//

#ifndef NPDF_RECT_H
#define NPDF_RECT_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class Rect : public ObjectWrap<Rect>
{
public:
  Rect(const CallbackInfo &callbackInfo);
  ~Rect();
  static FunctionReference constructor;
  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env,
                                "Rect",
                                {});
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Rect", constructor);
  }

};

#endif //NPDF_RECT_H
