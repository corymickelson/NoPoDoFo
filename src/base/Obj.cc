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
      InstanceMethod("write", &Obj::Write),
      InstanceMethod("flateCompressStream", &Obj::FlateCompressStream),
      InstanceMethod("delayedStreamLoad", &Obj::DelayedStreamLoad),
      InstanceMethod("getBool", &Obj::GetBool),
      InstanceMethod("getNumber", &Obj::GetNumber),
      InstanceMethod("getReal", &Obj::GetReal),
      InstanceMethod("getString", &Obj::GetString),
      InstanceMethod("getName", &Obj::GetName),
      InstanceMethod("getArray", &Obj::GetArray),
      InstanceMethod("getReference", &Obj::GetReference),
      InstanceMethod("getRawData", &Obj::GetRawData) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Obj", constructor);
}

Obj::Obj(const Napi::CallbackInfo& info)
  : ObjectWrap<Obj>(info)
{
  if (info.Length() == 1) {
    obj = *info[0].As<Napi::External<PdfObject>>().Data();
  }
}

Napi::Value
Obj::GetStream(const CallbackInfo& info)
{
  try {
    PdfMemStream* pStream = dynamic_cast<PdfMemStream*>(obj.GetStream());
    //    auto stream = obj.GetStream();
    auto stream = pStream->Get();
    auto length = pStream->GetLength();
    //    char* alloc =
    //      static_cast<char*>(malloc(sizeof(char) *
    //      static_cast<size_t>(length)));
    //    stream->GetCopy(&alloc, &length);
    auto value = Buffer<char>::Copy(info.Env(), stream, length);
    //    free(alloc);
    return value;
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Obj::HasStream(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), obj.HasStream());
}

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
  } else if (obj.IsName()) {
    js = "Name";
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
  auto resolver = Promise::Resolver::New(info.Env());
  string key = info[0].As<String>().Utf8Value();
  auto value = Napi::Number::New(
    info.Env(), obj.GetByteOffset(key.c_str(), ePdfWriteMode_Default));
  resolver.Resolve(value);
  return resolver.Promise();
}

Napi::Value
Obj::GetOwner(const CallbackInfo& info)
{
  return Value();
}

void
Obj::SetOwner(const CallbackInfo& info, const Napi::Value& value)
{}

Napi::Value
Obj::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string output = info[0].As<String>().Utf8Value();
  auto resolver = Promise::Resolver::New(info.Env());
  PdfOutputDevice device(output.c_str());
  obj.Write(&device, ePdfWriteMode_Default, nullptr);
  resolver.Resolve(String::New(info.Env(), output));
  return resolver.Promise();
}

Napi::Value
Obj::Reference(const CallbackInfo& info)
{
  try {
    PdfReference init = obj.Reference();
    auto initPtr = Napi::External<PdfReference>::New(info.Env(), &init);
    return Ref::constructor.New({ initPtr });
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
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
Obj::GetNumber(const CallbackInfo& info)
{
  if (!obj.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }
  return Number::New(info.Env(), obj.GetNumber());
}

Napi::Value
Obj::GetReal(const CallbackInfo& info)
{
  if (!obj.IsReal()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }

  return Number::New(info.Env(), obj.GetReal());
}

Napi::Value
Obj::GetString(const CallbackInfo& info)
{
  if (!obj.IsString()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a String");
  }

  return String::New(info.Env(), obj.GetString().GetStringUtf8());
}

Napi::Value
Obj::GetName(const CallbackInfo& info)
{
  if (!obj.IsName()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a string");
  }
  try {
    string name = obj.GetName().GetName();
    return String::New(info.Env(), name);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Obj::GetArray(const CallbackInfo& info)
{
  if (!obj.IsArray()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as array");
  }
  auto init = obj.GetArray();
  auto ptr = External<PdfArray>::New(info.Env(), &init);
  auto instance = Arr::constructor.New({ ptr });
  return instance;
}

Napi::Value
Obj::GetBool(const CallbackInfo& info)
{
  if (!obj.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a boolean");
  }
  return Boolean::New(info.Env(), obj.GetBool());
}

Napi::Value
Obj::GetReference(const CallbackInfo& info)
{
  if (!obj.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a Ref");
  }
  auto init = obj.GetReference();
  auto ptr = External<PdfReference>::New(info.Env(), &init);
  auto instance = Ref::constructor.New({ ptr });
  return instance;
}

Napi::Value
Obj::GetRawData(const CallbackInfo& info)
{
  if (!obj.IsRawData()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a buffer");
  }
  string data = obj.GetRawData().data();
  return Buffer<char>::Copy(info.Env(), data.c_str(), data.length());
}
