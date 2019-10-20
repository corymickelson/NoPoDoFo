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

#ifndef NPDF_PAINTER_H
#define NPDF_PAINTER_H

#define CONVERSION_CONSTANT 0.002834645669291339

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Painter : public Napi::ObjectWrap<Painter>
{
public:
  explicit Painter(const Napi::CallbackInfo& callbackInfo);
  explicit Painter(const Painter&) = delete;
  const Painter& operator=(const Painter&) = delete;
  ~Painter();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void SetPage(const Napi::CallbackInfo&);
  void SetColor(const Napi::CallbackInfo&);
  void SetColorCMYK(const Napi::CallbackInfo&);
  JsValue GetCanvas(const Napi::CallbackInfo&);
  void SetStrokingGrey(const Napi::CallbackInfo&);
  void SetGrey(const Napi::CallbackInfo&);
  void SetStrokingColorCMYK(const Napi::CallbackInfo&);
  void SetStrokeWidth(const Napi::CallbackInfo&);
  void SetStrokeStyle(const Napi::CallbackInfo&);
  void SetLineCapStyle(const Napi::CallbackInfo&);
  void SetLineJoinStyle(const Napi::CallbackInfo&);
  void SetFont(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetFont(const Napi::CallbackInfo&);
  void SetClipRect(const Napi::CallbackInfo&);
  void SetMiterLimit(const Napi::CallbackInfo&);
  void Rectangle(const Napi::CallbackInfo&);
  void Ellipse(const Napi::CallbackInfo&);
  void Circle(const Napi::CallbackInfo&);
  void ClosePath(const Napi::CallbackInfo&);
  void LineTo(const Napi::CallbackInfo&);
  void MoveTo(const Napi::CallbackInfo&);
  void CubicBezierTo(const Napi::CallbackInfo&);
  void HorizontalLineTo(const Napi::CallbackInfo&);
  void VerticalLineTo(const Napi::CallbackInfo&);
  void SmoothCurveTo(const Napi::CallbackInfo&);
  void QuadCurveTo(const Napi::CallbackInfo&);
  void ArcTo(const Napi::CallbackInfo&);
  void Close(const Napi::CallbackInfo&);
  void Stroke(const Napi::CallbackInfo&);
  void FillAndStroke(const Napi::CallbackInfo&);
  void Fill(const Napi::CallbackInfo&);
  void EndPath(const Napi::CallbackInfo&);
  void Clip(const Napi::CallbackInfo&);
  void Save(const Napi::CallbackInfo&);
  void Restore(const Napi::CallbackInfo&);
  void SetExtGState(const Napi::CallbackInfo&);
  void SetTabWidth(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetTabWidth(const Napi::CallbackInfo&);
  JsValue GetCurrentPath(const Napi::CallbackInfo&);
  void FinishPage(const Napi::CallbackInfo&);
  void DrawText(const Napi::CallbackInfo&);
  void DrawImage(const Napi::CallbackInfo&);
  void DrawMultiLineText(const Napi::CallbackInfo&);
  void DrawLine(const Napi::CallbackInfo&);
  void DrawTextAligned(const Napi::CallbackInfo&);
  JsValue GetMultiLineText(const Napi::CallbackInfo&);
  void BeginText(const Napi::CallbackInfo& info);
  void EndText(const Napi::CallbackInfo& info);
  void AddText(const Napi::CallbackInfo&);
  void MoveTextPosition(const Napi::CallbackInfo&);
  void DrawGlyph(const Napi::CallbackInfo&);
  JsValue GetPrecision(const Napi::CallbackInfo&);
  void SetPrecision(const Napi::CallbackInfo&, const JsValue&);

  PoDoFo::PdfPainter& GetPainter() const { return *Self; }

private:
  bool IsMemDoc = false;
  std::unique_ptr<PoDoFo::PdfPainter> Self;
  PoDoFo::PdfDocument* Doc;
  static void GetCMYK(JsValue&, float* CMYK);
  static void GetRGB(JsValue&, float* rgb);
  std::shared_ptr<spdlog::logger> Log;
};
}
#endif // NPDF_PAINTER_H
