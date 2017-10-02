//
// Created by red on 10/1/17.
//

#ifndef NPDF_STREAM_H
#define NPDF_STREAM_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class Stream : public ObjectWrap<Stream> {
public:
  explicit Stream(const CallbackInfo &callbackInfo);
  ~Stream() = default;
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env &env, Napi::Object
  &target) {
    Function ctor = DefineClass(
        env,
        "Stream",
        { } );
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Stream", constructor);
  }
  void Write(const CallbackInfo &);
private:
  PdfStream* stream;
};

#endif //NPDF_STREAM_H
