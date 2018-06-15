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

#include "Outline.h"
#include "../base/Obj.h"

using namespace Napi;
using namespace PoDoFo;

using std::make_unique;

namespace NoPoDoFo {

FunctionReference Outline::constructor; // NOLINT

Outline::Outline(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() < 1) {
    Error::New(info.Env(), "Outlines requires one of: Obj, External<PdfObject>")
      .ThrowAsJavaScriptException();
    return;
  } else if (info[0].IsExternal()) {
    outlines =
      make_unique<PdfOutlines>(info[0].As<External<PdfObject>>().Data());
  } else if (info[0].IsObject() &&
             info[0].As<Object>().InstanceOf(Obj::constructor.Value())) {
    outlines =
      make_unique<PdfOutlines>(&Obj::Unwrap(info[0].As<Object>())->GetObject());
  } else {
    TypeError::New(info.Env(),
                   "Outlines requires one of: Obj, External<PdfObject>")
      .ThrowAsJavaScriptException();
    return;
  }
}

void
Outline::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
    env,
    "Outline",
    { InstanceAccessor("prev", &Outline::Prev, nullptr),
      InstanceAccessor("next", &Outline::Next, nullptr),
      InstanceAccessor("first", &Outline::First, nullptr),
      InstanceAccessor("last", &Outline::Last, nullptr),
      InstanceAccessor(
        "destination", &Outline::GetDestination, &Outline::SetDestination),
      InstanceAccessor("action", &Outline::GetAction, &Outline::SetAction),
      InstanceAccessor("title", &Outline::GetTitle, &Outline::SetTitle),
      InstanceAccessor(
        "textFormat", &Outline::GetTextFormat, &Outline::SetTextFormat),
      InstanceAccessor(
        "textColor", &Outline::GetTextColor, &Outline::SetTextColor),
      InstanceMethod("createChild", &Outline::CreateChild),
      InstanceMethod("createNext", &Outline::CreateNext),
      InstanceMethod("insertChild", &Outline::InsertChild),
      InstanceMethod("getParent", &Outline::GetParent),
      InstanceMethod("erase", &Outline::Erase) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Outline", ctor);
}
void
Outline::SetDestination(const Napi::CallbackInfo&, const Napi::Value&)
{}
Napi::Value
Outline::CreateChild(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::CreateNext(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::InsertChild(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::Prev(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::Next(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::First(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::Last(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::GetParent(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::Erase(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::GetDestination(const Napi::CallbackInfo&)
{
  return Value();
}
Napi::Value
Outline::GetAction(const Napi::CallbackInfo&)
{
  return Value();
}
void
Outline::SetAction(const Napi::CallbackInfo&, const Napi::Value&)
{}
Napi::Value
Outline::GetTitle(const Napi::CallbackInfo&)
{
  return Value();
}
void
Outline::SetTitle(const Napi::CallbackInfo&, const Napi::Value&)
{}
Napi::Value
Outline::GetTextFormat(const Napi::CallbackInfo&)
{
  return Value();
}
void
Outline::SetTextFormat(const Napi::CallbackInfo&, const Napi::Value&)
{}
Napi::Value
Outline::GetTextColor(const Napi::CallbackInfo&)
{
  return Value();
}
void
Outline::SetTextColor(const Napi::CallbackInfo&, const Napi::Value&)
{}
}
