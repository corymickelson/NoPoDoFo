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

#include "Destination.h"
#include "../base/Dictionary.h"

#include "../ValidateArguments.h"
#include "../base/Obj.h"
#include "./Page.h"
#include "./Rect.h"
#include <iostream>

using namespace PoDoFo;
using namespace Napi;

using std::cout;
using std::endl;
using std::string;
using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Destination::constructor; // NOLINT

Destination::Destination(const CallbackInfo& info)
  : ObjectWrap(info)
{
  vector<int> opts = AssertCallbackInfo(
    info,
    { { 0, { option(napi_external), option(napi_object) } },
      { 1, { option(napi_number), option(napi_object), nullopt } },
      { 2, { option(napi_number), nullopt } },
      { 3, { option(napi_number), nullopt } } });
  // Create a copy of an existing object
  if (opts[0] == 0) {
    destination =
      new PdfDestination(*info[0].As<External<PdfDestination>>().Data());
  }
  // Create a new Destination object
  else if (opts[0] == 1) {
    auto noPage = Page::Unwrap(info[0].As<Object>());
    auto doc = noPage->page.GetObject()->GetOwner()->GetParentDocument();
    auto i = noPage->page.GetPageNumber();
    if (opts[1] == 0) {
      // PdfDestination(PdfPage, EPdfDestinationFit)
      if (opts[2] == 1) {
        destination = new PdfDestination(
          doc->GetPage(i),
          static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value()));
      }
      // PdfDestination(PdfPage, EPdfDestinationFit, FitArgument)
      else if (opts[2] == 0) {
        destination = new PdfDestination(
          doc->GetPage(i),
          static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value()),
          info[2].As<Number>().DoubleValue());
      }
      // PdfDestination(PdfPage, left, top, zoom)
      else if (opts[2] == 0 && opts[3] == 0) {
        destination =
          new PdfDestination(doc->GetPage(i),
                             info[1].As<Number>().DoubleValue(),  // left
                             info[2].As<Number>().DoubleValue(),  // top
                             info[3].As<Number>().DoubleValue()); // zoom
      }

    } else if (opts[1] == 1) {
      destination = new PdfDestination(
        doc->GetPage(i), Rect::Unwrap(info[1].As<Object>())->GetRect());
    } else {
      Error::New(info.Env()).ThrowAsJavaScriptException();
    }
  }
}
Destination::~Destination()
{
  HandleScope scope(Env());
//  delete page;
  delete destination;
  destination = nullptr;
}
void
Destination::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Destination",
    { InstanceMethod("type", &Destination::GetType),
      InstanceMethod("zoom", &Destination::GetZoom),
      InstanceMethod("rect", &Destination::GetRect),
      InstanceMethod("top", &Destination::GetTop),
      InstanceMethod("left", &Destination::GetLeft),
      InstanceMethod("d", &Destination::GetDValue),
      InstanceMethod("getObject", &Destination::GetObject),
      InstanceMethod("addToDictionary", &Destination::AddToDictionary) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Destination", ctor);
}
Napi::Value
Destination::GetType(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetDestination().GetType());
}
Napi::Value
Destination::GetZoom(const Napi::CallbackInfo& info)
{
  if (GetDestination().GetType() != ePdfDestinationType_XYZ) {
    Error::New(info.Env(), "Destination must be of type XYZ to get zoom value.")
      .ThrowAsJavaScriptException();
    return {};
  }
  return Number::New(info.Env(), GetDestination().GetZoom());
}
Napi::Value
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
Napi::Value
Destination::GetTop(const Napi::CallbackInfo& info)
{
  EPdfDestinationType t = GetDestination().GetType();
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
Napi::Value
Destination::GetLeft(const Napi::CallbackInfo& info)
{
  EPdfDestinationType t = GetDestination().GetType();
  if (t == ePdfDestinationType_FitR || t == ePdfDestinationType_FitV ||
      t == ePdfDestinationType_XYZ) {
    return Number::New(info.Env(), GetDestination().GetLeft());
  } else {
    Error::New(info.Env(), "Destination must be of type FitV, FitR or FitXYZ")
      .ThrowAsJavaScriptException();
    return Value();
  }
}
Napi::Value
Destination::GetDValue(const Napi::CallbackInfo& info)
{
  EPdfDestinationType t = GetDestination().GetType();
  if (t == ePdfDestinationType_FitH || t == ePdfDestinationType_FitV ||
      t == ePdfDestinationType_FitBH) {
    return Number::New(info.Env(), GetDestination().GetDValue());
  } else {
    Error::New(info.Env(), "Destination must be of type FitV, FitH or FitBH")
      .ThrowAsJavaScriptException();
    return Value();
  }
}
Napi::Value
Destination::GetObject(const Napi::CallbackInfo& info)
{
  return Obj::constructor.New(
    { External<PdfObject>::New(info.Env(), GetDestination().GetObject()) });
}

void
Destination::AddToDictionary(const Napi::CallbackInfo& info)
{
  if (info[0].As<Object>().InstanceOf(Dictionary::constructor.Value())) {
    PdfDictionary d = Dictionary::Unwrap(info[0].As<Object>())->GetDictionary();
    GetDestination().AddToDictionary(d);
  }
}
}
