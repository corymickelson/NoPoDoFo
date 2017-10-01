//
// Created by red on 9/23/17.
//

#include <napi.h>
#include "Dictionary.h"
#include "ValidateArguments.h"
#include "Obj.h"

// FunctionReference Dictionary::constructor;

Dictionary::Dictionary(const CallbackInfo& info)
  : ObjectWrap<Dictionary>(info)
{
  auto objWrap = info[0].As<Object>();
  auto objPtr = Obj::Unwrap(objWrap);
  dict = objPtr->GetObject().GetDictionary();
  //  dict = *info[0].As<Napi::External<PoDoFo::PdfDictionary>>().Data();
}

void
Dictionary::AddKey(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_object });
  PdfName key(info[0].As<String>().Utf8Value());
  auto objWrap = info[0].As<Object>();
  //  Obj obj = Obj::
}

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

void
Dictionary::SetDirty(const CallbackInfo& info, const Napi::Value& value)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  dict.SetDirty(value.As<Boolean>());
}

Napi::Value
Dictionary::GetDirty(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), dict.IsDirty());
}

Napi::Value
Dictionary::GetKeyAs(const CallbackInfo& info)
{
  return Value();
}

void
Dictionary::Write(const CallbackInfo& info)
{}
