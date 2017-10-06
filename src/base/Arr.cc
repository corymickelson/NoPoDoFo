//
// Created by red on 10/2/17
//

#include "Arr.h"
#include "../ValidateArguments.h"
#include "Obj.h"

using namespace Napi;
using namespace PoDoFo;

Napi::FunctionReference Arr::constructor;

Arr::Arr(const CallbackInfo& info)
  : ObjectWrap<Arr>(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_external });
  arr = *info[0].As<External<PdfArray>>().Data();
}

void
Arr::Initialize(Napi::Env& env, Napi::Object& target)
{
  Function ctor =
    DefineClass(env,
                "Arr",
                { InstanceAccessor("dirty", &Arr::IsDirty, &Arr::SetDirty),
                  InstanceMethod("getIndex", &Arr::GetIndex),
                  InstanceMethod("contains", &Arr::ContainsString),
                  InstanceMethod("indexOf", &Arr::GetStringIndex),
                  InstanceMethod("write", &Arr::Write) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Arr", constructor);
}
void
Arr::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string output = info[0].As<String>().Utf8Value();
  PdfOutputDevice device(output.c_str());
  arr.Write(&device, ePdfWriteMode_Default);
}

Napi::Value
Arr::ContainsString(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string searchString = info[0].As<String>().Utf8Value();
  bool match = arr.ContainsString(searchString);
  return Napi::Boolean::New(info.Env(), match);
}

Napi::Value
Arr::GetStringIndex(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string str = info[0].As<String>().Utf8Value();
  return Napi::Number::New(info.Env(), arr.GetStringIndex(str));
}

Napi::Value
Arr::IsDirty(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), arr.IsDirty());
}

Napi::Value
Arr::GetIndex(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  size_t index = info[0].As<Number>().Uint32Value();
  PdfObject item = arr[index];
  return Obj::ParseToType(info, item);
}

void
Arr::SetDirty(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::Error::New(info.Env(), "dirty must be of type boolean");
  }
  arr.SetDirty(value.As<Boolean>());
}
