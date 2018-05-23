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

#include "Obj.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Array.h"
#include "Dictionary.h"
#include "Ref.h"

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::make_shared;
using std::string;

namespace NoPoDoFo {

FunctionReference Obj::constructor; // NOLINT

void
Obj::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Obj",
    { InstanceAccessor("stream", &Obj::GetStream, nullptr),
      InstanceAccessor("type", &Obj::GetDataType, nullptr),
      InstanceAccessor("length", &Obj::GetObjectLength, nullptr),
      InstanceAccessor("reference", &Obj::Reference, nullptr),
      InstanceAccessor("immutable", &Obj::GetImmutable, &Obj::SetImmutable),
      InstanceMethod("hasStream", &Obj::HasStream),
      InstanceMethod("getOffset", &Obj::GetByteOffset),
      InstanceMethod("write", &Obj::Write),
      InstanceMethod("flateCompressStream", &Obj::FlateCompressStream),
      InstanceMethod("delayedStreamLoad", &Obj::DelayedStreamLoad),
      InstanceMethod("getBool", &Obj::GetBool),
      InstanceMethod("getDictionary", &Obj::GetDictionary),
      InstanceMethod("getNumber", &Obj::GetNumber),
      InstanceMethod("getReal", &Obj::GetReal),
      InstanceMethod("getString", &Obj::GetString),
      InstanceMethod("getName", &Obj::GetName),
      InstanceMethod("getArray", &Obj::GetArray),
      InstanceMethod("getReference", &Obj::GetReference),
      InstanceMethod("getRawData", &Obj::GetRawData),
      InstanceMethod("clear", &Obj::Clear),
      InstanceMethod("eq", &Obj::Eq) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Obj", ctor);
}

Obj::Obj(const Napi::CallbackInfo& info)
  : ObjectWrap<Obj>(info)
{
  obj = make_shared<PdfObject>(*info[0].As<External<PdfObject>>().Data());
}

void
Obj::Clear(const Napi::CallbackInfo& info)
{
  try {
    obj->Clear();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Obj::GetStream(const CallbackInfo& info)
{
  try {
    auto pStream = dynamic_cast<PdfMemStream*>(obj->GetStream());
    auto stream = pStream->Get();
    auto length = pStream->GetLength();
    auto value =
      Buffer<char>::Copy(info.Env(), stream, static_cast<size_t>(length));
    return value;
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

Napi::Value
Obj::HasStream(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), obj->HasStream());
}

Napi::Value
Obj::GetObjectLength(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           obj->GetObjectLength(ePdfWriteMode_Default));
}

Napi::Value
Obj::GetImmutable(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), obj->GetImmutable());
}
void
Obj::SetImmutable(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsBoolean()) {
    try {
      obj->SetImmutable(value.As<Boolean>());
    } catch (PdfError& err) {
      ErrorHandler(err, info);
    }
  }
}

Napi::Value
Obj::GetDataType(const CallbackInfo& info)
{
  string js;
  if (obj->IsArray()) {
    js = "Array";
  } else if (obj->IsBool()) {
    js = "Boolean";
  } else if (obj->IsDictionary()) {
    js = "Dictionary";
  } else if (obj->IsEmpty()) {
    js = "Empty";
  } else if (obj->IsHexString()) {
    js = "HexString";
  } else if (obj->IsNull()) {
    js = "Null";
  } else if (obj->IsNumber()) {
    js = "Number";
  } else if (obj->IsName()) {
    js = "Name";
  } else if (obj->IsRawData()) {
    js = "RawData";
  } else if (obj->IsReal()) {
    js = "Real";
  } else if (obj->IsReference()) {
    js = "Reference";
  } else if (obj->IsString()) {
    js = "String";
  } else {
    js = "Unknown";
  }
  return Napi::String::New(info.Env(), js);
}

Napi::Value
Obj::Eq(const CallbackInfo& info)
{
  auto wrap = info[0].As<Object>();
  if (!wrap.InstanceOf(Obj::constructor.Value())) {
    throw Error::New(info.Env(), "Must be an instance of NoPoDoFo Obj");
  }
  auto value = Obj::Unwrap(wrap);
  return Boolean::New(info.Env(), value->GetObject() == this->GetObject());
}

Napi::Value
Obj::Reference(const CallbackInfo& info)
{
  try {
    PdfReference init = obj->Reference();
    auto initPtr = Napi::External<PdfReference>::New(info.Env(), &init);
    return Ref::constructor.New({ initPtr });
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

void
Obj::FlateCompressStream(const CallbackInfo& info)
{
  try {
    obj->FlateCompressStream();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Obj::DelayedStreamLoad(const CallbackInfo& info)
{
  try {
    obj->DelayedStreamLoad();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Obj::GetNumber(const CallbackInfo& info)
{
  if (!obj->IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }
  return Number::New(info.Env(), obj->GetNumber());
}

Napi::Value
Obj::GetReal(const CallbackInfo& info)
{
  if (!obj->IsReal()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }

  return Number::New(info.Env(), obj->GetReal());
}

Napi::Value
Obj::GetString(const CallbackInfo& info)
{
  if (!obj->IsString()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a String");
  }

  return String::New(info.Env(), obj->GetString().GetStringUtf8());
}

Napi::Value
Obj::GetName(const CallbackInfo& info)
{
  if (!obj->IsName()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a string");
  }
  try {
    string name = obj->GetName().GetName();
    return String::New(info.Env(), name);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

Napi::Value
Obj::GetArray(const CallbackInfo& info)
{
  if (!obj->IsArray()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as array");
  }
  auto instance = Array::constructor.New({ this->Value() });
  return instance;
}

Napi::Value
Obj::GetBool(const CallbackInfo& info)
{
  if (!obj->IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a boolean");
  }
  return Boolean::New(info.Env(), obj->GetBool());
}

Napi::Value
Obj::GetReference(const CallbackInfo& info)
{
  if (!obj->IsReference()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as Ref");
  }
  EscapableHandleScope scope(info.Env());
  auto init = obj->GetReference();
  auto ptr = External<PdfReference>::New(info.Env(), &init);
  auto instance = Ref::constructor.New({ ptr });
  //  return scope.Escape(instance);
  return instance;
}

Napi::Value
Obj::GetDictionary(const CallbackInfo& info)
{
  if (!obj->IsDictionary()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as Dictionary");
  }
  return Dictionary::constructor.New({ this->Value() });
}

Napi::Value
Obj::GetRawData(const CallbackInfo& info)
{
  if (!obj->IsRawData()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a buffer");
  }
  string data = obj->GetRawData().data();
  return Buffer<char>::Copy(info.Env(), data.c_str(), data.length());
}

class ObjOffsetAsync : public Napi::AsyncWorker
{
public:
  ObjOffsetAsync(Napi::Function& cb, Obj* obj, string arg)
    : Napi::AsyncWorker(cb)
    , obj(obj)
    , arg(std::move(arg))
  {}

protected:
  void Execute() override
  {
    try {
      auto o = obj->GetObject();
      value = o->GetByteOffset(arg.c_str(), ePdfWriteMode_Default);
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Napi::Error& err) {
      SetError(err.Message());
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), Number::New(Env(), value) });
  }

private:
  Obj* obj;
  string arg;
  long value = -1;
};

Napi::Value
Obj::GetByteOffset(const CallbackInfo& info)
{

  string arg = info[0].As<String>().Utf8Value();
  auto cb = info[1].As<Function>();
  ObjOffsetAsync* worker = new ObjOffsetAsync(cb, this, arg);
  worker->Queue();
  return info.Env().Undefined();
}

class ObjWriteAsync : public Napi::AsyncWorker
{
public:
  ObjWriteAsync(Napi::Function& cb, Obj* obj, string dest)
    : AsyncWorker(cb)
    , obj(obj)
    , arg(std::move(dest))
  {}

protected:
  void Execute() override
  {
    try {
      PdfOutputDevice device(arg.c_str());
      obj->GetObject()->WriteObject(&device, ePdfWriteMode_Default, nullptr);
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Napi::Error& err) {
      SetError(err.Message());
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), arg) });
  }

private:
  Obj* obj;
  string arg;
};

Napi::Value
Obj::Write(const CallbackInfo& info)
{
  auto cb = info[1].As<Function>();
  ObjWriteAsync* worker =
    new ObjWriteAsync(cb, this, info[0].As<String>().Utf8Value());
  worker->Queue();
  return info.Env().Undefined();
}
}
