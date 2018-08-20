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

#include "Array.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../doc/Document.h"
#include "Obj.h"

using namespace Napi;
using namespace PoDoFo;

using std::string;

namespace NoPoDoFo {

Napi::FunctionReference Array::constructor; // NOLINT

Array::Array(const CallbackInfo& info)
  : ObjectWrap<Array>(info)
  , obj(Obj::Unwrap(info[0].As<Object>())->GetObject())
{}

Array::~Array()
{
  HandleScope scope(Env());
  for (auto child : children) {
    delete child;
  }
}
void
Array::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Array",
    { InstanceAccessor("dirty", &Array::IsDirty, &Array::SetDirty),
      InstanceAccessor("length", &Array::Length, nullptr),
      InstanceAccessor("immutable", &Array::GetImmutable, &Array::SetImmutable),
      InstanceMethod("at", &Array::At),
      InstanceMethod("containsString", &Array::ContainsString),
      InstanceMethod("indexOf", &Array::GetStringIndex),
      InstanceMethod("write", &Array::Write),
      InstanceMethod("push", &Array::Push),
      InstanceMethod("pop", &Array::Pop),
      InstanceMethod("clear", &Array::Clear) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Array", ctor);
}

Napi::Value
Array::GetImmutable(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetArray().GetImmutable());
}

void
Array::SetImmutable(const CallbackInfo& info, const Napi::Value& value)
{
  try {
    GetArray().SetImmutable(value.As<Boolean>());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Array::Length(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetArray().size());
}
void
Array::Write(const CallbackInfo& info)
{
  string output = info[0].As<String>().Utf8Value();
  PdfOutputDevice device(output.c_str());
  GetArray().Write(&device, ePdfWriteMode_Default);
}
Napi::Value
Array::ContainsString(const CallbackInfo& info)
{
  string searchString = info[0].As<String>().Utf8Value();
  bool match = GetArray().ContainsString(searchString);
  return Napi::Boolean::New(info.Env(), match);
}
Napi::Value
Array::GetStringIndex(const CallbackInfo& info)
{
  string str = info[0].As<String>().Utf8Value();
  return Napi::Number::New(info.Env(), GetArray().GetStringIndex(str));
}

Napi::Value
Array::IsDirty(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetArray().IsDirty());
}

Napi::Value
Array::At(const CallbackInfo& info)
{
  return GetObjAtIndex(info);
}

void
Array::SetDirty(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::Error::New(info.Env(), "dirty must be of type boolean");
  }
  GetArray().SetDirty(value.As<Boolean>());
}

void
Array::Push(const CallbackInfo& info)
{
  auto wrapper = info[0].As<Object>();
  if (!wrapper.InstanceOf(Obj::constructor.Value())) {
    throw Error::New(info.Env(), "must be an instance of Obj");
  }
  try {
    auto item = Obj::Unwrap(wrapper);
    GetArray().push_back(item->GetObject());

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Value
Array::Pop(const CallbackInfo& info)
{
  Napi::Value item = GetObjAtIndex(info);
  size_t index = info[0].As<Number>().Uint32Value();
  GetArray().erase(GetArray().begin() + index);
  return item;
}

Value
Array::GetObjAtIndex(const CallbackInfo& info)
{
  size_t index = info[0].As<Number>().Uint32Value();
  if (index > GetArray().size()) {
    throw Napi::RangeError();
  }
  PdfObject* item;
  if (GetArray()[index].IsReference()) {
    PdfReference indirect = GetArray()[index].GetReference();
    auto owner = obj.GetOwner();
    if (!owner) {
      Napi::Array ref = Napi::Array::New(info.Env(), 2);
      ref.Set(static_cast<uint32_t>(0), indirect.GenerationNumber());
      ref.Set(static_cast<uint32_t>(1), indirect.ObjectNumber());
      return ref;
    }
    item = obj.GetOwner()->GetObject(GetArray()[index].GetReference());

  } else {
    item = &(GetArray()[index]);
  }
  auto child = new PdfObject(*item);
  children.push_back(child);
  auto initPtr = Napi::External<PdfObject>::New(info.Env(), child);
  auto instance = Obj::constructor.New({ initPtr });
  return instance;
}

void
Array::Clear(const CallbackInfo& info)
{
  try {
    GetArray().Clear();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
}
