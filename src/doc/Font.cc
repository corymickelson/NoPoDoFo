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

#include "Font.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Stream.h"
#include "Encoding.h"

using namespace PoDoFo;

namespace NoPoDoFo {
FunctionReference Font::constructor;

Font::Font(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_external });
  font = info[0].As<External<PdfFont>>().Data();
}

void
Font::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Font",
    { InstanceAccessor("size", &Font::GetFontSize, &Font::SetFontSize),
      InstanceAccessor("scale", &Font::GetFontScale, &Font::SetFontScale),
      InstanceAccessor(
        "charSpace", &Font::GetFontCharSpace, &Font::SetFontCharSpace),
      InstanceAccessor("wordSpace", &Font::GetWordSpace, &Font::SetWordSpace),
      InstanceAccessor("underline", &Font::IsUnderlined, &Font::SetUnderline),
      InstanceAccessor("strikeOut", &Font::IsStrikeOut, &Font::SetStrikeOut),
      InstanceMethod("isBold", &Font::IsBold),
      InstanceMethod("isItalic", &Font::IsItalic),
      InstanceMethod("getIdentifier", &Font::GetIdentifier),
      InstanceMethod("getMetrics", &Font::GetFontMetric),
      InstanceMethod("getEncoding", &Font::GetEncoding),
      InstanceMethod("write", &Font::WriteToStream),
      InstanceMethod("embed", &Font::EmbedFont),
      InstanceMethod("stringWidth", &Font::StringWidth) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Font", ctor);
}
Napi::Value
Font::GetFontSize(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<double>(font->GetFontSize()));
}
void
Font::SetFontSize(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Error::New(info.Env(), "font size must be number");
  }
  font->SetFontSize(value.As<Number>().FloatValue());
}
Napi::Value
Font::GetFontScale(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<double>(font->GetFontScale()));
}
void
Font::SetFontScale(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Error::New(info.Env(), "scale must be a number");
  }
  font->SetFontScale(value.As<Number>().FloatValue());
}
Napi::Value
Font::GetFontCharSpace(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<double>(font->GetFontCharSpace()));
}
void
Font::SetFontCharSpace(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Error::New(info.Env(), "char space must be a number");
  }
  font->SetFontCharSpace(value.As<Number>().FloatValue());
}
Napi::Value
Font::GetWordSpace(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<double>(font->GetWordSpace()));
}
void
Font::SetWordSpace(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Error::New(info.Env(), "word space must be a number");
  }
  font->SetWordSpace(value.As<Number>().FloatValue());
}
Napi::Value
Font::IsUnderlined(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), font->IsUnderlined());
}
void
Font::SetUnderline(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Error::New(info.Env(), "underlined must be of type boolean");
  }
  font->SetUnderlined(value.As<Boolean>());
}
Napi::Value
Font::IsStrikeOut(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), font->IsStrikeOut());
}
void
Font::SetStrikeOut(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Error::New(info.Env(), "strike out must be of type boolean");
  }

  font->SetStrikeOut(value.As<Boolean>());
}
Napi::Value
Font::GetIdentifier(const Napi::CallbackInfo& info)
{
  return String::New(info.Env(), font->GetIdentifier().GetName());
}
Napi::Value
Font::GetEncoding(const Napi::CallbackInfo& info)
{
  const PdfEncoding* encoding = font->GetEncoding();
  return Encoding::constructor.New({ External<PdfEncoding>::New(
    info.Env(), const_cast<PdfEncoding*>(encoding)) });
}
Napi::Value
Font::GetFontMetric(const Napi::CallbackInfo& info)
{
  auto obj = Object::New(info.Env());
  const PdfFontMetrics* metrics = font->GetFontMetrics();
  obj.Set("lineSpacing", Number::New(info.Env(), metrics->GetLineSpacing()));
  obj.Set("underlineThickness",
          Number::New(info.Env(), metrics->GetUnderlineThickness()));
  obj.Set("underlinePosition",
          Number::New(info.Env(), metrics->GetUnderlinePosition()));
  obj.Set("strikeOutPosition",
          Number::New(info.Env(), metrics->GetStrikeOutPosition()));
  obj.Set("strikeOutThickness",
          Number::New(info.Env(), metrics->GetStrikeoutThickness()));
  obj.Set("fileName", String::New(info.Env(), metrics->GetFilename()));
  //  obj.Set("fontData", String::New(info.Env(), metrics->GetFontData()));
  obj.Set("fontName", String::New(info.Env(), metrics->GetFontname()));
  obj.Set("fontWeight", Number::New(info.Env(), metrics->GetWeight()));
  obj.Set("fontSize",
          Number::New(info.Env(), static_cast<double>(metrics->GetFontSize())));
  obj.Set(
    "fontScale",
    Number::New(info.Env(), static_cast<double>(metrics->GetFontScale())));
  obj.Set(
    "charSpace",
    Number::New(info.Env(), static_cast<double>(metrics->GetFontCharSpace())));
  obj.Set(
    "wordSpace",
    Number::New(info.Env(), static_cast<double>(metrics->GetWordSpace())));
  //    obj.Set("fontType", metrics->GetFontType());
  return obj;
}
Napi::Value
Font::IsBold(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), font->IsBold());
}
Napi::Value
Font::IsItalic(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), font->IsItalic());
}

Napi::Value
Font::StringWidth(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string text = info[0].As<String>().Utf8Value();
  return Number::New(info.Env(), font->GetFontMetrics()->StringWidth(text));
}
void
Font::WriteToStream(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_object });
  string content = info[0].As<String>().Utf8Value();
  auto streamWrap = info[1].As<Object>();
  if (!streamWrap.InstanceOf(Stream::constructor.Value())) {
    throw Error::New(info.Env(), "not an instance of NoPoDoFo::Stream");
  }
  Stream* stream = Stream::Unwrap(streamWrap);
  try {
    font->WriteStringToStream(PdfString(content), stream->GetStream());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
void
Font::EmbedFont(const Napi::CallbackInfo& info)
{
  try {
    font->EmbedFont();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Font::~Font()
{
  Napi::HandleScope scope(Env());
  delete font;
  //  font = nullptr;
}
}
