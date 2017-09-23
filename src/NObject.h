//
// Created by red on 9/23/17
//

#include "ErrorHandler.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class NObject : public ObjectWrap<NObject>
{
public:
  explicit NObject(const CallbackInfo&);
  ~NObject() { delete _obj; }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "Object",
                  { InstanceAccessor("stream", &NObject::GetStream, nullptr) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Object", constructor);
  }
  Napi::Value GetStream(const CallbackInfo&);

private:
  PdfObject* _obj;
};
