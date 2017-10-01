//
// Created by red on 9/23/17
//

//#include "Dictionary.h"
#include "ErrorHandler.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class NObject : public ObjectWrap<NObject>
{
public:
  explicit NObject(const CallbackInfo&);
  ~NObject() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(
      env,
      "PDObject",
      { InstanceAccessor("stream", &NObject::GetStream, nullptr),
        InstanceAccessor("type", &NObject::GetDataType, nullptr),
        InstanceAccessor("length", &NObject::GetObjectLength, nullptr),
        InstanceAccessor("owner", &NObject::GetOwner, &NObject::SetOwner),
        InstanceAccessor("reference", &NObject::Reference, nullptr),
        InstanceMethod("hasStream", &NObject::HasStream),
        InstanceMethod("getOffset", &NObject::GetByteOffset),
        InstanceMethod("write", &NObject::WriteObject),
        InstanceMethod("flateCompressStream", &NObject::FlateCompressStream),
        InstanceMethod("delayedStreamLoad", &NObject::DelayedStreamLoad) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("PDObject", constructor);
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
