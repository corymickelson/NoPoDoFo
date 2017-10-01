#include "Obj.h"
#include "ErrorHandler.h"
#include "Ref.h"
#include "ValidateArguments.h"

FunctionReference Obj::constructor;

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
