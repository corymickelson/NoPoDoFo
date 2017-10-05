//
// Created by red on 9/13/17.
//

#ifndef NPDF_PAINTER_H
#define NPDF_PAINTER_H

#define CONVERSION_CONSTANT 0.002834645669291339

#include <napi.h>
#include <podofo/podofo.h>

class Painter : public Napi::ObjectWrap<Painter>
{
public:
  Painter(const Napi::CallbackInfo& callbackInfo);
  ~Painter()
  {
    delete painter;
    delete document;
  }
  PoDoFo::PdfRect pageSize;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetPage(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetPage(const Napi::CallbackInfo&);
  void FinishPage(const Napi::CallbackInfo&);
  void DrawText(const Napi::CallbackInfo&);
  void DrawImage(const Napi::CallbackInfo&);
  Napi::Value GetPrecision(const Napi::CallbackInfo&);
  void SetPrecision(const Napi::CallbackInfo&, const Napi::Value&);

  PoDoFo::PdfMemDocument* GetDocument() { return document; }
  PoDoFo::PdfPainter* GetPainter() { return painter; }

private:
  PoDoFo::PdfPainter* painter;
  PoDoFo::PdfMemDocument* document;
};

#endif // NPDF_PAINTER_H
