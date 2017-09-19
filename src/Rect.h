//
// Created by red on 9/11/17.
//

#ifndef NPDF_RECT_H
#define NPDF_RECT_H

#include "Page.h"
#include "ValidateArguments.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class Rect : public ObjectWrap<Rect>
{
public:
  Rect(const CallbackInfo& callbackInfo);
  ~Rect();
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env,
                                "Rect",
                                {
                                  InstanceMethod("intersect", &Rect::Intersect),
                                  InstanceMethod("getWidth", &Rect::GetWidth),
                                  InstanceMethod("getHeight", &Rect::GetHeight),
                                  InstanceMethod("getBottom", &Rect::GetBottom),
                                  InstanceMethod("getLeft", &Rect::GetLeft),
                                  InstanceMethod("setWidth", &Rect::SetWidth),
                                  InstanceMethod("setHeight", &Rect::SetHeight),
                                  InstanceMethod("setLeft", &Rect::SetLeft),
                                  InstanceMethod("setBottom", &Rect::SetBottom),

                                });
    target.Set("Rect", ctor);
  }

  void Intersect(const CallbackInfo&);
  Napi::Value GetWidth(const CallbackInfo&);
  void SetWidth(const CallbackInfo&);
  Napi::Value GetHeight(const CallbackInfo&);
  void SetHeight(const CallbackInfo&);
  Napi::Value GetBottom(const CallbackInfo&);
  void SetBottom(const CallbackInfo&);
  Napi::Value GetLeft(const CallbackInfo&);
  void SetLeft(const CallbackInfo&);
  PoDoFo::PdfRect GetRect() { return rect; }

private:
  PoDoFo::PdfRect rect = *new PoDoFo::PdfRect();
};

#endif // NPDF_RECT_H
