//
// Created by red on 10/1/17.
//

#ifndef NPDF_STREAM_H
#define NPDF_STREAM_H

#include <napi.h>
#include <podofo/podofo.h>

class Stream : public Napi::ObjectWrap<Stream>
{
public:
  explicit Stream(const Napi::CallbackInfo& callbackInfo);
  ~Stream() = default;
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void Write(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfStream* stream;
};

#endif // NPDF_STREAM_H
