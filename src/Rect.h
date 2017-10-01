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
  Rect(const CallbackInfo &callbackInfo);
  ~Rect();
  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env,
                                "Rect",
                                {
                                  InstanceAccessor("left", &Rect::GetLeft, &Rect::SetLeft),
                                  InstanceAccessor("bottom", &Rect::GetBottom, &Rect::SetBottom),
                                  InstanceAccessor("width", &Rect::GetWidth, &Rect::SetWidth),
                                  InstanceAccessor("height", &Rect::GetHeight, &Rect::SetHeight),
                                  InstanceMethod("intersect", &Rect::Intersect)

                                });
    target.Set("Rect", ctor);
  }

  void
  Intersect(const CallbackInfo &);
  Napi::Value
  GetWidth(const CallbackInfo &);
  void
  SetWidth(const CallbackInfo &, const Napi::Value &);
  Napi::Value
  GetHeight(const CallbackInfo &);
  void
  SetHeight(const CallbackInfo &, const Napi::Value &);
  Napi::Value
  GetBottom(const CallbackInfo &);
  void
  SetBottom(const CallbackInfo &, const Napi::Value &);
  Napi::Value
  GetLeft(const CallbackInfo &);
  void
  SetLeft(const CallbackInfo &, const Napi::Value &);
  PoDoFo::PdfRect
  GetRect() { return rect; }

private:
  PoDoFo::PdfRect rect = *new PoDoFo::PdfRect();
};

#endif // NPDF_RECT_H
