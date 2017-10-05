#include "Ref.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

using namespace Napi;
using namespace PoDoFo;

Napi::FunctionReference Ref::constructor;

Ref::Ref(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  ref = *info[0].As<Napi::External<PdfReference>>().Data();
}

void
Ref::Initialize(Napi::Env& env, Napi::Object& target)
{
  Napi::Function ctor = DefineClass(
    env,
    "Ref",
    { InstanceAccessor(
        "generation", &Ref::GetGenerationNumber, &Ref::SetGenerationNumber),
      InstanceAccessor("object", &Ref::GetObjectNumber, &Ref::SetObjectNumber),
      InstanceMethod("toString", &Ref::ToString),
      InstanceMethod("write", &Ref::Write),
      InstanceMethod("isIndirect", &Ref::IsIndirect) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Ref", constructor);
}
void
Ref::SetGenerationNumber(const Napi::CallbackInfo& info,
                         const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "Generation number must be of type number");
  }
  int gen = value.As<Napi::Number>();
  ref.SetGenerationNumber(static_cast<const pdf_gennum>(gen));
}
Napi::Value
Ref::GetGenerationNumber(const Napi::CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), ref.GenerationNumber());
}
void
Ref::SetObjectNumber(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Object number must be of type number");
  }
  int objNumber = value.As<Napi::Number>();
  ref.SetObjectNumber(static_cast<const pdf_gennum>(objNumber));
}
Napi::Value
Ref::GetObjectNumber(const Napi::CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), ref.ObjectNumber());
}
Napi::Value
Ref::ToString(const Napi::CallbackInfo& info)
{
  return Napi::String::New(info.Env(), ref.ToString());
}
void
Ref::Write(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string output = info[0].As<Napi::String>().Utf8Value();
  try {
    PdfOutputDevice device(output.c_str());
    ref.Write(&device, ePdfWriteMode_Default);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Ref::IsIndirect(const Napi::CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), ref.IsIndirect());
}
