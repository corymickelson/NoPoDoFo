//
// Created by red on 9/11/17.
//

#ifndef NPDF_ANNOTATION_H
#define NPDF_ANNOTATION_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class Annotation : public ObjectWrap<Annotation>
{
public:
  Annotation(const CallbackInfo &callbackInfo);
  ~Annotation();
  static FunctionReference constructor;
  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env,
                                "Annotation",
                                {});
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Annotation", constructor);
  }

};
#endif //NPDF_ANNOTATION_H
