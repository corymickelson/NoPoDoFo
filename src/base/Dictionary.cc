//
// Created by red on 9/23/17.
//

#include "Dictionary.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Obj.h"
#include <algorithm>

using namespace std;
using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
FunctionReference Dictionary::constructor;

Dictionary::Dictionary(const CallbackInfo& info)
  : ObjectWrap<Dictionary>(info)
{
  auto objWrap = info[0].As<Object>();
  if (!objWrap.InstanceOf(Obj::constructor.Value())) {
    throw Error::New(
      info.Env(), "Dictionary constructor requires an instance of PdfObject");
  }
  auto obj = Obj::Unwrap(objWrap)->GetObject();
  if (!obj.IsDictionary()) {
    throw Error::New(info.Env(), "PdfObject must be of type 'Dictionary'");
  }
  dict = new PdfDictionary(obj.GetDictionary());
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
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Dictionary", ctor);
}
void
Dictionary::AddKey(const CallbackInfo& info)
{
  if (info.Length() < 2) {
    throw Napi::Error::New(
      info.Env(),
      "add key requires a key and value. Key and value args not found");
  }
  auto v = info[1];
  PdfObject o;

  if (v.IsBoolean()) {
    o = PdfObject(v.As<Boolean>());
  } else if (v.IsNumber()) {
    o = PdfObject(v.As<Number>().DoubleValue());
  } else if (v.IsString()) {
    o = PdfObject(PdfString(v.As<String>().Utf8Value()));
  } else if (v.IsObject()) {
    auto objWrap = info[1].As<Object>();
    Obj* obj = Obj::Unwrap(objWrap);
    o = obj->GetObject();
  }
  PdfName key(info[0].As<String>().Utf8Value());
  dict->AddKey(key, o);
}

Napi::Value
Dictionary::GetKey(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string k = info[0].As<String>().Utf8Value();
  if (!dict->HasKey(PdfName(k))) {
    throw Napi::Error::New(info.Env(),
                           "Key could not be found, please use "
                           "Dictionary.HasKey before accessing key value");
  }
  try {
    PdfObject* v = dict->GetKey(PdfName(k));
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
  TKeyMap keys = dict->GetKeys();
  auto js = Napi::Array::New(info.Env());
  auto it = keys.begin();
  size_t n = 0;
  while (it != keys.end()) {
    string name = (*it).first.GetName();
    js.Set(Napi::Number::New(info.Env(), n),
           Napi::String::New(info.Env(), name));
    n++;
    ++it;
  }
  return js;
}

Napi::Value
Dictionary::RemoveKey(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_string });
  try {
    dict->RemoveKey(PdfName(info[0].As<String>().Utf8Value()));
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Dictionary::HasKey(const CallbackInfo& info)
{
  string key = info[0].As<String>().Utf8Value();
  return Napi::Boolean::New(info.Env(), dict->HasKey(key));
}

Napi::Value
Dictionary::Clear(const CallbackInfo& info)
{
  throw Error::New(info.Env(), "unimplemented");
}

void
Dictionary::SetImmutable(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsBoolean()) {
    dict->SetImmutable(value.As<Boolean>());
  } else {
    throw Napi::Error::New(info.Env(), "Value must be boolean type");
  }
}

Napi::Value
Dictionary::GetImmutable(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), dict->GetImmutable());
}

void
Dictionary::SetDirty(const CallbackInfo& info, const Napi::Value& value)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  dict->SetDirty(value.As<Boolean>());
}

Napi::Value
Dictionary::GetDirty(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), dict->IsDirty());
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
      return Boolean::New(info.Env(), dict->GetKeyAsBool(key));
    } else if (type == "long") {
      return Number::New(info.Env(), dict->GetKeyAsLong(key));
    } else if (type == "name") {
      return String::New(info.Env(), dict->GetKeyAsName(key).GetName());
    } else if (type == "real") {
      return Number::New(info.Env(), dict->GetKeyAsReal(key));
    } else {
      throw Napi::Error::New(info.Env(),
                             "Type must be one of: boolean, long, real, name.");
    }
  }
}

void
Dictionary::WriteSync(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  try {
    string output = info[0].As<String>().Utf8Value();
    PdfOutputDevice device(output.c_str());
    dict->Write(&device, ePdfWriteMode_Default);
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
    const TKeyMap& keys = dict->GetKeys();
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

class DictWriteAsync : public Napi::AsyncWorker
{
public:
  DictWriteAsync(Napi::Function& cb, Dictionary* dict, string dest)
    : Napi::AsyncWorker(cb)
    , dict(dict)
    , arg(std::move(dest))
  {
  }

protected:
  void Execute() override
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
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), Napi::String::New(Env(), arg) });
  }

private:
  Dictionary* dict;
  string arg;
};

Napi::Value
Dictionary::Write(const CallbackInfo& info)
{
  try {
    AssertFunctionArgs(
      info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_function });
    string output = info[0].As<String>().Utf8Value();
    auto cb = info[1].As<Function>();
    DictWriteAsync* worker = new DictWriteAsync(cb, this, output);
    worker->Queue();
    return info.Env().Undefined();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}
Dictionary::~Dictionary()
{
  if (dict != nullptr) {
    HandleScope scope(Env());
    delete dict;
  }
}
}
