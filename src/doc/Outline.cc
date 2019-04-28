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
#include "../Defines.h"
#include "../base/Color.h"
#include "../base/Obj.h"
#include "Action.h"
#include "Destination.h"
#include "Document.h"
#include "StreamDocument.h"
#include <algorithm>
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::string;

namespace NoPoDoFo {

FunctionReference Outline::constructor; // NOLINT

Outline::Outline(const CallbackInfo& info)
  : ObjectWrap(info)
  , outline(*info[0].As<External<PdfOutlineItem>>().Data())
{
  dbglog = spdlog::get("DbgLog");
}
Outline::~Outline()
{
  dbglog->debug("Outline Cleanup");
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
Outline::SetDestination(const Napi::CallbackInfo& /*info*/,
                        const Napi::Value& value)
{
  auto child = new PdfDestination(
    Destination::Unwrap(value.As<Object>())->GetDestination());
  GetOutline().SetDestination(*child);
}
Napi::Value
Outline::CreateChild(const Napi::CallbackInfo& info)
{
  string name = info[0].As<String>().Utf8Value();
  auto d = Destination::Unwrap(info[1].As<Object>())->GetDestination();
  PdfOutlineItem* child = GetOutline().CreateChild(name, d);
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), child) });
}
Napi::Value
Outline::CreateNext(const Napi::CallbackInfo& info)
{
  string name = info[0].As<String>().Utf8Value();
  if (info[1].As<Object>().InstanceOf(Destination::Constructor.Value())) {
    auto d = Destination::Unwrap(info[1].As<Object>())->GetDestination();
    PdfOutlineItem* item = GetOutline().CreateNext(name, d);
    return Outline::constructor.New(
      { External<PdfOutlineItem>::New(info.Env(), item) });
  }
  if (info[1].As<Object>().InstanceOf(Action::Constructor.Value())) {
    PdfAction& action = Action::Unwrap(info[1].As<Object>())->GetAction();
    auto item = GetOutline().CreateNext(name, action);
    return Outline::constructor.New(
      { External<PdfOutlineItem>::New(info.Env(), item) });
  }
  return info.Env().Undefined();
}
Napi::Value
Outline::InsertChild(const Napi::CallbackInfo& info)
{
  GetOutline().InsertChild(&Outline::Unwrap(info[0].As<Object>())->outline);
  return info.Env().Undefined();
}
Napi::Value
Outline::Prev(const Napi::CallbackInfo& info)
{
  auto prev = GetOutline().Prev();
  if (!prev)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), prev) });
}
Napi::Value
Outline::Next(const Napi::CallbackInfo& info)
{
  auto next = GetOutline().Next();
  if (!next)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), next) });
}
Napi::Value
Outline::First(const Napi::CallbackInfo& info)
{
  auto first = GetOutline().First();
  if (!first)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), first) });
}
Napi::Value
Outline::Last(const Napi::CallbackInfo& info)
{
  auto last = GetOutline().Last();
  if (!last)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), last) });
}
Napi::Value
Outline::GetParent(const Napi::CallbackInfo& info)
{
  return Outline::constructor.New({ External<PdfOutlineItem>::New(
    info.Env(), GetOutline().GetParentOutline()) });
}
Napi::Value
Outline::Erase(const Napi::CallbackInfo& info)
{
  GetOutline().Erase();
  return info.Env().Undefined();
}
Napi::Value
Outline::GetDestination(const Napi::CallbackInfo& info)
{
  PdfDestination* d;
  if (info[0].As<Object>().InstanceOf(Document::Constructor.Value())) {
    d =
      GetOutline().GetDestination(Document::Unwrap(info[0].As<Object>())->Base);
    if (!d)
      return info.Env().Null();
  } else if (info[0].As<Object>().InstanceOf(
               StreamDocument::constructor.Value())) {
    d = GetOutline().GetDestination(
      StreamDocument::Unwrap(info[0].As<Object>())->Base);
    if (!d)
      return info.Env().Null();
  } else {
    TypeError::New(info.Env(), "A StreamDocument or Document is required")
      .ThrowAsJavaScriptException();
    return {};
  }
  return Destination::Constructor.New(
    { External<PdfDestination>::New(info.Env(), d) });
}
Napi::Value
Outline::GetAction(const Napi::CallbackInfo& info)
{
  PdfAction* a = GetOutline().GetAction();
  if (!a)
    return info.Env().Null();
  return Action::Constructor.New({ External<PdfAction>::New(info.Env(), a) });
}
void
Outline::SetAction(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetOutline().SetAction(Action::Unwrap(value.As<Object>())->GetAction());
}
Napi::Value
Outline::GetTitle(const Napi::CallbackInfo& info)
{
  string title = GetOutline().GetTitle().GetStringUtf8();
  return String::New(info.Env(), title);
}
void
Outline::SetTitle(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetOutline().SetTitle(PdfString(value.As<String>().Utf8Value()));
}
Napi::Value
Outline::GetTextFormat(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetOutline().GetTextFormat());
}
void
Outline::SetTextFormat(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetOutline().SetTextFormat(
    static_cast<EPdfOutlineFormat>(value.As<Number>().Int32Value()));
}
Napi::Value
Outline::GetTextColor(const Napi::CallbackInfo& info)
{
  return Color::Constructor.New({
    Number::New(info.Env(), GetOutline().GetTextColorRed()),
    Number::New(info.Env(), GetOutline().GetTextColorGreen()),
    Number::New(info.Env(), GetOutline().GetTextColorBlue()),
  });
}
void
Outline::SetTextColor(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsObject()) {
    TypeError::New(info.Env(), "TextColor must be an instance of Color")
      .ThrowAsJavaScriptException();
    return;
  }
  PdfColor color = *Color::Unwrap(value.As<Object>())->Self;
  if (!color.IsRGB()) {
    cout << "Outline text coloring only supports color format RGB, color "
            "automatically converted to RGB"
         << endl;
    color = color.ConvertToRGB();
  }
  GetOutline().SetTextColor(color.GetRed(), color.GetGreen(), color.GetBlue());
}
}
