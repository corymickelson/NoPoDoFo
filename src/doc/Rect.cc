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

#include "Rect.h"
#include "../ValidateArguments.h"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;
using std::cout;
using std::endl;
using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Rect::constructor; // NOLINT

Rect::Rect(const CallbackInfo& info)
  : ObjectWrap(info)
{
  DbgLog = spdlog::get("DbgLog");
  vector<int> opts = AssertCallbackInfo(
    info,
    {
      { 0, { nullopt, option(napi_external), option(napi_number) } },
      { 1, { nullopt, option(napi_number) } },
      { 2, { nullopt, option(napi_number) } },
      { 3, { nullopt, option(napi_number) } },
    });
  switch (opts[0]) {
    case 0:
      Self = new PdfRect();
      break;
    case 1:
      Self = new PdfRect(*info[0].As<External<PdfRect>>().Data());
      break;
    case 2:
      if (opts[1] == 0 || opts[2] == 0 || opts[3] == 0) {
        Error::New(info.Env(),
                   "Rectangle requires 4 real number for left, bottom, width, "
                   "and height")
          .ThrowAsJavaScriptException();
        return;
      }
      Self = new PdfRect(info[0].As<Number>().DoubleValue(),
                         info[1].As<Number>().DoubleValue(),
                         info[2].As<Number>().DoubleValue(),
                         info[3].As<Number>().DoubleValue());
      break;
    default:
      cout << "WTF error: Rect constructor failed to parse args and find valid "
              "constructor"
           << endl;
      break;
  }
}

Rect::~Rect()
{
  DbgLog->debug("Rect Cleanup");
  HandleScope scope(Env());
  delete Self;
}

void
Rect::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Rect",
    { InstanceAccessor("left", &Rect::GetLeft, &Rect::SetLeft),
      InstanceAccessor("bottom", &Rect::GetBottom, &Rect::SetBottom),
      InstanceAccessor("width", &Rect::GetWidth, &Rect::SetWidth),
      InstanceAccessor("height", &Rect::GetHeight, &Rect::SetHeight),
      InstanceMethod("intersect", &Rect::Intersect) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Rect", ctor);
}
void
Rect::Intersect(const CallbackInfo& info)
{
  if (info.Length() != 1) {
    throw Napi::Error::New(info.Env(),
                           "Intersect requires a single argument of type Rect");
  }
  auto rectObj = info[0].As<Object>();
  Rect* rectIntersect = Rect::Unwrap(rectObj);
  PdfRect rect = rectIntersect->GetRect();
  GetRect().Intersect(rect);
}
Napi::Value
Rect::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetRect().GetWidth());
}
Napi::Value
Rect::GetHeight(const CallbackInfo& info)
{

  return Napi::Number::New(info.Env(), GetRect().GetHeight());
}
Napi::Value
Rect::GetLeft(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetRect().GetLeft());
}
Napi::Value
Rect::GetBottom(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetRect().GetBottom());
}
void
Rect::SetWidth(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double width = value.As<Number>();
  GetRect().SetWidth(width);
}
void
Rect::SetHeight(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double height = value.As<Number>();
  GetRect().SetHeight(height);
}
void
Rect::SetLeft(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double left = value.As<Number>();
  GetRect().SetLeft(left);
}
void
Rect::SetBottom(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double bottom = value.As<Number>();
  GetRect().SetBottom(bottom);
}
}
