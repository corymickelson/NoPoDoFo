//
// Created by red on 9/23/17
//

#include <napi.h>
#include <podofo/podofo.h>

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
  void WriteObject(const Napi::CallbackInfo&);
  Napi::Value Reference(const Napi::CallbackInfo&);
  void FlateCompressStream(const Napi::CallbackInfo&);
  void DelayedStreamLoad(const Napi::CallbackInfo&);
  Napi::Value AsType(const Napi::CallbackInfo&);

  const PoDoFo::PdfObject GetObject() { return obj; }

  static Napi::Value ParseToType(const Napi::CallbackInfo&, PoDoFo::PdfObject&);

private:
  PoDoFo::PdfObject obj = *new PoDoFo::PdfObject();
};
