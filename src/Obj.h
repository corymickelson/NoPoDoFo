//
// Created by red on 9/23/17
//

//#include "Dictionary.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class Obj : public ObjectWrap<Obj>
{
public:
  explicit Obj(const CallbackInfo&);
  ~Obj() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(
      env,
      "Obj",
      { InstanceAccessor("stream", &Obj::GetStream, nullptr),
        InstanceAccessor("type", &Obj::GetDataType, nullptr),
        InstanceAccessor("length", &Obj::GetObjectLength, nullptr),
        InstanceAccessor("owner", &Obj::GetOwner, &Obj::SetOwner),
        InstanceAccessor("reference", &Obj::Reference, nullptr),
        InstanceMethod("hasStream", &Obj::HasStream),
        InstanceMethod("getOffset", &Obj::GetByteOffset),
        InstanceMethod("write", &Obj::WriteObject),
        InstanceMethod("flateCompressStream", &Obj::FlateCompressStream),
        InstanceMethod("delayedStreamLoad", &Obj::DelayedStreamLoad) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Obj", constructor);
  }
  Napi::Value GetStream(const CallbackInfo&);
  Napi::Value HasStream(const CallbackInfo&);
  Napi::Value GetObjectLength(const CallbackInfo&);
  Napi::Value GetDataType(const CallbackInfo&);
  Napi::Value GetByteOffset(const CallbackInfo&);
  Napi::Value GetOwner(const CallbackInfo&);
  void SetOwner(const CallbackInfo&, const Napi::Value&);
  void WriteObject(const CallbackInfo&);
  Napi::Value Reference(const CallbackInfo&);
  void FlateCompressStream(const CallbackInfo&);
  void DelayedStreamLoad(const CallbackInfo&);

  const PdfObject GetObject() { return obj; }

private:
  PdfObject obj = *new PdfObject();
};
