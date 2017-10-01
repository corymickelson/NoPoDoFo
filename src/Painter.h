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
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor =
      DefineClass(env,
                  "Painter",
                  { InstanceAccessor("page", &Painter::GetPage, &Painter::SetPage),
                    InstanceAccessor("precision", &Painter::GetPrecision, &Painter::SetPrecision),
                    InstanceMethod("finishPage", &Painter::FinishPage),
                    InstanceMethod("drawText", &Painter::DrawText),
                    InstanceMethod("drawImage", &Painter::DrawImage) });
    target.Set("Painter", ctor);
  }
  void SetPage(const CallbackInfo&, const Napi::Value &);
  Napi::Value GetPage(const CallbackInfo &);
  void FinishPage(const CallbackInfo&);
  void DrawText(const CallbackInfo&);
  void DrawImage(const CallbackInfo&);
  Napi::Value GetPrecision(const CallbackInfo&);
  void SetPrecision(const CallbackInfo &, const Napi::Value &);

  PoDoFo::PdfMemDocument* GetDocument() { return document; }
  PoDoFo::PdfPainter* GetPainter() { return painter; }

private:
  PoDoFo::PdfPainter* painter;
  PoDoFo::PdfMemDocument* document;
};

#endif // NPDF_PAINTER_H
