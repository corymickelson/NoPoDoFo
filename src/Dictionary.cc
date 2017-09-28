//
// Created by red on 9/23/17.
//

#include "Dictionary.h"

FunctionReference Dictionary::constructor;

Dictionary::Dictionary(const CallbackInfo& info)
  : ObjectWrap<Dictionary>(info)
{
  dict = *info[0].As<Napi::External<PoDoFo::PdfDictionary>>().Data();
}

void
Dictionary::AddKey(const CallbackInfo& info)
{}

Napi::Value
Dictionary::GetKey(const CallbackInfo& info)
{
  return Value();
}

Napi::Value
Dictionary::GetKeys(const CallbackInfo& info)
{
  return Value();
}

Napi::Value
Dictionary::RemoveKey(const CallbackInfo& info)
{
  return Value();
}

Napi::Value
Dictionary::HasKey(const CallbackInfo& info)
{
  string key = info[0].As<String>().Utf8Value();
  return Napi::Boolean::New(info.Env(), dict.HasKey(key));
}

Napi::Value
Dictionary::Clear(const CallbackInfo& info)
{
  return Value();
}

void
Dictionary::SetImmutable(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsBoolean()) {
    dict.SetImmutable(value.As<Boolean>());
  } else {
    throw Napi::Error::New(info.Env(), "Value must be boolean type");
  }
}

Napi::Value
Dictionary::GetImmutable(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), dict.GetImmutable());
}
