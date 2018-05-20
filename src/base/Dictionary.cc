/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Dictionary.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Obj.h"
#include <algorithm>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

namespace NoPoDoFo {

FunctionReference Dictionary::constructor; // NOLINT

/**
 * @note
 * @brief Dictionary::Dictionary
 * @param info
 */
Dictionary::Dictionary(const CallbackInfo& info)
  : ObjectWrap(info)
  , obj(info[0].As<External<PdfObject>>().Data())
{}

Dictionary::~Dictionary()
{
  HandleScope scope(Env());
  cout << "Destructing Dictionary" << endl;
  delete obj;
  obj = nullptr;
}

void
Dictionary::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Dictionary",
    { StaticMethod("tryGetDictionary", &Dictionary::ResolveDictionary),
      InstanceMethod("getKey", &Dictionary::GetKey),
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
      //      InstanceMethod("toObject", &Dictionary::ToObject),
      InstanceMethod("eq", &Dictionary::Eq) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Dictionary", ctor);
}

Value
Dictionary::ResolveDictionary(const CallbackInfo& info)
{
  auto doc = Document::Unwrap(info[0].As<Object>());
  auto value = Obj::Unwrap(info[1].As<Object>())->GetObject();
  if (value->IsReference()) {
    value = doc->GetDocument()->GetObjects().GetObject(value->Reference());
  }
  return Dictionary::constructor.New(
    { External<PdfObject>::New(info.Env(), new PdfObject(*value)) });
}

PdfDictionary&
Dictionary::Resolve(PdfDocument* doc, PdfObject* candidate)
{
  if (candidate->IsDictionary()) {
    return candidate->GetDictionary();
  } else if (candidate->IsReference()) {
    return Resolve(doc,
                   doc->GetObjects()->GetObject(candidate->GetReference()));
  }
}
Napi::Value
Dictionary::Eq(const CallbackInfo& info)
{
  auto wrap = info[0].As<Object>();
  if (!wrap.InstanceOf(Dictionary::constructor.Value())) {
    throw Error::New(info.Env(), "Must be an instance of NoPoDoFo Obj");
  }
  auto value = Dictionary::Unwrap(wrap);
  return Boolean::New(info.Env(), value->GetDictionary() == GetDictionary());
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
  PdfName key(info[0].As<String>().Utf8Value());
  try {

    if (v.IsBoolean()) {
      GetDictionary().AddKey(key, PdfVariant(v.As<Boolean>()));
    } else if (v.IsNumber()) {
      GetDictionary().AddKey(key, PdfVariant(v.As<Number>().Int64Value()));
    } else if (v.IsString()) {
      GetDictionary().AddKey(key, PdfString(v.As<String>().Utf8Value()));
    } else if (v.IsObject() &&
               v.As<Object>().InstanceOf(Obj::constructor.Value())) {
      auto objWrap = info[1].As<Object>();
      PdfObject* obj = Obj::Unwrap(objWrap)->GetObject();
      if (obj->IsDictionary()) {
        GetDictionary().AddKey(key, obj->Reference());
        cout << "Adding key: " << key.GetName() << " as reference#"
             << obj->Reference().ObjectNumber() << endl;
      } else
        GetDictionary().AddKey(key, obj);
    } else {
      TypeError::New(
        info.Env(),
        "Invalid dictionary value type. See NoPoDoFo documentation "
        "for more information and examples.")
        .ThrowAsJavaScriptException();
    }

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Dictionary::GetKey(const CallbackInfo& info)
{
  string k = info[0].As<String>().Utf8Value();
  bool resolveValue = true;
  if(info.Length() == 2 && info[1].IsBoolean()) {
    resolveValue = info[1].As<Boolean>();
  }
  if (!GetDictionary().HasKey(PdfName(k))) {
    throw Napi::Error::New(info.Env(),
                           "Key could not be found, please use "
                           "Dictionary.HasKey before accessing key value");
  }
  try {
    PdfObject* o = GetDictionary().GetKey(PdfName(k));
    // try and resolve the reference
    if (o->IsReference() && resolveValue) {
      if (!o->GetOwner()->GetObject(o->GetReference())) {
        if (!o->GetOwner()->GetParentDocument()->GetObjects()->GetObject(
              o->GetReference())) {
          stringstream msg;
          msg << "Could not resolve object reference: "
              << o->GetReference().ObjectNumber() << ", "
              << o->GetReference().GenerationNumber() << endl;
          Error::New(info.Env(), msg.str());
          return info.Env().Undefined();
        } else {
          o = o->GetOwner()->GetParentDocument()->GetObjects()->GetObject(
            o->GetReference());
        }
      } else {
        o = o->GetOwner()->GetObject(o->GetReference());
      }
    }
    auto objPtr = Napi::External<PdfObject>::New(
      info.Env(), new PdfObject(*o), [](Napi::Env env, PdfObject* data) {
        HandleScope scope(env);
        delete data;
      });
    auto instance = Obj::constructor.New({ objPtr });
    return instance;
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

Napi::Value
Dictionary::GetKeys(const CallbackInfo& info)
{
  TKeyMap keys = GetDictionary().GetKeys();
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
  try {
    GetDictionary().RemoveKey(PdfName(info[0].As<String>().Utf8Value()));
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

Napi::Value
Dictionary::HasKey(const CallbackInfo& info)
{
  string key = info[0].As<String>().Utf8Value();
  return Napi::Boolean::New(info.Env(), GetDictionary().HasKey(key));
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
    GetDictionary().SetImmutable(value.As<Boolean>());
  } else {
    throw Napi::Error::New(info.Env(), "Value must be boolean type");
  }
}

Napi::Value
Dictionary::GetImmutable(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetDictionary().GetImmutable());
}

void
Dictionary::SetDirty(const CallbackInfo& info, const Napi::Value& value)
{
  GetDictionary().SetDirty(value.As<Boolean>());
}

Napi::Value
Dictionary::GetDirty(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetDictionary().IsDirty());
}

Napi::Value
Dictionary::GetKeyAs(const CallbackInfo& info)
{
  string type = info[1].As<String>().Utf8Value();
  string key = info[0].As<String>().Utf8Value();
  vector<string> valid = { "boolean", "long", "name", "real" };
  if (find(valid.begin(), valid.end(), type) != valid.end()) {
    if (type == "boolean") {
      return Boolean::New(info.Env(), GetDictionary().GetKeyAsBool(key));
    } else if (type == "long") {
      return Number::New(info.Env(), GetDictionary().GetKeyAsLong(key));
    } else if (type == "name") {
      return String::New(info.Env(),
                         GetDictionary().GetKeyAsName(key).GetName());
    } else if (type == "real") {
      return Number::New(info.Env(), GetDictionary().GetKeyAsReal(key));
    } else {
      throw Napi::Error::New(info.Env(),
                             "Type must be one of: boolean, long, real, name.");
    }
  }
  return info.Env().Undefined();
}

void
Dictionary::WriteSync(const CallbackInfo& info)
{
  try {
    string output = info[0].As<String>().Utf8Value();
    PdfOutputDevice device(output.c_str());
    GetDictionary().Write(&device, ePdfWriteMode_Default);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

// Napi::Value
// Dictionary::ToObject(const CallbackInfo& info)
//{
//  try {
//    auto js = Object::New(info.Env());
//    auto keys = GetDictionary().GetKeys();
//    map<PoDoFo::PdfName, PoDoFo::PdfObject*>::const_iterator it;
//    for (it = keys.begin(); it != keys.end(); it++) {
//      auto key = String::New(info.Env(), (*it).first.GetName());
//      auto ptr = External<PdfObject>::New(
//        info.Env(), new PdfObject((*it).second), [](Napi::Env env, PdfObject*
//        data) {
//          HandleScope scope(env);
//          delete data;
//          data = nullptr;
//        });
//      auto value = Obj::constructor.New({ ptr });
//      js.Set(key, value);
//    }
//    return js;
//  } catch (PdfError& err) {
//    ErrorHandler(err, info);
//  } catch (Napi::Error& err) {
//    ErrorHandler(err, info);
//  }
//  return info.Env().Undefined();
//}

class DictWriteAsync : public Napi::AsyncWorker
{
public:
  DictWriteAsync(Napi::Function& cb, Dictionary* dict, string dest)
    : Napi::AsyncWorker(cb)
    , dict(dict)
    , arg(std::move(dest))
  {}
  ~DictWriteAsync()
  {
    HandleScope scope(Env());
    dict = nullptr;
  }

protected:
  void Execute() override
  {
    try {
      PdfOutputDevice device(arg.c_str());
      dict->GetDictionary().Write(&device, ePdfWriteMode_Default);
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
  return info.Env().Undefined();
}
}
