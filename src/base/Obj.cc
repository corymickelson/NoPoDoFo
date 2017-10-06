#include "Obj.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Arr.h"
#include "Dictionary.h"
#include "Ref.h"

using namespace Napi;
using namespace PoDoFo;

FunctionReference Obj::constructor;

void
Obj::Initialize(Napi::Env& env, Napi::Object& target)
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
      InstanceMethod("delayedStreamLoad", &Obj::DelayedStreamLoad),
      InstanceMethod("asType", &Obj::AsType) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Obj", constructor);
}

Obj::Obj(const Napi::CallbackInfo& info)
  : ObjectWrap<Obj>(info)
{
  obj = *info[0].As<Napi::External<PdfObject>>().Data();
}

Napi::Value
Obj::GetStream(const CallbackInfo& info)
{
  auto stream = obj.GetStream();
  return Value();
}

Value
Obj::HasStream(const CallbackInfo&)
{}

Napi::Value
Obj::GetObjectLength(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           obj.GetObjectLength(ePdfWriteMode_Default));
}

Napi::Value
Obj::GetDataType(const CallbackInfo& info)
{
  string js;
  if (obj.IsArray()) {
    js = "Array";
  } else if (obj.IsBool()) {
    js = "Boolean";
  } else if (obj.IsDictionary()) {
    js = "Dictionary";
  } else if (obj.IsEmpty()) {
    js = "Empty";
  } else if (obj.IsHexString()) {
    js = "HexString";
  } else if (obj.IsNull()) {
    js = "Null";
  } else if (obj.IsNumber()) {
    js = "Number";
  } else if (obj.IsRawData()) {
    js = "RawData";
  } else if (obj.IsReal()) {
    js = "Real";
  } else if (obj.IsReference()) {
    js = "Reference";
  } else if (obj.IsString()) {
    js = "String";
  } else {
    js = "Unknown";
  }
  return Napi::String::New(info.Env(), js);
}

Napi::Value
Obj::GetByteOffset(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string key = info[0].As<String>().Utf8Value();
  return Napi::Number::New(
    info.Env(), obj.GetByteOffset(key.c_str(), ePdfWriteMode_Default));
}

Napi::Value
Obj::GetOwner(const CallbackInfo& info)
{
  return Value();
}

void
Obj::SetOwner(const CallbackInfo& info, const Napi::Value& value)
{}

void
Obj::WriteObject(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string output = info[0].As<String>().Utf8Value();
  try {
    PdfOutputDevice device(output.c_str());
    obj.WriteObject(&device, ePdfWriteMode_Default, nullptr);

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Obj::Reference(const CallbackInfo& info)
{
  return Value();
}

void
Obj::FlateCompressStream(const CallbackInfo& info)
{
  try {
    obj.FlateCompressStream();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Obj::DelayedStreamLoad(const CallbackInfo& info)
{
  try {
    obj.DelayedStreamLoad();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Obj::AsType(const CallbackInfo& info)
{
  return ParseToType(info, obj);
  //  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  //  string type = info[0].As<String>().Utf8Value();
  //  Napi::Value value;
  //  if (type == "Boolean")
  //    value = Napi::Boolean::New(info.Env(), obj.GetBool());
  //  else if (type == "Number")
  //    value = Napi::Number::New(info.Env(), obj.GetNumber());
  //  else if (type == "Name")
  //    value = Napi::String::New(info.Env(), obj.GetName().GetName());
  //  else if (type == "Real")
  //    value = Napi::Number::New(info.Env(), obj.GetReal());
  //  else if (type == "String")
  //    value = Napi::String::New(info.Env(), obj.GetString().GetStringUtf8());
  //  else if (type == "Array") {
  //    auto init = obj.GetArray();
  //    auto initPtr = Napi::External<PdfArray>::New(info.Env(), &init);
  //    value = Arr::constructor.New({ initPtr });
  //  } else if (type == "Dictionary") {
  //    throw Napi::Error::New(info.Env(), "Init from Dictionary constructor");
  //  } else if (type == "Reference") {
  //    auto init = obj.GetReference();
  //    auto initPtr = Napi::External<PdfReference>::New(info.Env(), &init);
  //    value = Ref::constructor.New({ initPtr });
  //  } else if (type == "RawData") {
  //    throw Napi::Error::New(info.Env(), "unimplemented");
  //  }
  //  return value;
}

static Napi::Value
ParseToType(const Napi::CallbackInfo& info, PdfObject& obj)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string type = info[0].As<String>().Utf8Value();
  Napi::Value value;
  if (type == "Boolean")
    value = Napi::Boolean::New(info.Env(), obj.GetBool());
  else if (type == "Number")
    value = Napi::Number::New(info.Env(), obj.GetNumber());
  else if (type == "Name")
    value = Napi::String::New(info.Env(), obj.GetName().GetName());
  else if (type == "Real")
    value = Napi::Number::New(info.Env(), obj.GetReal());
  else if (type == "String")
    value = Napi::String::New(info.Env(), obj.GetString().GetStringUtf8());
  else if (type == "Array") {
    auto init = obj.GetArray();
    auto initPtr = Napi::External<PdfArray>::New(info.Env(), &init);
    value = Arr::constructor.New({ initPtr });
  } else if (type == "Dictionary") {
    value = Napi::String::New(info.Env(), "Dictionary");
  } else if (type == "Reference") {
    auto init = obj.GetReference();
    auto initPtr = Napi::External<PdfReference>::New(info.Env(), &init);
    value = Ref::constructor.New({ initPtr });
  } else if (type == "RawData") {
    throw Napi::Error::New(info.Env(), "unimplemented");
  }
  return value;
}
