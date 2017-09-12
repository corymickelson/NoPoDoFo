//
// Created by red on 9/11/17.
//

#ifndef NPDF_IMAGE_H
#define NPDF_IMAGE_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class Image : public ObjectWrap<Image>
{
public:
  Image(const CallbackInfo &callbackInfo);
  ~Image();
  static FunctionReference constructor;
  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env,
                                "Image",
                                {});
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Image", constructor);
  }

};

#endif //NPDF_IMAGE_H
