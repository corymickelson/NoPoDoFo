//
// Created by red on 9/23/17
//

#ifndef NPDF_OBJ_H
#define NPDF_OBJ_H

#include <napi.h>
#include <podofo/podofo.h>

#include <utility>

using namespace std;

class Obj : public Napi::ObjectWrap<Obj>
{
public:
  explicit Obj(const Napi::CallbackInfo&);
  ~Obj() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetStream(const Napi::CallbackInfo&);
  Napi::Value HasStream(const Napi::CallbackInfo&);
  Napi::Value GetObjectLength(const Napi::CallbackInfo&);
  Napi::Value GetDataType(const Napi::CallbackInfo&);
  Napi::Value GetByteOffset(const Napi::CallbackInfo&);
  Napi::Value GetOwner(const Napi::CallbackInfo&);
  void SetOwner(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value Write(const Napi::CallbackInfo&);
  Napi::Value Reference(const Napi::CallbackInfo&);
  void FlateCompressStream(const Napi::CallbackInfo&);
  void DelayedStreamLoad(const Napi::CallbackInfo&);
  Napi::Value GetNumber(const Napi::CallbackInfo&);
  Napi::Value GetReal(const Napi::CallbackInfo&);
  Napi::Value GetString(const Napi::CallbackInfo&);
  Napi::Value GetName(const Napi::CallbackInfo&);
  Napi::Value GetArray(const Napi::CallbackInfo&);
  Napi::Value GetBool(const Napi::CallbackInfo&);
  Napi::Value GetReference(const Napi::CallbackInfo&);
  Napi::Value GetRawData(const Napi::CallbackInfo&);

  const PoDoFo::PdfObject GetObject() { return obj; }

private:
  PoDoFo::PdfObject obj = *new PoDoFo::PdfObject();
};

#endif
