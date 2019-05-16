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
#include "Data.h"
#include "Date.h"
#include "Dictionary.h"
#include "Obj.h"
#include "Ref.h"
#include "spdlog/spdlog.h"

using namespace Napi;
using namespace PoDoFo;

using std::string;

namespace NoPoDoFo {

FunctionReference Array::Constructor; // NOLINT

/**
 * The NoPoDoFo::Array constructor excepts the following arguments:
 *  (PdfExternal, Number)
 *  if the Number is 0, the external is a pointer to a PdfObject
 *  if the number is 1, the external is a pointer to a PdfArray
 *  if no arguments are provided a new PdfArray is created
 * @param info
 */
Array::Array(const CallbackInfo& info)
  : ObjectWrap<Array>(info)
  , Self(
      info.Length() == 2
        ? (info[0].IsExternal() && info[1].IsNumber() &&
               info[1].As<Number>().Int32Value() == 0
             ? (Parent = info[0].As<External<PdfObject>>().Data())->GetArray()
             : *info[0].As<External<PdfArray>>().Data())
        : *(Init = new PdfArray()))
{
  DbgLog = spdlog::get("DbgLog");
  if (Init != nullptr) {
    DbgLog->debug("Initialized New Array");
  }
}

Array::~Array()
{
  DbgLog->debug("Array cleanup");
  HandleScope scope(Env());
  for (auto child : Children) {
    delete child;
  }
  delete Init;
  if (Parent) {
    Parent = nullptr;
  }
}
void
Array::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
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
      InstanceMethod("asArray", &Array::ToJsArray),
      InstanceMethod("clear", &Array::Clear),
      InstanceMethod("splice", &Array::Splice) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Array", ctor);
}

JsValue
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
JsValue
Array::Length(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetArray().size());
}
void
Array::Write(const CallbackInfo& info)
{
  const auto output = info[0].As<String>().Utf8Value();
  PdfOutputDevice device(output.c_str());
  GetArray().Write(&device, ePdfWriteMode_Default);
}
JsValue
Array::ContainsString(const CallbackInfo& info)
{
  const auto searchString = info[0].As<String>().Utf8Value();
  const auto match = GetArray().ContainsString(searchString);
  return Napi::Boolean::New(info.Env(), match);
}
JsValue
Array::GetStringIndex(const CallbackInfo& info)
{
  const auto str = info[0].As<String>().Utf8Value();
  return Napi::Number::New(info.Env(), GetArray().GetStringIndex(str));
}

JsValue
Array::IsDirty(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetArray().IsDirty());
}

JsValue
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
  const auto wrapper = info[0].As<Object>();
  if (!wrapper.InstanceOf(Obj::Constructor.Value())) {
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

JsValue
Array::Pop(const CallbackInfo& info)
{
  const auto copy = GetObjAtIndex(info);
  const size_t index = info[0].As<Number>().Uint32Value();
  GetArray().erase(GetArray().begin() + index);
  return copy;
}

JsValue
Array::GetObjAtIndex(const CallbackInfo& info)
{
  const size_t index = info[0].As<Number>().Uint32Value();
  if (index > GetArray().size()) {
    throw Napi::RangeError();
  }
  PdfObject* item;
  if (GetArray()[index].IsReference()) {
    auto indirect = GetArray()[index].GetReference();
    return Ref::Constructor.New(
      { External<PdfReference>::New(info.Env(), &indirect) });
  } else {
    item = &(GetArray()[index]);
  }
  // Create copy for shift and pop operations
  const auto child = new PdfObject(*item);
  DbgLog->debug("Array[{}] = {}", index, child->GetDataTypeString());
  Children.push_back(child);
  const auto initPtr = Napi::External<PdfObject>::New(info.Env(), child);
  const auto instance = Obj::Constructor.New({ initPtr });
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
JsValue
Array::ToJsArray(const Napi::CallbackInfo& info)
{
  Napi::Array js = Napi::Array::New(info.Env());
  auto it = GetArray().begin();
  while (it != GetArray().end()) {
    Obj::TrickleDown(info.Env(), nullptr, (*it), js);
    it++;
  }
  return js;
}
JsValue
Array::Splice(const Napi::CallbackInfo& info)
{
  const auto opts = AssertCallbackInfo(
    info,
    { { 0, { option(napi_number) } },
      { 1, { option(napi_number), option(napi_object), tl::nullopt } },
      { 2, { option(napi_object), tl::nullopt } } });
  const auto out = Napi::Array::New(info.Env());
  vector<PdfObject> spliceIn;
  const auto env = info.Env();
  const auto getPdfElement = [env](Napi::Value v) -> PdfObject {
    if (v.IsBoolean()) {
      return PdfObject(v.As<Boolean>());
    } else if (v.IsString()) {
      return PdfObject(v.As<String>());
    } else if (v.IsNumber()) {
      return PdfObject(v.As<Number>().Int64Value());
    } else if (v.IsObject()) {
      if (v.As<Object>().InstanceOf(Dictionary::Constructor.Value())) {
        return Dictionary::Unwrap(v.As<Object>())->GetDictionary();
      } else if (v.As<Object>().InstanceOf(Array::Constructor.Value())) {
        return Array::Unwrap(v.As<Object>())->GetArray();
      } else if (v.As<Object>().InstanceOf(Obj::Constructor.Value())) {
        return Obj::Unwrap(v.As<Object>())->GetObject();
      } else if (v.As<Object>().InstanceOf(Ref::Constructor.Value())) {
        const auto r = Ref::Unwrap(v.As<Object>())->Self;
        return PdfVariant(*r);
      }
    }
    throw std::exception();
  };
  if (opts[2] == 0 || opts[1] == 1) {
    try {
      if (info[opts[2] == 0 ? 2 : 1].IsArray()) {
        Napi::Array a = info[opts[2] == 0 ? 2 : 1].As<Napi::Array>();
        for (int i = 0; i < a.Length(); ++i) {
          const auto inn = getPdfElement(a.Get(i));
          spliceIn.emplace_back(inn);
        }
      } else {
        const auto inn = getPdfElement(info[opts[2] == 0 ? 2 : 1].As<Object>());
        spliceIn.emplace_back(inn);
      }
    } catch (...) {
      const auto msg =
        "Unknown type, please see the NoPoDoFo::Array docs for details";
      TypeError::New(env, msg).ThrowAsJavaScriptException();
    }
  }
  if (opts[1] == 0) {
    if (GetArray().size() <
        info[0].As<Number>().Int32Value() + info[1].As<Number>().Int32Value()) {
      const auto msg = "starting position plus count exceeds array length";
      DbgLog->debug(msg);
      RangeError::New(info.Env(), msg).ThrowAsJavaScriptException();
    }
    if (GetArray().GetImmutable()) {
      const auto msg = "Array is immutable";
      DbgLog->debug(msg);
      Error::New(info.Env(), msg).ThrowAsJavaScriptException();
    }
    auto it = GetArray().begin() + info[0].As<Number>().Uint32Value();
    while (it != GetArray().begin() + (info[0].As<Number>().Uint32Value() +
                                       info[1].As<Number>().Uint32Value())) {
      const auto item = Obj::Constructor.New(
        { Napi::External<PdfObject>::New(info.Env(), it.base()) });
      out[out.Length()] = item;
      GetArray().erase(it);
    }
  }
  if (!spliceIn.empty()) {
  }
  return out;
}
}
