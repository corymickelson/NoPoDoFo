#include "NObject.h"

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

void
NObject::SetDictionary(const CallbackInfo& info)
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
