//
// Created by red on 9/23/17.
//

#include "Dictionary.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Obj.h"
#include <boost/filesystem.hpp>

using namespace Napi;
using namespace PoDoFo;
using namespace boost;

Dictionary::Dictionary(const CallbackInfo& info)
  : ObjectWrap<Dictionary>(info)
{
  auto objWrap = info[0].As<Object>();
  auto objPtr = Obj::Unwrap(objWrap);
  dict = objPtr->GetObject().GetDictionary();
}
void
Dictionary::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Dictionary",
    { InstanceMethod("getKey", &Dictionary::GetKey),
      InstanceMethod("getKeys", &Dictionary::GetKeys),
      InstanceMethod("hasKey", &Dictionary::HasKey),
      InstanceMethod("addKey", &Dictionary::AddKey),
      InstanceMethod("removeKey", &Dictionary::RemoveKey),
      InstanceMethod("getKeyAs", &Dictionary::GetKeyAs),
      InstanceAccessor("dirty", &Dictionary::GetDirty, &Dictionary::SetDirty),
      InstanceAccessor(
        "immutable", &Dictionary::GetImmutable, &Dictionary::SetImmutable),
      InstanceMethod("clear", &Dictionary::Clear),
      InstanceMethod("write", &Dictionary::Write),
      InstanceMethod("writeSync", &Dictionary::WriteSync),
      InstanceMethod("toObject", &Dictionary::ToObject) });
  target.Set("Dictionary", ctor);
}
void
Dictionary::AddKey(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_object });
  PdfName key(info[0].As<String>().Utf8Value());
  auto objWrap = info[1].As<Object>();
  Obj* o = Obj::Unwrap(objWrap);
  auto obj = o->GetObject();
  dict.AddKey(key, obj);
}

Napi::Value
Dictionary::GetKey(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string k = info[0].As<String>().Utf8Value();
  if (!dict.HasKey(PdfName(k))) {
    throw Napi::Error::New(info.Env(),
                           "Key could not be found, please use "
                           "Dictionary.HasKey before accessing key value");
  }
  try {
    PdfObject* v = dict.GetKey(PdfName(k));
    auto objPtr = Napi::External<PdfObject>::New(info.Env(), v);
    auto instance = Obj::constructor.New({ objPtr });
    return instance;
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Dictionary::GetKeys(const CallbackInfo& info)
{
  TKeyMap keys = dict.GetKeys();
  auto js = Napi::Array::New(info.Env());
  auto it = keys.begin();
  size_t n = 0;
  while (it != keys.end()) {
    string name = (*it).first.GetName();
    js.Set(Napi::Number::New(info.Env(), n),
           Napi::String::New(info.Env(), name));
    n++;
    it++;
  }
  return js;
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
  AssertFunctionArgs(info, 2, { napi_valuetype::napi_string });
  string type = info[0].As<String>().Utf8Value();
  string key = info[1].As<String>().Utf8Value();
  vector<string> valid = { "boolean", "long", "name", "real" };
  if (find(valid.begin(), valid.end(), type) != valid.end()) {
    if (type == "boolean") {
      return Boolean::New(info.Env(), dict.GetKeyAsBool(key));
    } else if (type == "long") {
      return Number::New(info.Env(), dict.GetKeyAsLong(key));
    } else if (type == "name") {
      return String::New(info.Env(), dict.GetKeyAsName(key).GetName());
    } else if (type == "real") {
      return Number::New(info.Env(), dict.GetKeyAsReal(key));
    } else {
      throw Napi::Error::New(info.Env(),
                             "Type must be one of: boolean, long, real, name.");
    }
  }
}

Napi::Value
Dictionary::Write(const CallbackInfo& info)
{
  try {
    auto resolver = Promise::Resolver::New(info.Env());
    AssertFunctionArgs(
      info, 1, { napi_valuetype::napi_string });
    string output = info[0].As<String>().Utf8Value();
    if (filesystem::exists(output)) {
      filesystem::remove(output);
    }
    PdfOutputDevice device(output.c_str());
    dict.Write(&device, ePdfWriteMode_Default);
    resolver.Resolve(String::New(info.Env(), output));
    return resolver.Promise();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

void
Dictionary::WriteSync(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  try {
    string output = info[0].As<String>().Utf8Value();
    if (filesystem::exists(output)) {
      filesystem::remove(output);
    }
    PdfOutputDevice device(output.c_str());
    dict.Write(&device, ePdfWriteMode_Default);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Dictionary::ToObject(const CallbackInfo& info)
{
  try {
    Object js = Object::New(info.Env());
    const TKeyMap& keys = dict.GetKeys();
    map<PoDoFo::PdfName, PoDoFo::PdfObject*>::const_iterator it;
    for (it = keys.begin(); it != keys.end(); it++) {
      auto key = String::New(info.Env(), (*it).first.GetName());
      auto ptr = External<PdfObject>::New(info.Env(), (*it).second);
      auto value = Obj::constructor.New({ ptr });
      js.Set(key, value);
    }
    return js;

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

