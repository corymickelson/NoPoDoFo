//
// Created by red on 19/11/17.
//

#ifndef NPDF_VECTOR_H
#define NPDF_VECTOR_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo{
class Vector : public Napi::ObjectWrap<Vector> {
public:
  explicit Vector(const Napi::CallbackInfo &info);
  ~Vector();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env &env, Napi::Object &target);
  Napi::Value GetParentDocument(const Napi::CallbackInfo &info);
  void SetParentDocument(const Napi::CallbackInfo &info, const Napi::Value &value);
  void SetAutoDelete(const Napi::CallbackInfo &info);
  void SetCanReuseObjectNumber(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value GetCanReuseObjectNumber(const Napi::CallbackInfo &info);
  void Clear(const Napi::CallbackInfo &info);
  Napi::Value GetSize(const Napi::CallbackInfo &info);
  Napi::Value GetObjectCount(const Napi::CallbackInfo &info);
  Napi::Value GetObject(const Napi::CallbackInfo &info);
  Napi::Value GetIndex(const Napi::CallbackInfo &info);
  Napi::Value CreateObject(const Napi::CallbackInfo &info);
  Napi::Value RemoveObject(const Napi::CallbackInfo &info);
  Napi::Value CollectGarbage(const Napi::CallbackInfo &info);

private:
  PoDoFo::PdfDocument *document;
  PoDoFo::PdfVecObjects *vector;
};
}
#endif //NPDF_VECTOR_H
