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
#include "Page.h"
#include "../base/Obj.h"
#include "Action.h"
#include "Destination.h"
#include "Document.h"
#include "StreamDocument.h"

using namespace Napi;
using namespace PoDoFo;

using std::make_unique;
using std::string;

namespace NoPoDoFo {

FunctionReference Outline::constructor; // NOLINT

Outline::Outline(const CallbackInfo& info)
  : ObjectWrap(info)
  , outline(*info[0].As<External<PdfOutlineItem>>().Data())
{}
Outline::~Outline()
{
  HandleScope scope(Env());
  cout << "Removing Outline" << endl;
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
      //      InstanceMethod("createRoot", &Outline::CreateRoot),
      InstanceMethod("createNext", &Outline::CreateNext),
      InstanceMethod("insertChild", &Outline::InsertChild),
      InstanceMethod("getParent", &Outline::GetParent),
      InstanceMethod("erase", &Outline::Erase) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Outline", ctor);
}
void
Outline::SetDestination(const Napi::CallbackInfo& info,
                        const Napi::Value& value)
{
  auto child = new PdfDestination(
    Destination::Unwrap(value.As<Object>())->GetDestination());
//  destinations.emplace_back(child);
  outline.SetDestination(*child);
}
Napi::Value
Outline::CreateChild(const Napi::CallbackInfo& info)
{
  string name = info[0].As<String>().Utf8Value();
//  auto page = Page::Unwrap(info[1].As<Object>())->page;
  auto d =
    Destination::Unwrap(info[1].As<Object>())->GetDestination();
//  auto child = new PdfDestination(d);
//  destinations.emplace_back(child);
  PdfOutlineItem* item = outline.CreateChild(name, d);
//  delete child;
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), item) });
}
Napi::Value
Outline::CreateNext(const Napi::CallbackInfo& info)
{
  string name = info[0].As<String>().Utf8Value();
  if (info[1].As<Object>().InstanceOf(Destination::constructor.Value())) {
    auto d =
      Destination::Unwrap(info[1].As<Object>())->GetDestination();
//    auto child = new PdfDestination(d);
//    destinations.emplace_back(child);
    PdfOutlineItem* item = outline.CreateNext(name, d);
    return Outline::constructor.New(
      { External<PdfOutlineItem>::New(info.Env(), item) });
  } else if (info[1].As<Object>().InstanceOf(Action::constructor.Value())) {
    PdfAction* action = Action::Unwrap(info[1].As<Object>())->GetAction();
    auto item = outline.CreateNext(name, *action);
    return Outline::constructor.New(
      { External<PdfOutlineItem>::New(info.Env(), item) });
  }
  return info.Env().Undefined();
}
Napi::Value
Outline::InsertChild(const Napi::CallbackInfo& info)
{
  outline.InsertChild(&Outline::Unwrap(info[0].As<Object>())->outline);
  return info.Env().Undefined();
}
Napi::Value
Outline::Prev(const Napi::CallbackInfo& info)
{
  auto prev = outline.Prev();
  if (!prev)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), prev) });
}
Napi::Value
Outline::Next(const Napi::CallbackInfo& info)
{
  auto next = outline.Next();
  if (!next)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), next) });
}
Napi::Value
Outline::First(const Napi::CallbackInfo& info)
{
  auto first = outline.First();
  if (!first)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), first) });
}
Napi::Value
Outline::Last(const Napi::CallbackInfo& info)
{
  auto last = outline.Last();
  if (!last)
    return info.Env().Null();
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), last) });
}
Napi::Value
Outline::GetParent(const Napi::CallbackInfo& info)
{
  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), outline.GetParentOutline()) });
}
Napi::Value
Outline::Erase(const Napi::CallbackInfo& info)
{
  outline.Erase();
  return info.Env().Undefined();
}
Napi::Value
Outline::GetDestination(const Napi::CallbackInfo& info)
{
  PdfDestination* d;
  if (info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
    d = outline.GetDestination(Document::Unwrap(info[0].As<Object>())->base);
    if (!d)
      return info.Env().Null();
  } else if (info[0].As<Object>().InstanceOf(
               StreamDocument::constructor.Value())) {
    d = outline.GetDestination(
      StreamDocument::Unwrap(info[0].As<Object>())->base);
    if (!d)
      return info.Env().Null();
  } else {
    TypeError::New(info.Env(), "A StreamDocument or Document is required")
      .ThrowAsJavaScriptException();
    return {};
  }
  return Destination::constructor.New(
    { External<PdfDestination>::New(info.Env(), d) });
}
Napi::Value
Outline::GetAction(const Napi::CallbackInfo& info)
{
  PdfAction* a = outline.GetAction();
  if (!a)
    return info.Env().Null();
  return Action::constructor.New({ External<PdfAction>::New(info.Env(), a) });
}
void
Outline::SetAction(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  outline.SetAction(*Action::Unwrap(value.As<Object>())->GetAction());
}
Napi::Value
Outline::GetTitle(const Napi::CallbackInfo& info)
{
  return String::New(info.Env(), outline.GetTitle().GetStringUtf8());
}
void
Outline::SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  outline.SetTitle(PdfString(value.As<String>().Utf8Value()));
}
Napi::Value
Outline::GetTextFormat(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), outline.GetTextFormat());
}
void
Outline::SetTextFormat(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  outline.SetTextFormat(
    static_cast<EPdfOutlineFormat>(value.As<Number>().Int32Value()));
}
Napi::Value
Outline::GetTextColor(const Napi::CallbackInfo& info)
{
  double r = outline.GetTextColorRed();
  double b = outline.GetTextColorBlue();
  double g = outline.GetTextColorGreen();
  Array a = Array::New(info.Env());
  a.Set(static_cast<uint32_t>(0), Number::New(info.Env(), r));
  a.Set(static_cast<uint32_t>(1), Number::New(info.Env(), g));
  a.Set(static_cast<uint32_t>(2), Number::New(info.Env(), b));
  return a;
}
void
Outline::SetTextColor(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  Napi::Array a = value.As<Napi::Array>();
  double r = a.Get(static_cast<uint32_t>(0)).As<Number>();
  double g = a.Get(static_cast<uint32_t>(1)).As<Number>();
  double b = a.Get(static_cast<uint32_t>(2)).As<Number>();
  outline.SetTextColor(r, g, b);
}
}
