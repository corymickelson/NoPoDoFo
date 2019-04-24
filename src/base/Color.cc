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

#include "Color.h"
#include "../Defines.h"
#include "../ValidateArguments.h"
#include <optional/optional.hpp>
#include <spdlog/spdlog.h>

using namespace PoDoFo;
using namespace Napi;
using std::string;
using std::stringstream;
using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Color::Constructor;

void
Color::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Color",
    { InstanceMethod("isRGB", &Color::IsRGB),
      InstanceMethod("isCMYK", &Color::IsCMYK),
      InstanceMethod("isGreyScale", &Color::IsGreyScale),
      InstanceMethod("convertToRGB", &Color::ConvertToRGB),
      InstanceMethod("convertToCMYK", &Color::ConvertToCMYK),
      InstanceMethod("convertToGreyScale", &Color::ConvertToGreyScale),
      InstanceMethod("getRed", &Color::GetRed),
      InstanceMethod("getGreen", &Color::GetGreen),
      InstanceMethod("getBlue", &Color::GetBlue),
      InstanceMethod("getCyan", &Color::GetCyan),
      InstanceMethod("getMagenta", &Color::GetMagenta),
      InstanceMethod("getBlack", &Color::GetBlack),
      InstanceMethod("getGrey", &Color::GetGrey),
      InstanceMethod("getColorStreamString", &Color::GetColorStreamString) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Color", ctor);
}

Color::Color(const CallbackInfo& info)
  : ObjectWrap(info)
{
  vector<int> opts =
    AssertCallbackInfo(info,
                       { { 0,
                           { option(napi_number),
                             option(napi_object),
                             option(napi_external),
                             option(napi_string) } },
                         { 1, { nullopt, option(napi_number) } },
                         { 2, { nullopt, option(napi_number) } },
                         { 3, { nullopt, option(napi_number) } } });
  DbgLog = spdlog::get("DbgLog");
  if (opts[0] == 3) {
    const auto cs = info[0].As<String>().Utf8Value().c_str();
    Clr = new PdfColor(PdfColor::FromString(cs));
  } else if (opts[0] == 0 && opts[1] == 0) {
    Clr = new PdfColor(info[0].As<Number>().FloatValue());
  } else if (opts[0] == 1) {
    Clr = new PdfColor(*Color::Unwrap(info[0].As<Object>())->Clr);
  } else if (opts[0] == 2) {
    Clr = new PdfColor(*info[0].As<External<PdfColor>>().Data());
  } else if (opts[0] == 0 && opts[1] == 1 && opts[2] == 1 && opts[3] == 0) {
    Clr = new PdfColor(info[0].As<Number>().FloatValue(),
                         info[1].As<Number>().FloatValue(),
                         info[2].As<Number>().FloatValue());
  }

  else if (opts[0] == 0 && opts[1] == 1 && opts[2] == 1 && opts[3] == 1) {
    Clr = new PdfColor(info[0].As<Number>().FloatValue(),
                         info[1].As<Number>().FloatValue(),
                         info[2].As<Number>().FloatValue(),
                         info[3].As<Number>().FloatValue());
  } else {
    Error::New(info.Env(), "Color must be one of GreyScale, RGB, or CMYK")
      .ThrowAsJavaScriptException();
  }
}

Color::~Color()
{
  DbgLog->debug("Color Cleanup");
  HandleScope scope(Env());
  delete Clr;
}

JsValue
Color::IsRGB(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), Clr->IsRGB());
}

JsValue
Color::IsCMYK(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), Clr->IsCMYK());
}
JsValue
Color::IsGreyScale(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), Clr->IsGrayScale());
}
JsValue
Color::GetName(const CallbackInfo& info)
{
  return String::New(info.Env(), Clr->GetName());
}
JsValue
Color::GetDensity(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetDensity());
}
JsValue
Color::ConvertToGreyScale(const CallbackInfo& info)
{
  PdfColor greyScale = Clr->ConvertToGrayScale();
  return Color::Constructor.New(
    { Number::New(info.Env(), greyScale.GetGrayScale()) });
}
JsValue
Color::ConvertToRGB(const CallbackInfo& info)
{
  PdfColor rgb = Clr->ConvertToRGB();
  return Color::Constructor.New({ Number::New(info.Env(), rgb.GetRed()),
                                  Number::New(info.Env(), rgb.GetGreen()),
                                  Number::New(info.Env(), rgb.GetBlue()) });
}
JsValue
Color::ConvertToCMYK(const CallbackInfo& info)
{
  PdfColor cmyk = Clr->ConvertToCMYK();
  return Color::Constructor.New({ Number::New(info.Env(), cmyk.GetCyan()),
                                  Number::New(info.Env(), cmyk.GetMagenta()),
                                  Number::New(info.Env(), cmyk.GetYellow()),
                                  Number::New(info.Env(), cmyk.GetBlack()) });
}
JsValue
Color::GetGrey(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetGrayScale());
}
JsValue
Color::GetCyan(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetCyan());
}
JsValue
Color::GetMagenta(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetMagenta());
}
JsValue
Color::GetYellow(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetYellow());
}
JsValue
Color::GetBlack(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetBlack());
}
JsValue
Color::GetBlue(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetBlue());
}
JsValue
Color::GetRed(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetRed());
}
JsValue
Color::GetGreen(const CallbackInfo& info)
{
  return Number::New(info.Env(), Clr->GetGreen());
}
JsValue
Color::GetColorStreamString(const CallbackInfo& info)
{
  stringstream ss;
  PdfLocaleImbue(ss);
  if (Clr->IsCMYK()) {
    ss << Clr->GetCyan() << " " << Clr->GetMagenta() << " "
       << Clr->GetYellow() << " " << Clr->GetBlack() << " " << CMYK_OP;
  } else if (Clr->IsGrayScale()) {
    ss << Clr->GetGrayScale() << " " << GREY_OP;
  } else if (Clr->IsRGB()) {
    ss << Clr->GetRed() << " " << Clr->GetGreen() << " " << Clr->GetBlue()
       << " " << RGB_OP;
  } else {
    Error::New(info.Env(),
               "Color StringStream currently supports CMYK, RGB, and GreyScale")
      .ThrowAsJavaScriptException();
    return {};
  }
  return String::New(info.Env(), ss.str());
}
}
