//
// Created by red on 9/23/17.
//

#include "Dictionary.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Obj.h"

using namespace Napi;
using namespace PoDoFo;

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
      InstanceMethod("write", &Dictionary::Write) });
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
  PdfObject* v = dict.GetKey(PdfName(k));
  auto objPtr = Napi::External<PdfObject>::New(info.Env(), v);
  auto instance = Obj::constructor.New({ objPtr });
  return instance;
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
  return Value();
}

void
Dictionary::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_function });
  string output = info[0].As<String>().Utf8Value();
  auto cb = info[1].As<Function>();
  DictWriteAsync* worker = new DictWriteAsync(cb, this, output);
  worker->Queue();
}

void
DictWriteAsync::Execute()
{
  try {
    PdfOutputDevice device(arg.c_str());
    dict->GetDictionary()->Write(&device, ePdfWriteMode_Default);
  } catch (PdfError& err) {
    SetError(ErrorHandler::WriteMsg(err));
  } catch (Napi::Error& err) {
    SetError(err.Message());
  }
}

void
DictWriteAsync::OnOK()
{
  HandleScope scope(Env());
  Callback().Call({ Env().Null(), Napi::String::New(Env(), arg) });
}
