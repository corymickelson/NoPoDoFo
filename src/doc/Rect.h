//
// Created by red on 9/11/17.
//

#ifndef NPDF_RECT_H
#define NPDF_RECT_H

#include "../ValidateArguments.h"
#include "Page.h"
#include <napi.h>
#include <podofo/podofo.h>
namespace NoPoDoFo {
class Rect : public Napi::ObjectWrap<Rect>
{
public:
  Rect(const Napi::CallbackInfo& callbackInfo);
  ~Rect() = default;
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void Intersect(const Napi::CallbackInfo&);
  Napi::Value GetWidth(const Napi::CallbackInfo&);
  void SetWidth(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetHeight(const Napi::CallbackInfo&);
  void SetHeight(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetBottom(const Napi::CallbackInfo&);
  void SetBottom(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetLeft(const Napi::CallbackInfo&);
  void SetLeft(const Napi::CallbackInfo&, const Napi::Value&);
  PoDoFo::PdfRect GetRect() { return rect; }

private:
  PoDoFo::PdfRect rect = *new PoDoFo::PdfRect();
};
}
#endif // NPDF_RECT_H
