/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2019
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

#include "Destination.h"
#include "../base/Dictionary.h"

#include "../Defines.h"
#include "../ValidateArguments.h"
#include "../base/Obj.h"
#include "./Page.h"
#include "./Rect.h"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace PoDoFo;
using namespace Napi;

using std::cout;
using std::endl;
using std::string;
using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Destination::Constructor; // NOLINT

Destination::Destination(const CallbackInfo& info)
  : ObjectWrap(info)
{
  Log = spdlog::get("Log");
  auto opts = AssertCallbackInfo(
    info,
    { { 0, { option(napi_external), option(napi_object) } },
      { 1, { option(napi_number), option(napi_object), nullopt } },
      { 2, { option(napi_number), nullopt } },
      { 3, { option(napi_number), nullopt } } });
  // Create a copy of an existing object
  if (opts[0] == 0) {
    Self =
      new PdfDestination(*info[0].As<External<PdfDestination>>().Data());
  }
  // Create a new Destination object
  else if (opts[0] == 1) {
    const auto page = Page::Unwrap(info[0].As<Object>());
    if (opts[1] == 0) {
      if (opts[2] == 1) {
        Self = new PdfDestination(
          &page->Self,
          static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value()));
      } else if (opts[2] == 0) {
        Self = new PdfDestination(
          &page->Self,
          static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value()),
          info[2].As<Number>().DoubleValue());
      } else if (opts[2] == 0 && opts[3] == 0) {
        Self =
          new PdfDestination(&page->Self,
                             info[1].As<Number>().DoubleValue(),  // left
                             info[2].As<Number>().DoubleValue(),  // top
                             info[3].As<Number>().DoubleValue()); // zoom
      }

    } else if (opts[1] == 1) {
      Self = new PdfDestination(
        &page->Self, Rect::Unwrap(info[1].As<Object>())->GetRect());
    } else {
      Error::New(info.Env()).ThrowAsJavaScriptException();
    }
  }
}
Destination::~Destination()
{
  Logger(Log, spdlog::level::trace, "Destination Cleanup");
  delete Self;
  Self = nullptr;
}
void
Destination::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  const char* name = "Destination";
  Function ctor = DefineClass(
    env,
    name,
    { InstanceMethod("type", &Destination::GetType),
      InstanceMethod("zoom", &Destination::GetZoom),
      InstanceMethod("rect", &Destination::GetRect),
      InstanceMethod("top", &Destination::GetTop),
      InstanceMethod("left", &Destination::GetLeft),
      InstanceMethod("d", &Destination::GetDValue),
      InstanceMethod("getObject", &Destination::GetObject),
      InstanceMethod("addToDictionary", &Destination::AddToDictionary) });
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set(name, ctor);
}
JsValue
Destination::GetType(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetDestination().GetType());
}
JsValue
Destination::GetZoom(const Napi::CallbackInfo& info)
{
  if (GetDestination().GetType() != ePdfDestinationType_XYZ) {
    Error::New(info.Env(), "Destination must be of type XYZ to get zoom value.")
      .ThrowAsJavaScriptException();
    return {};
  }
  return Number::New(info.Env(), GetDestination().GetZoom());
}
JsValue
Destination::GetRect(const Napi::CallbackInfo& info)
{
  if (GetDestination().GetType() != ePdfDestinationType_FitR) {
    Error::New(info.Env(), "Destination must be of type FitR to get Rect value")
      .ThrowAsJavaScriptException();
    return {};
  }
  auto r = GetDestination().GetRect();
  return Rect::constructor.New({ External<PdfRect>::New(info.Env(), &r) });
}
JsValue
Destination::GetTop(const Napi::CallbackInfo& info)
{
  const EPdfDestinationType t = GetDestination().GetType();
  if (t == ePdfDestinationType_FitR || t == ePdfDestinationType_FitH ||
      t == ePdfDestinationType_XYZ || t == ePdfDestinationType_FitBH) {
    return Number::New(info.Env(), GetDestination().GetTop());
  } else {
    Error::New(info.Env(),
               "Destination must be of type FitH, FitR, FitBH or FitXYZ")
      .ThrowAsJavaScriptException();
    return Value();
  }
}
JsValue
Destination::GetLeft(const Napi::CallbackInfo& info)
{
  const EPdfDestinationType t = GetDestination().GetType();
  if (t == ePdfDestinationType_FitR || t == ePdfDestinationType_FitV ||
      t == ePdfDestinationType_XYZ) {
    return Number::New(info.Env(), GetDestination().GetLeft());
  } else {
    Error::New(info.Env(), "Destination must be of type FitV, FitR or FitXYZ")
      .ThrowAsJavaScriptException();
    return Value();
  }
}
JsValue
Destination::GetDValue(const Napi::CallbackInfo& info)
{
  const EPdfDestinationType t = GetDestination().GetType();
  if (t == ePdfDestinationType_FitH || t == ePdfDestinationType_FitV ||
      t == ePdfDestinationType_FitBH) {
    return Number::New(info.Env(), GetDestination().GetDValue());
  } else {
    Error::New(info.Env(), "Destination must be of type FitV, FitH or FitBH")
      .ThrowAsJavaScriptException();
    return Value();
  }
}
JsValue
Destination::GetObject(const Napi::CallbackInfo& info)
{
  return Obj::Constructor.New(
    { External<PdfObject>::New(info.Env(), GetDestination().GetObject()) });
}

void
Destination::AddToDictionary(const Napi::CallbackInfo& info)
{
  if (info[0].As<Object>().InstanceOf(Dictionary::Constructor.Value())) {
    auto d = Dictionary::Unwrap(info[0].As<Object>())->GetDictionary();
    GetDestination().AddToDictionary(d);
  }
}
}
