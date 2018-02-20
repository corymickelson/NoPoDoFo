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

#ifndef NPDF_PAINTER_H
#define NPDF_PAINTER_H

#define CONVERSION_CONSTANT 0.002834645669291339

#include "Document.h"

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Painter : public Napi::ObjectWrap<Painter>
{
public:
  explicit Painter(const Napi::CallbackInfo& callbackInfo);
  ~Painter();

  PoDoFo::PdfRect pageSize;
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetPage(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetPage(const Napi::CallbackInfo&);

  void SetColor(const Napi::CallbackInfo&);
  void SetColorCMYK(const Napi::CallbackInfo&);
  Napi::Value GetCanvas(const Napi::CallbackInfo&);
  void SetStrokingGrey(const Napi::CallbackInfo&);
  void SetGrey(const Napi::CallbackInfo&);
  void SetStrokingColorCMYK(const Napi::CallbackInfo&);
  void SetStrokeWidth(const Napi::CallbackInfo&);
  void SetStrokeStyle(const Napi::CallbackInfo&);
  void SetLineCapStyle(const Napi::CallbackInfo&);
  void SetLineJoinStyle(const Napi::CallbackInfo&);
  void SetFont(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetFont(const Napi::CallbackInfo&);
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
  void SetTabWidth(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetTabWidth(const Napi::CallbackInfo&);
  Napi::Value GetCurrentPath(const Napi::CallbackInfo&);
  void FinishPage(const Napi::CallbackInfo&);
  void DrawText(const Napi::CallbackInfo&);
  void DrawImage(const Napi::CallbackInfo&);
  void DrawMultiLineText(const Napi::CallbackInfo&);
  void DrawLine(const Napi::CallbackInfo&);
  void DrawTextAligned(const Napi::CallbackInfo&);
  Napi::Value GetMultiLineText(const Napi::CallbackInfo&);
  void BeginText(const Napi::CallbackInfo& info);
  void EndText(const Napi::CallbackInfo& info);
  void AddText(const Napi::CallbackInfo&);
  void MoveTextPosition(const Napi::CallbackInfo&);
  void DrawGlyph(const Napi::CallbackInfo&);
  Napi::Value GetPrecision(const Napi::CallbackInfo&);
  void SetPrecision(const Napi::CallbackInfo&, const Napi::Value&);

  PoDoFo::PdfMemDocument* GetDocument() { return document->GetDocument(); }
  PoDoFo::PdfPainter* GetPainter() { return painter; }

private:
  PoDoFo::PdfPainter* painter;
  //  PoDoFo::PdfMemDocument* document;
  Document* document;
  void GetCMYK(Napi::Value&, int* cmyk);
  void GetRGB(Napi::Value&, int* rgb);
};
}
#endif // NPDF_PAINTER_H
