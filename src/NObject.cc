#include "NObject.h"
#include "ValidateArguments.h"

FunctionReference NObject::constructor;

NObject::NObject(const Napi::CallbackInfo& info)
  : ObjectWrap<NObject>(info)
{
  obj = *info[0].As<Napi::External<PdfObject>>().Data();
}

Napi::Value
NObject::GetStream(const CallbackInfo& info)
{
  auto stream = obj.GetStream();
  return Value();
}

Value
NObject::HasStream(const CallbackInfo&)
{}

Napi::Value
NObject::GetObjectLength(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           obj.GetObjectLength(ePdfWriteMode_Default));
}

Napi::Value
NObject::GetDataType(const CallbackInfo& info)
{
  string js;
  if (obj.IsArray()) {
    js = "Array";
  }
  if (obj.IsBool()) {
    js = "Boolean";
  }
  if (obj.IsDictionary()) {
    js = "Dictionary";
  }
  if (obj.IsEmpty()) {
    js = "Empty";
  }
  if (obj.IsHexString()) {
    js = "HexString";
  }
  if (obj.IsNull()) {
    js = "Null";
  }
  if (obj.IsNumber()) {
    js = "Number";
  }
  if (obj.IsRawData()) {
    js = "RawData";
  }
  if (obj.IsReal()) {
    js = "Real";
  }
  if (obj.IsReference()) {
    js = "Reference";
  }
  if (obj.IsString()) {
    js = "String";
  } else {
    throw Napi::Error::New(info.Env(), "Unknown DataType");
  }
  return Napi::String::New(info.Env(), js);
}

Napi::Value
NObject::GetByteOffset(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string key = info[0].As<String>().Utf8Value();
  return Napi::Number::New(
    info.Env(), obj.GetByteOffset(key.c_str(), ePdfWriteMode_Default));
}

Napi::Value
NObject::GetOwner(const CallbackInfo& info)
{
  return Value();
}

void
NObject::SetOwner(const CallbackInfo& info, const Napi::Value& value)
{}

void
NObject::WriteObject(const CallbackInfo& info)
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
NObject::Reference(const CallbackInfo& info)
{
  return Value();
}

void
NObject::FlateCompressStream(const CallbackInfo& info)
{
  try {
    obj.FlateCompressStream();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
NObject::DelayedStreamLoad(const CallbackInfo& info)
{
  try {
    obj.DelayedStreamLoad();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
