//
// Created by red on 9/23/17
//

#include "Dictionary.h"
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
      "Object",
      { InstanceAccessor("stream", &NObject::GetStream, nullptr),
        InstanceMethod("getDictionary", &NObject::GetDictionary),
        InstanceMethod("setDictionary", &NObject::SetDictionary),
        InstanceAccessor("type", &NObject::GetDataType, nullptr),
        InstanceAccessor("length", &NObject::GetObjectLength, nullptr) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Object", constructor);
  }
  Napi::Value GetStream(const CallbackInfo&);
  Napi::Value GetDictionary(const CallbackInfo&);
  void SetDictionary(const CallbackInfo&);
  Napi::Value GetObjectLength(const CallbackInfo&);
  Napi::Value GetDataType(const CallbackInfo&);
  const PdfObject GetObject() { return obj; }

private:
  PdfObject obj = *new PdfObject();
};
