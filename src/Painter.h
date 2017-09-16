//
// Created by red on 9/13/17.
//

#ifndef NPDF_PAINTER_H
#define NPDF_PAINTER_H

#include "Image.h"
#include "ValidateArguments.h"
#include <napi.h>
#include <podofo/podofo.h>

class Painter : public Napi::ObjectWrap<Painter>
{
public:
  Painter(const Napi::CallbackInfo& callbackInfo);
  ~Painter()
  {
    delete _painter;
    delete _document;
  }
  PoDoFo::PdfRect pageSize;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor =
      DefineClass(env,
                  "Painter",
                  { InstanceMethod("setPage", &Painter::SetPage),
                    InstanceMethod("finishPage", &Painter::FinishPage),
                    InstanceMethod("drawText", &Painter::DrawText),
                    InstanceMethod("drawImage", &Painter::DrawImage),
                    InstanceMethod("getPrecision", &Painter::GetPrecision) });
    target.Set("Painter", ctor);
  }
  void SetPage(const CallbackInfo&);
  void FinishPage(const CallbackInfo&);
  void DrawText(const CallbackInfo&);
  void DrawImage(const CallbackInfo&);
  Napi::Value GetPrecision(const CallbackInfo&);
  PoDoFo::PdfMemDocument* GetDocument() { return _document; }
  PoDoFo::PdfPainter* GetPainter() { return _painter; }

private:
  PoDoFo::PdfPainter* _painter;
  PoDoFo::PdfMemDocument* _document;
};

#endif // NPDF_PAINTER_H
