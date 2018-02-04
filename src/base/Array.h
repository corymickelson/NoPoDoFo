//
// Created by red on 10/2/17
//
#ifndef NPDF_ARR_H
#define NPDF_ARR_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Array : public Napi::ObjectWrap<Array>
{
public:
  explicit Array(const Napi::CallbackInfo&);
  ~Array();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void Write(const Napi::CallbackInfo&);
  Napi::Value ToArray(const Napi::CallbackInfo&);
  Napi::Value Length(const Napi::CallbackInfo&);
  Napi::Value ContainsString(const Napi::CallbackInfo&);
  Napi::Value GetStringIndex(const Napi::CallbackInfo&);
  Napi::Value IsDirty(const Napi::CallbackInfo&);
  Napi::Value At(const Napi::CallbackInfo&);
  void SetDirty(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetImmutable(const Napi::CallbackInfo&);
  void SetImmutable(const Napi::CallbackInfo&, const Napi::Value&);
  void Push(const Napi::CallbackInfo&);
  Napi::Value Pop(const Napi::CallbackInfo&);
  void Clear(const Napi::CallbackInfo&);
  Napi::Value Eq(const Napi::CallbackInfo&);
  PoDoFo::PdfArray* GetArray() { return array; }

private:
  PoDoFo::PdfArray* array;
  Napi::Value GetObjAtIndex(const Napi::CallbackInfo&);
};
}
#endif
