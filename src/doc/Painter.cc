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
#include "../base/Color.h"
#include "../base/Stream.h"
#include "../base/XObject.h"
#include "Document.h"
#include "ExtGState.h"
#include "Font.h"
#include "Image.h"
#include "Page.h"
#include "Rect.h"
#include "StreamDocument.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::make_unique;
using std::string;

namespace NoPoDoFo {

FunctionReference Painter::constructor; // NOLINT

Painter::Painter(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  dbglog = spdlog::get("DbgLog");
  auto o = info[0].As<Object>();
  if (o.InstanceOf(Document::Constructor.Value())) {
    isMemDoc = true;
    document = Document::Unwrap(o)->Base;
  } else if (o.InstanceOf(StreamDocument::constructor.Value())) {
    isMemDoc = false;
    document = StreamDocument::Unwrap(o)->Base;
  } else {
    TypeError::New(info.Env(), "requires an instance of BaseDocument")
      .ThrowAsJavaScriptException();
    return;
  }
  painter = make_unique<PdfPainter>();
}

Painter::~Painter()
{
  dbglog->debug("Painter Cleanup");
  HandleScope scope(Env());
  document = nullptr;
}

void
Painter::Initialize(Napi::Env& env, Napi::Object& target)
{
  Napi::HandleScope scope(env);
  Napi::Function ctor = DefineClass(
    env,
    "Painter",
    { InstanceAccessor(
        "tabWidth", &Painter::GetTabWidth, &Painter::SetTabWidth),
      InstanceAccessor(
        "precision", &Painter::GetPrecision, &Painter::SetPrecision),
      InstanceAccessor("canvas", &Painter::GetCanvas, nullptr),
      InstanceAccessor("font", &Painter::GetFont, &Painter::SetFont),
      InstanceMethod("setPage", &Painter::SetPage),
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
Painter::SetPage(const Napi::CallbackInfo& info)
{
  if (!info[0].IsObject()) {
    throw Napi::Error::New(info.Env(), "args[0] must be an instance of Page.");
  }
  if (info[0].As<Object>().InstanceOf(Page::constructor.Value())) {
    auto canvas = Page::Unwrap(info[0].As<Object>());
    painter->SetPage(&canvas->page);
  } else if (info[0].As<Object>().InstanceOf(XObject::Constructor.Value())) {
    auto canvas = &XObject::Unwrap(info[0].As<Object>())->GetXObject();
    painter->SetPage(canvas);
  } else {
    TypeError::New(info.Env(),
                   "Painter must be an instance of PdfCanvas: XObject or Page")
      .ThrowAsJavaScriptException();
    return;
  }
}

void
Painter::SetColor(const CallbackInfo& info)
{
  painter->SetColor(*Color::Unwrap(info[0].As<Object>())->Self);
}

void
Painter::SetColorCMYK(const CallbackInfo& info)
{
  if (info.Length() < 1 || !info[0].IsArray()) {
    throw TypeError::New(
      info.Env(), "Requires CMYK color: [number, number, number, number]");
  }
  auto js = info[0].As<Array>();
  float cmyk[4];
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
  float value = info[0].As<Number>();
  if (value < 0.0 || value > 1.0) {
    throw Error::New(info.Env(), "value must be between 0.0 and 1.0");
  }
  painter->SetStrokingGray(value);
}

void
Painter::SetGrey(const CallbackInfo& info)
{
  float value = info[0].As<Number>();
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
  float cmyk[4];
  GetCMYK(js, cmyk);
  painter->SetStrokingColorCMYK(cmyk[0], cmyk[1], cmyk[2], cmyk[3]);
}

void
Painter::SetStrokeWidth(const Napi::CallbackInfo& info)
{
  float value = info[0].As<Number>();
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
  float x, y;
  string text = info[1].As<String>().Utf8Value();
  if (text.empty()) {
    Error::New(
      info.Env(),
      "Text value provided was empty, Painter can not draw empty text.")
      .ThrowAsJavaScriptException();
    return;
  }
  auto d = info[0].As<Object>();
  x = d.Get("x").As<Number>();
  y = d.Get("y").As<Number>();
  try {
    painter->DrawText(
      x, y, PdfString(reinterpret_cast<const pdf_utf8*>(text.c_str())));
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
void
Painter::DrawMultiLineText(const CallbackInfo& info)
{
  string text = info[1].As<String>().Utf8Value();
  if (text.empty()) {
    Error::New(
      info.Env(),
      "Text value provided was empty, Painter can not draw empty text.")
      .ThrowAsJavaScriptException();
    return;
  }
  PdfRect rect = Rect::Unwrap(info[0].As<Object>())->GetRect();
  EPdfAlignment alignment = ePdfAlignment_Center;
  if (info.Length() >= 3 && info[2].IsNumber()) {
    alignment = static_cast<EPdfAlignment>(info[2].As<Number>().Int32Value());
  }
  EPdfVerticalAlignment verticalAlignment = ePdfVerticalAlignment_Center;
  if (info.Length() >= 4 && info[3].IsNumber()) {
    verticalAlignment =
      static_cast<EPdfVerticalAlignment>(info[3].As<Number>().Int32Value());
  }
  try {
    painter->DrawMultiLineText(
      rect,
      PdfString(reinterpret_cast<const pdf_utf8*>(text.c_str())),
      alignment,
      verticalAlignment);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
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
    double width = 1.0, height = 1.0;
    if (info.Length() >= 4 && info[3].IsObject()) {
      auto optObj = info[3].As<Object>();
      if (optObj.HasOwnProperty("width") && optObj.HasOwnProperty("height")) {
        auto w = optObj.Get("width").As<Number>().DoubleValue();
        auto h = optObj.Get("height").As<Number>().DoubleValue();
        width = w / img.GetWidth();
        height = h / img.GetHeight();
      } else if (optObj.HasOwnProperty("scaleX") &&
                 optObj.HasOwnProperty("scaleY")) {
        width = optObj.Get("scaleX").As<Number>().DoubleValue();
        height = optObj.Get("scaleY").As<Number>().DoubleValue();
      } else {
        TypeError::New(info.Env(),
                       "Options include either defining the width and height "
                       "(in pdf points) or a scaling factor")
          .ThrowAsJavaScriptException();
      }
    }
    painter->DrawImage(x, y, &img, width, height);
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
  int styleIndex = info[0].As<Number>();
  auto scale = info[1].As<Number>();
  painter->SetStrokeStyle(
    static_cast<EPdfStrokeStyle>(styleIndex), nullptr, false, scale, false);
}
void
Painter::SetLineCapStyle(const Napi::CallbackInfo& info)
{
  painter->SetLineCapStyle(
    static_cast<EPdfLineCapStyle>(info[0].As<Number>().Int32Value()));
}
void
Painter::SetLineJoinStyle(const Napi::CallbackInfo& info)
{
  painter->SetLineJoinStyle(
    static_cast<EPdfLineJoinStyle>(info[0].As<Number>().Int32Value()));
}
void
Painter::SetFont(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  Font* font = Font::Unwrap(value.As<Object>());
  try {
    painter->SetFont(&font->GetFont());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Painter::GetFont(const Napi::CallbackInfo& info)
{
  if (!painter->GetFont()) {
    return info.Env().Null();
  }
  return Font::Constructor.New(
    { External<PdfFont>::New(info.Env(), painter->GetFont()) });
}
void
Painter::SetClipRect(const Napi::CallbackInfo& info)
{
  Rect* r = Rect::Unwrap(info[0].As<Object>());
  painter->SetClipRect(r->GetRect());
}

void
Painter::SetMiterLimit(const CallbackInfo& info)
{
  float limit = info[0].As<Number>();
  painter->SetMiterLimit(limit);
}

void
Painter::Rectangle(const CallbackInfo& info)
{
  Rect* r = Rect::Unwrap(info[0].As<Object>());
  try {
    painter->Rectangle(r->GetRect());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::Ellipse(const CallbackInfo& info)
{
  auto o = info[0].As<Object>();
  float x, y, width, height;
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
  auto o = info[0].As<Object>();
  float x, y, radius;
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
  auto o = info[0].As<Object>();
  float x, y;
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
  float x, y;
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
  float value = info[0].As<Number>();
  try {
    painter->HorizontalLineTo(value);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::VerticalLineTo(const CallbackInfo& info)
{
  float value = info[0].As<Number>();
  try {
    painter->VerticalLineTo(value);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::SmoothCurveTo(const CallbackInfo& info)
{
  auto d1 = info[0].As<Object>();
  auto d2 = info[1].As<Object>();
  float d1x, d1y, d2x, d2y;
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
  auto point1 = info[0].As<Object>();
  auto point2 = info[1].As<Object>();
  float rotation = info[2].As<Number>();
  bool large = info[3].As<Boolean>();
  bool sweep = info[4].As<Boolean>();
  float p1x, p1y, p2x, p2y;
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
  auto wrap = info[0].As<Object>();
  if (!wrap.InstanceOf(ExtGState::Constructor.Value())) {
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
  double x, y, width;
  int alignmentIndex = info[2].As<Number>();
  string text = info[1].As<String>().Utf8Value();
  if (text.empty()) {
    Error::New(
      info.Env(),
      "Text value provided was empty, Painter can not draw empty text.")
      .ThrowAsJavaScriptException();
    return;
  }
  auto alignment = static_cast<EPdfAlignment>(alignmentIndex);
  auto o = info[0].As<Object>();
  x = o.Get("x").As<Number>();
  y = o.Get("y").As<Number>();
  width = o.Get("width").As<Number>();
  try {
    painter->DrawTextAligned(
      x,
      y,
      width,
      PdfString(reinterpret_cast<const pdf_utf8*>(text.c_str())),
      alignment);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Painter::GetMultiLineText(const CallbackInfo& info)
{
  double width = info[0].As<Number>();
  string text = info[1].As<String>().Utf8Value();
  bool skipSpaces = info[2].As<Boolean>();
  vector<PdfString> lines =
    painter->GetMultiLineTextAsLines(width, PdfString(text), skipSpaces);
  auto js = Array::New(info.Env());
  uint32_t count = 0;
  for (auto& i : lines) {
    js.Set(count, i.GetStringUtf8());
    count++;
  }
  return js;
}

void
Painter::BeginText(const CallbackInfo& info)
{
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
  try {
    painter->AddText(PdfString(reinterpret_cast<const pdf_utf8*>(
      info[0].As<String>().Utf8Value().c_str())));
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::MoveTextPosition(const CallbackInfo& info)
{
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
  if (!isMemDoc) {
    TypeError::New(info.Env(), "Requires instance of PdfMemDocument")
      .ThrowAsJavaScriptException();
    return;
  }
  auto sharedMemDoc = static_cast<PdfMemDocument*>(document);
  auto point = info[0].As<Object>();
  string glyph = info[1].As<String>().Utf8Value();
  double x, y;
  x = point.Get("x").As<Number>();
  y = point.Get("y").As<Number>();
  try {
    painter->DrawGlyph(sharedMemDoc, x, y, glyph.c_str());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Painter::GetCMYK(Napi::Value& value, float* cmyk)
{
  auto js = value.As<Array>();
  for (uint8_t i = 0; i < js.Length(); i++) {
    cmyk[i] = js.Get(i).As<Number>().FloatValue();
  }
}

void
Painter::GetRGB(Napi::Value& value, float* rgb)
{
  auto js = value.As<Array>();
  for (uint8_t i = 0; i < js.Length(); i++) {
    rgb[i] = js.Get(i).As<Number>().FloatValue();
  }
}
}
