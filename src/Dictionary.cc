//
// Created by red on 9/23/17.
//

#include "Dictionary.h"
#include "Obj.h"
#include "ValidateArguments.h"

Dictionary::Dictionary(const CallbackInfo &info)
    : ObjectWrap<Dictionary>(info) {
  auto objWrap = info[0].As<Object>();
  auto objPtr = Obj::Unwrap(objWrap);
  dict = objPtr->GetObject().GetDictionary();
}

void
Dictionary::AddKey(const CallbackInfo &info) {
  AssertFunctionArgs(
      info, 2, {napi_valuetype::napi_string, napi_valuetype::napi_object});
  PdfName key(info[0].As<String>().Utf8Value());
  auto objWrap = info[1].As<Object>();
  Obj *o=  Obj::Unwrap(objWrap);
  auto obj = o->GetObject();
  dict.AddKey(key, obj);
}

Napi::Value
Dictionary::GetKey(const CallbackInfo &info) {
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_string});
  string k = info[0].As<String>().Utf8Value();
  PdfObject *v = dict.GetKey(PdfName(k));
  auto objPtr = Napi::External<PdfObject>::New(info.Env(), v);
  auto instance = Obj::constructor.New({objPtr});
  return instance;
}

Napi::Value
Dictionary::GetKeys(const CallbackInfo &info) {
  auto keys = static_cast<map<PdfName, PdfObject *> &&>(dict.GetKeys());
  auto js = Napi::Array::New(info.Env());
  uint32_t count = 0;
  for (auto &key : keys) {
    js.Set(count,
           Napi::String::New(info.Env(), key.second->GetName().GetName()));
    count++;
  }
  return js;
}

Napi::Value
Dictionary::RemoveKey(const CallbackInfo &info) {
  return Value();
}

Napi::Value
Dictionary::HasKey(const CallbackInfo &info) {
  string key = info[0].As<String>().Utf8Value();
  return Napi::Boolean::New(info.Env(), dict.HasKey(key));
}

Napi::Value
Dictionary::Clear(const CallbackInfo &info) {
  return Value();
}

void
Dictionary::SetImmutable(const CallbackInfo &info, const Napi::Value &value) {
  if (value.IsBoolean()) {
    dict.SetImmutable(value.As<Boolean>());
  } else {
    throw Napi::Error::New(info.Env(), "Value must be boolean type");
  }
}

Napi::Value
Dictionary::GetImmutable(const CallbackInfo &info) {
  return Napi::Boolean::New(info.Env(), dict.GetImmutable());
}

void
Dictionary::SetDirty(const CallbackInfo &info, const Napi::Value &value) {
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_boolean});
  dict.SetDirty(value.As<Boolean>());
}

Napi::Value
Dictionary::GetDirty(const CallbackInfo &info) {
  return Napi::Boolean::New(info.Env(), dict.IsDirty());
}

Napi::Value
Dictionary::GetKeyAs(const CallbackInfo &info) {
  return Value();
}

void
Dictionary::Write(const CallbackInfo &info) {}
