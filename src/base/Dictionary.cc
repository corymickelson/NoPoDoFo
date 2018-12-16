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
#include "Array.h"
#include "Obj.h"
#include "Ref.h"
#include <algorithm>
#include <sstream>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::stringstream;
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
  , self(info.Length() == 1 && info[0].IsExternal()
           ? *info[0].As<External<PdfDictionary>>().Data()
           : *(init = new PdfDictionary()))
{}

void
Dictionary::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Dictionary",
    { InstanceMethod("getKey", &Dictionary::GetKey),
      InstanceMethod("getKeyType", &Dictionary::GetKeyType),
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
      InstanceMethod("eq", &Dictionary::Eq) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Dictionary", ctor);
}
Dictionary::~Dictionary()
{
  HandleScope scope(Env());
  for (auto i : children) {
    delete i;
  }
  delete init;
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
               v.As<Object>().InstanceOf(Dictionary::constructor.Value())) {
      cout << "Adding inline dictionary" << endl;
      auto dict = Dictionary::Unwrap(v.As<Object>());
      GetDictionary().AddKey(key, dict->GetDictionary());
    } else if (v.IsObject() &&
               v.As<Object>().InstanceOf(Ref::constructor.Value())) {
      auto refValue = Ref::Unwrap(v.As<Object>());
      GetDictionary().AddKey(key, *refValue->self);
    } else if (v.IsObject() &&
               v.As<Object>().InstanceOf(Obj::constructor.Value())) {
      auto obj = Obj::Unwrap(v.As<Object>())->GetObject();
      if (obj.IsDictionary()) {
        GetDictionary().AddKey(key, obj.Reference());
        cout << "Adding key: " << key.GetName() << " as reference#"
             << obj.Reference().ObjectNumber() << endl;
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
Dictionary::GetKeyType(const Napi::CallbackInfo& info)
{
  string k = info[0].As<String>().Utf8Value();
  PdfObject* v = GetDictionary().GetKey(k);
  if (!v) {
    Error::New(info.Env(), "Could not resolve key")
      .ThrowAsJavaScriptException();
    return {};
  }
  string t;
  switch (v->GetDataType()) {
    case ePdfDataType_Bool:
      t = "Boolean";
      break;
    case ePdfDataType_Number:
      t = "Number";
      break;
    case ePdfDataType_Real:
      t = "Real";
      break;
    case ePdfDataType_String:
    case ePdfDataType_HexString:
      t = "String";
      break;
    case ePdfDataType_Name:
      t = "Name";
      break;
    case ePdfDataType_Array:
      t = "Array";
      break;
    case ePdfDataType_Dictionary:
      t = "Dictionary";
      break;
    case ePdfDataType_Null:
      t = "Null";
      break;
    case ePdfDataType_Reference:
      t = "Reference";
      break;
    case ePdfDataType_RawData:
      t = "RawData";
      break;
    case ePdfDataType_Unknown:
      t = "Undefined";
      break;
  }
  return String::New(info.Env(), t);
}
Napi::Value
Dictionary::GetKey(const CallbackInfo& info)
{
  string k = info[0].As<String>();
  bool resolveType = true;
  if (info.Length() > 1 && info[1].IsBoolean()) {
    resolveType = info[1].As<Boolean>();
  }
  PdfObject* v = GetDictionary().GetKey(k);
  if (!v) {
    Error::New(info.Env(), "Could not resolve key")
      .ThrowAsJavaScriptException();
    return {};
  }
  if (!resolveType) {
    return Obj::constructor.New(
      { Napi::External<PdfObject>::New(info.Env(), v) });
  }
  switch (v->GetDataType()) {
    case ePdfDataType_Bool:
      return Boolean::New(info.Env(), v->GetBool());
    case ePdfDataType_Number:
      return Number::New(info.Env(), v->GetNumber());
    case ePdfDataType_Real:
      return Number::New(info.Env(), v->GetReal());
    case ePdfDataType_String:
    case ePdfDataType_HexString:
      return String::New(info.Env(), v->GetString().GetString());
    case ePdfDataType_Name:
      return String::New(info.Env(), v->GetName().GetName());
    case ePdfDataType_Array:
      return NoPoDoFo::Array::constructor.New(
        { Napi::External<PdfObject>::New(info.Env(), v) });
    case ePdfDataType_Dictionary:
      return Dictionary::constructor.New({ Napi::External<PdfDictionary>::New(
        info.Env(), &v->GetDictionary()) });
    case ePdfDataType_Null:
      return info.Env().Null();
    case ePdfDataType_RawData: {
      if (!v->HasStream()) {
        Error::New(info.Env(), "RawData missing Stream property")
          .ThrowAsJavaScriptException();
        return {};
      }
      auto pStream = dynamic_cast<PdfMemStream*>(v->GetStream());
      auto stream = pStream->Get();
      auto length = pStream->GetLength();
      return Buffer<char>::Copy(
        info.Env(), stream, static_cast<size_t>(length));
    }
    case ePdfDataType_Reference: {
      return Ref::constructor.New(
        { Number::New(info.Env(), v->GetReference().ObjectNumber()),
          Number::New(info.Env(), v->GetReference().GenerationNumber()) });
    }
    case ePdfDataType_Unknown:
      return info.Env().Undefined();
  }
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
