//
// Created by red on 10/1/17
//

#ifndef NPDF_REF_H
#define NPDF_REF_H

#include <napi.h>
#include <podofo/podofo.h>

class Ref : public Napi::ObjectWrap<Ref>
{
public:
  explicit Ref(const Napi::CallbackInfo&);
  ~Ref() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void SetGenerationNumber(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetGenerationNumber(const Napi::CallbackInfo&);
  void SetObjectNumber(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetObjectNumber(const Napi::CallbackInfo&);

  Napi::Value ToString(const Napi::CallbackInfo&);
  void Write(const Napi::CallbackInfo&);
  Napi::Value IsIndirect(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfReference ref = *new PoDoFo::PdfReference();
};

#endif
