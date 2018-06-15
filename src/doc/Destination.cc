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
#include "./Page.h"
#include <iostream>

using namespace PoDoFo;
using namespace Napi;

using std::cout;
using std::endl;
using std::make_unique;
using std::string;

namespace NoPoDoFo {

FunctionReference Destination::constructor; // NOLINT

Destination::Destination(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() == 1 && info[0].Type() == napi_external) {
    destination = make_unique<PdfDestination>(
      *info[0].As<External<PdfDestination>>().Data());
  } else if (info.Length() == 2 && info[0].IsObject() && info[1].IsNumber()) {
    PdfPage page = Page::Unwrap(info[0].As<Object>())->page;
    auto fit =
      static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value());
    destination = make_unique<PdfDestination>(&page, fit);
  } else {
    throw TypeError();
  }
}

void
Destination::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Destination",
    { InstanceAccessor("page", &Destination::GetPage, nullptr),
      InstanceAccessor("type", &Destination::GetType, nullptr),
      InstanceAccessor("zoom", &Destination::GetZoom, nullptr),
      InstanceAccessor("rect", &Destination::GetRect, nullptr),
      InstanceAccessor("top", &Destination::GetTop, nullptr),
      InstanceAccessor("left", &Destination::GetLeft, nullptr),
      InstanceAccessor("d", &Destination::GetDValue, nullptr),
      InstanceMethod("getObject", &Destination::GetObject),
      InstanceMethod("getArray", &Destination::GetArray),
      InstanceMethod("addToDictionary", &Destination::AddToDictionary) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Destination", ctor);
}
Napi::Value
Destination::GetPage(const Napi::CallbackInfo& info)
{
  //  destination->GetPage()->GetObject()->GetOwner()->GetParentDocument();
  return Value();
}
Napi::Value
Destination::GetType(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), destination->GetType());
}
Napi::Value
Destination::GetZoom(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Destination::GetRect(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Destination::GetTop(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Destination::GetLeft(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Destination::GetDValue(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Destination::GetObject(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Destination::GetArray(const Napi::CallbackInfo&)
{
  return Value();
}
void
Destination::AddToDictionary(const Napi::CallbackInfo&)
{}
}
