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

#include "Painter.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Stream.h"
#include "Document.h"
#include "ExtGState.h"
#include "Font.h"
#include "Image.h"
#include "Page.h"
#include "Rect.h"

using namespace Napi;
using namespace PoDoFo;
namespace NoPoDoFo {
FunctionReference Painter::constructor;
Painter::Painter(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_object });
  auto o = info[0].As<Object>();
  if (!o.InstanceOf(Document::constructor.Value())) {
    throw Error::New(
      info.Env(), "Painter requires an instance of Document for construction");
  }
  document = Document::Unwrap(o);
  painter = new PdfPainter();
}

Painter::~Painter()
{
  Napi::HandleScope scope(Env());
  delete painter;
  document = nullptr;
}
void
Painter::Initialize(Napi::Env& env, Napi::Object& target)
{
  Napi::HandleScope scope(env);
  Napi::Function ctor = DefineClass(
    env,
    "Painter",
    { InstanceAccessor("page", &Painter::GetPage, &Painter::SetPage),
      InstanceAccessor(
        "tabWidth", &Painter::GetTabWidth, &Painter::SetTabWidth),
      InstanceAccessor(
        "precision", &Painter::GetPrecision, &Painter::SetPrecision),
      InstanceAccessor("canvas", &Painter::GetCanvas, nullptr),
      InstanceAccessor("font", &Painter::GetFont, &Painter::SetFont),
      InstanceMethod("finishPage", &Painter::FinishPage),
      InstanceMethod("setColor", &Painter::SetColor),
      InstanceMethod("setStrokeWidth", &Painter::SetStrokeWidth),
      InstanceMethod("setGrey", &Painter::SetGrey),
      InstanceMethod("setStrokingGrey", &Painter::SetStrokingGrey),
      InstanceMethod("setColorCMYK", &Painter::SetColorCMYK),
      InstanceMethod("setStrokingColorCMYK", &Painter::SetStrokingColorCMYK),
      InstanceMethod("setStrokeStyle", &Painter::SetStrokeStyle),
      InstanceMethod("setLineCapStyle", &Painter::SetLineCapStyle),
      InstanceMethod("setLineJoinStyle", &Painter::SetLineJoinStyle),
      InstanceMethod("setClipRect", &Painter::SetClipRect),
      InstanceMethod("setMiterLimit", &Painter::SetMiterLimit),
      InstanceMethod("rectangle", &Painter::Rectangle),
      InstanceMethod("ellipse", &Painter::Ellipse),
      InstanceMethod("circle", &Painter::Circle),
      InstanceMethod("closePath", &Painter::ClosePath),
      InstanceMethod("lineTo", &Painter::LineTo),
      InstanceMethod("moveTo", &Painter::MoveTo),
      InstanceMethod("cubicBezierTo", &Painter::CubicBezierTo),
      InstanceMethod("horizontalLineTo", &Painter::HorizontalLineTo),
      InstanceMethod("verticalLineTo", &Painter::VerticalLineTo),
      InstanceMethod("smoothCurveTo", &Painter::SmoothCurveTo),
      InstanceMethod("quadCurveTo", &Painter::QuadCurveTo),
      InstanceMethod("arcTo", &Painter::ArcTo),
      InstanceMethod("close", &Painter::Close),
      InstanceMethod("stroke", &Painter::Stroke),
      InstanceMethod("fillAndStroke", &Painter::FillAndStroke),
      InstanceMethod("fill", &Painter::Fill),
      InstanceMethod("endPath", &Painter::EndPath),
      InstanceMethod("clip", &Painter::Clip),
      InstanceMethod("save", &Painter::Save),
      InstanceMethod("restore", &Painter::Restore),
      InstanceMethod("setExtGState", &Painter::SetExtGState),
      InstanceMethod("getCurrentPath", &Painter::GetCurrentPath),
      InstanceMethod("drawGlyph", &Painter::DrawGlyph),
      InstanceMethod("moveTextPosition", &Painter::MoveTextPosition),
      InstanceMethod("addText", &Painter::AddText),
      InstanceMethod("endText", &Painter::EndText),
      InstanceMethod("beginText", &Painter::BeginText),
      InstanceMethod("getMultiLineText", &Painter::GetMultiLineText),
      InstanceMethod("drawTextAligned", &Painter::DrawTextAligned),
      InstanceMethod("drawLine", &Painter::DrawLine),
      InstanceMethod("drawMultiLineText", &Painter::DrawMultiLineText),
      InstanceMethod("drawText", &Painter::DrawText),
      InstanceMethod("drawImage", &Painter::DrawImage) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Painter", ctor);
}
void
Painter::SetPage(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsObject()) {
    throw Napi::Error::New(info.Env(), "Page must be an instance of Page.");
  }
  auto pageObj = value.As<Object>();
  Page* pagePtr = Page::Unwrap(pageObj);
  if (!pageObj.InstanceOf(Page::constructor.Value())) {
    throw Napi::Error::New(info.Env(), "Page must be an instance of Page.");
  }
  PoDoFo::PdfPage* page = pagePtr->GetPage();
  //  document = pagePtr->GetDocument();
  painter->SetPage(page);
  pageSize = page->GetPageSize();
}
Napi::Value
Painter::GetPage(const CallbackInfo& info)
{
  auto* page = dynamic_cast<PdfPage*>(painter->GetPage());
  auto pagePtr = Napi::External<PdfPage>::New(info.Env(), page);
  auto docPtr =
    Napi::External<PdfMemDocument>::New(info.Env(), document->GetDocument());
  auto instance = Page::constructor.New({ pagePtr, docPtr });
  return instance;
}

void
Painter::SetColor(const CallbackInfo& info)
{
  if (info[0].IsArray()) {
    auto jsValue = info[0].As<Array>();
    int rgb[3];
    GetRGB(jsValue, rgb);
    painter->SetColor(rgb[0], rgb[1], rgb[2]);
  } else {
    throw Napi::TypeError::New(info.Env(),
                               "Requires RGB color: [Number, Number, Number]");
  }
}

void
Painter::SetColorCMYK(const CallbackInfo& info)
{
  if (info.Length() < 1 || !info[0].IsArray()) {
    throw TypeError::New(
      info.Env(), "Requires CMYK color: [number, number, number, number]");
  }
  auto js = info[0].As<Array>();
  int cmyk[4];
  GetCMYK(js, cmyk);
  painter->SetColorCMYK(cmyk[0], cmyk[1], cmyk[2], cmyk[3]);
}

Napi::Value
Painter::GetCanvas(const CallbackInfo& info)
{
  PdfStream* instance = painter->GetCanvas();
  if (!instance) {
    return info.Env().Null();
  }
  return Stream::constructor.New(
    { External<PdfStream>::New(info.Env(), instance) });
}

void
Painter::SetStrokingGrey(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  double value = info[0].As<Number>();
  if (value < 0.0 || value > 1.0) {
    throw Error::New(info.Env(), "value must be between 0.0 and 1.0");
  }
  painter->SetStrokingGray(value);
}

void
Painter::SetGrey(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  double value = info[0].As<Number>();
  if (value < 0.0 || value > 1.0) {
    throw Error::New(info.Env(), "value must be between 0.0 and 1.0");
  }
  painter->SetGray(value);
}

void
Painter::SetStrokingColorCMYK(const CallbackInfo& info)
{
  if (info.Length() < 1 || !info[0].IsArray()) {
    throw TypeError::New(
      info.Env(), "Requires CMYK color: [number, number, number, number]");
  }
  auto js = info[0].As<Array>();
  int cmyk[4];
  GetCMYK(js, cmyk);
  painter->SetStrokingColorCMYK(cmyk[0], cmyk[1], cmyk[2], cmyk[3]);
}

void
Painter::SetStrokeWidth(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  double value = info[0].As<Number>();
  painter->SetStrokeWidth(value);
}

void
Painter::FinishPage(const CallbackInfo& info)
{
  try {
    painter->FinishPage();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
void
Painter::DrawText(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_object, napi_valuetype::napi_string });
  double x, y;
  string text = info[1].As<String>().Utf8Value();
  auto d = info[0].As<Object>();
  x = d.Get("x").As<Number>();
  y = d.Get("y").As<Number>();
  try {
    painter->DrawText(x, y, text.c_str());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
void
Painter::DrawMultiLineText(const CallbackInfo& info)
{
// For some reason windows builds break with unresolved external symbol on
// podofo's DrawMultiLineText method
#ifndef WIN32

  AssertFunctionArgs(info,
                     6,
                     { napi_object,
                       napi_string,
                       napi_number,
                       napi_number,
                       napi_boolean,
                       napi_boolean });
  PdfRect rect = *Rect::Unwrap(info[0].As<Object>())->GetRect();
  string text = info[1].As<String>().Utf8Value();
  EPdfAlignment alignment =
    static_cast<EPdfAlignment>(info[2].As<Number>().Int32Value());
  EPdfVerticalAlignment verticalAlignment =
    static_cast<EPdfVerticalAlignment>(info[3].As<Number>().Int32Value());
  bool clip = info[4].As<Boolean>();
  bool skipSpaces = info[5].As<Boolean>();
  try {
    painter->DrawMultiLineText(rect.GetBottom(),
                               rect.GetLeft(),
                               rect.GetWidth(),
                               rect.GetHeight(),
                               PdfString(text),
                               alignment,
                               verticalAlignment);

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }

#endif
}
void
Painter::DrawImage(const CallbackInfo& info)
{
  try {
    if (info.Length() < 3) {
      throw Napi::Error::New(
        info.Env(),
        "DrawImage requires a minimum of three parameters: Image, x, y");
    }
    // Image
    auto imgObj = info[0].As<Object>();
    Image* imgInstance = Image::Unwrap(imgObj);
    PdfImage img = imgInstance->GetImage();

    // Coordinates
    double x, y;
    if (!info[1].IsNumber() || !info[2].IsNumber()) {
      throw Napi::Error::New(info.Env(), "coorindates must be of type number");
    }
    x = info[1].As<Number>().DoubleValue();
    y = info[2].As<Number>().DoubleValue();

    // Scaling
    double width = 0.0, height = 0.0;
    if (info.Length() == 5) {
      if (!info[3].IsNumber() || !info[4].IsNumber()) {
        throw Napi::Error::New(info.Env(),
                               "scaling width & height must be of type number");
      }
      width = info[3].As<Number>().DoubleValue();
      height = info[4].As<Number>().DoubleValue();
    }
    if (width != 0.0 && height != 0.0)
      painter->DrawImage(x, y, &img, width, height);
    else
      painter->DrawImage(x, y, &img);

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Painter::GetPrecision(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           static_cast<double>(painter->GetPrecision()));
}
void
Painter::SetPrecision(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Precision must be of type number");
  }
  unsigned short p =
    static_cast<unsigned short>(value.As<Number>().Uint32Value());
  painter->SetPrecision(p);
}

void
Painter::SetStrokeStyle(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_number, napi_valuetype::napi_number });
  int styleIndex = info[0].As<Number>();
  auto scale = info[1].As<Number>();
  painter->SetStrokeStyle(
    static_cast<EPdfStrokeStyle>(styleIndex), nullptr, false, scale, false);
}
void
Painter::SetLineCapStyle(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  painter->SetLineCapStyle(
    static_cast<EPdfLineCapStyle>(info[0].As<Number>().Int32Value()));
}
void
Painter::SetLineJoinStyle(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  painter->SetLineJoinStyle(
    static_cast<EPdfLineJoinStyle>(info[0].As<Number>().Int32Value()));
}
void
Painter::SetFont(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  Font* font = Font::Unwrap(value.As<Object>());
  try {
    painter->SetFont(font->GetPoDoFoFont());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Painter::GetFont(const Napi::CallbackInfo& info)
{
  return Font::constructor.New(
    { External<PdfFont>::New(info.Env(), painter->GetFont()) });
}
void
Painter::SetClipRect(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  Rect* r = Rect::Unwrap(info[0].As<Object>());
  painter->SetClipRect(*r->GetRect());
}

void
Painter::SetMiterLimit(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  double limit = info[0].As<Number>();
  painter->SetMiterLimit(limit);
}

void
Painter::Rectangle(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  Rect* r = Rect::Unwrap(info[0].As<Object>());
  try {
    painter->Rectangle(*r->GetRect());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Ellipse(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto o = info[0].As<Object>();
  double x, y, width, height;
  x = o.Get("x").As<Number>();
  y = o.Get("y").As<Number>();
  width = o.Get("width").As<Number>();
  height = o.Get("height").As<Number>();
  try {
    painter->Ellipse(x, y, width, height);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Circle(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto o = info[0].As<Object>();
  double x, y, radius;
  x = o.Get("x").As<Number>();
  y = o.Get("y").As<Number>();
  radius = o.Get("radius").As<Number>();
  painter->Circle(x, y, radius);
}

void
Painter::ClosePath(const CallbackInfo& info)
{
  try {
    painter->ClosePath();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::LineTo(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto o = info[0].As<Object>();
  double x, y;
  x = o.Get("x").As<Number>();
  y = o.Get("y").As<Number>();
  try {
    painter->LineTo(x, y);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::MoveTo(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  double x, y;
  auto o = info[0].As<Object>();
  x = o.Get("x").As<Number>();
  y = o.Get("y").As<Number>();
  try {
    painter->MoveTo(x, y);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::CubicBezierTo(const CallbackInfo& info)
{
  AssertFunctionArgs(info,
                     3,
                     { napi_valuetype::napi_object,
                       napi_valuetype::napi_object,
                       napi_valuetype::napi_object });
  auto d1 = info[0].As<Object>();
  auto d2 = info[1].As<Object>();
  auto d3 = info[2].As<Object>();
  int d1x, d1y, d2x, d2y, d3x, d3y;
  d1x = d1.Get("x").As<Number>();
  d1y = d1.Get("y").As<Number>();
  d2x = d2.Get("x").As<Number>();
  d2y = d2.Get("y").As<Number>();
  d3x = d3.Get("x").As<Number>();
  d3y = d3.Get("y").As<Number>();
  try {
    painter->CubicBezierTo(d1x, d1y, d2x, d2y, d3x, d3y);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::HorizontalLineTo(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  double value = info[0].As<Number>();
  try {
    painter->HorizontalLineTo(value);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::VerticalLineTo(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  double value = info[0].As<Number>();
  try {
    painter->VerticalLineTo(value);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::SmoothCurveTo(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_object, napi_valuetype::napi_object });
  auto d1 = info[0].As<Object>();
  auto d2 = info[1].As<Object>();
  double d1x, d1y, d2x, d2y;
  d1x = d1.Get("x").As<Number>();
  d1y = d1.Get("y").As<Number>();
  d2x = d2.Get("x").As<Number>();
  d2y = d2.Get("y").As<Number>();
  try {
    painter->SmoothCurveTo(d1x, d1y, d2x, d2y);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::QuadCurveTo(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_object, napi_valuetype::napi_object });
  auto d1 = info[0].As<Object>();
  auto d2 = info[1].As<Object>();
  int d1x, d1y, d2x, d2y;
  d1x = d1.Get("x").As<Number>();
  d1y = d1.Get("y").As<Number>();
  d2x = d2.Get("x").As<Number>();
  d2y = d2.Get("y").As<Number>();
  try {
    painter->QuadCurveTo(d1x, d1y, d2x, d2y);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::ArcTo(const CallbackInfo& info)
{
  AssertFunctionArgs(info,
                     5,
                     {
                       napi_valuetype::napi_object,
                       napi_valuetype::napi_object,
                       napi_valuetype::napi_number,
                       napi_valuetype::napi_boolean,
                       napi_valuetype::napi_boolean,
                     });
  auto point1 = info[0].As<Object>();
  auto point2 = info[1].As<Object>();
  double rotation = info[2].As<Number>();
  bool large = info[3].As<Boolean>();
  bool sweep = info[4].As<Boolean>();
  double p1x, p1y, p2x, p2y;
  p1x = point1.Get("x").As<Number>();
  p1y = point1.Get("y").As<Number>();
  p2x = point2.Get("x").As<Number>();
  p2y = point2.Get("y").As<Number>();
  painter->ArcTo(p1x, p1y, p2x, p2y, rotation, large, sweep);
}

void
Painter::Close(const CallbackInfo& info)
{
  try {
    painter->Close();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Stroke(const CallbackInfo& info)
{
  try {
    painter->Stroke();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::FillAndStroke(const CallbackInfo& info)
{
  try {
    painter->FillAndStroke();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Fill(const CallbackInfo& info)
{
  try {
    painter->Fill();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::EndPath(const CallbackInfo& info)
{
  try {
    painter->EndPath();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Clip(const CallbackInfo& info)
{
  try {
    painter->Clip();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Save(const CallbackInfo& info)
{
  try {
    painter->Save();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Restore(const CallbackInfo& info)
{
  try {
    painter->Restore();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::SetExtGState(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto wrap = info[0].As<Object>();
  if (!wrap.InstanceOf(ExtGState::constructor.Value())) {
    throw TypeError::New(info.Env(), "must be of type ExtGState");
  }
  ExtGState* state = ExtGState::Unwrap(wrap);
  try {
    painter->SetExtGState(state->GetExtGState());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::SetTabWidth(const CallbackInfo& info, const Napi::Value& value)
{
  int n = value.As<Number>();
  try {
    painter->SetTabWidth(static_cast<unsigned short>(n));
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Painter::GetTabWidth(const CallbackInfo& info)
{
  return Number::New(info.Env(), painter->GetTabWidth());
}

Napi::Value
Painter::GetCurrentPath(const CallbackInfo& info)
{
  return String::New(info.Env(), painter->GetCurrentPath().str());
}

void
Painter::DrawLine(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_object, napi_valuetype::napi_object });
  auto start = info[0].As<Object>();
  auto end = info[1].As<Object>();
  double startX, startY, endX, endY;
  startX = start.Get("x").As<Number>().DoubleValue();
  startY = start.Get("y").As<Number>().DoubleValue();
  endX = end.Get("x").As<Number>().DoubleValue();
  endY = end.Get("y").As<Number>().DoubleValue();
  painter->DrawLine(startX, startY, endX, endY);
}

void
Painter::DrawTextAligned(const CallbackInfo& info)
{
  AssertFunctionArgs(info,
                     3,
                     { napi_valuetype::napi_object,
                       napi_valuetype::napi_string,
                       napi_valuetype::napi_number });
  double x, y, width;
  int alignmentIndex = info[2].As<Number>();
  string text = info[1].As<String>().Utf8Value();
  auto alignment = static_cast<EPdfAlignment>(alignmentIndex);
  auto o = info[0].As<Object>();
  x = o.Get("x").As<Number>();
  y = o.Get("y").As<Number>();
  width = o.Get("width").As<Number>();
  try {
    painter->DrawTextAligned(x, y, width, PdfString(text), alignment);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Painter::GetMultiLineText(const CallbackInfo& info)
{
  return info.Env().Undefined();
  /*AssertFunctionArgs(info,
                     3,
                     { napi_valuetype::napi_number,
                       napi_valuetype::napi_string,
                       napi_valuetype::napi_boolean });
  double width = info[0].As<Number>();
  string text = info[1].As<String>().Utf8Value();
  bool skipSpaces = info[2].As<Boolean>();
  vector<PdfString> lines =
#if PODOFO_VERSION_PATCH >= 6
    painter->GetMultiLineTextAsLines(width, PdfString(text), skipSpaces);
#else
    painter->GetMultiLineTextAsLines(width, PdfString(text));
#endif

  auto js = Array::New(info.Env());
  uint32_t count = 0;
  for (auto& i : lines) {
    js.Set(count, i.GetStringUtf8());
    count++;
  }
  return js;*/
}

void
Painter::BeginText(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto point = info[0].As<Object>();
  double x, y;
  x = point.Get("x").As<Number>();
  y = point.Get("y").As<Number>();
  try {
    painter->BeginText(x, y);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::EndText(const CallbackInfo& info)
{
  try {
    painter->EndText();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::AddText(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  try {
    painter->AddText(PdfString(info[0].As<String>().Utf8Value()));
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::MoveTextPosition(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto point = info[0].As<Object>();
  double x, y;
  x = point.Get("x").As<Number>();
  y = point.Get("y").As<Number>();
  try {
    painter->MoveTextPos(x, y);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::DrawGlyph(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_object, napi_valuetype::napi_string });
  auto point = info[0].As<Object>();
  string glyph = info[1].As<String>().Utf8Value();
  double x, y;
  x = point.Get("x").As<Number>();
  y = point.Get("y").As<Number>();
  try {
    painter->DrawGlyph(document->GetDocument(), x, y, glyph.c_str());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::GetCMYK(Napi::Value& value, int* cmyk)
{
  auto js = value.As<Array>();
  for (uint8_t i = 0; i < js.Length(); i++) {
    cmyk[i] = js.Get(i).As<Number>();
  }
}

void
Painter::GetRGB(Napi::Value& value, int* rgb)
{
  auto js = value.As<Array>();
  for (uint8_t i = 0; i < js.Length(); i++) {
    rgb[i] = js.Get(i).As<Number>();
  }
}
}
