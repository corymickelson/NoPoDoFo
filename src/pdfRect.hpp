//
// Created by red on 9/8/17.
//

#ifndef NPDF_PDFRECT_HPP
#define NPDF_PDFRECT_HPP

#include <napi.h>
#include <podofo/podofo.h>
#include <vector>

using namespace Napi;
using namespace std;

class Rectangle : public Napi::ObjectWrap<Rectangle>
{
public:
  explicit Rectangle(const Napi::CallbackInfo& callbackInfo);

  ~Rectangle() { delete _rectangle; }

  static Napi::FunctionReference constructor;

  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor =
      DefineClass(env,
                  "Rectangle",
                  { InstanceMethod("fromArray", &Rectangle::FromArray),
                    InstanceMethod("getBottom", &Rectangle::GetBottom),
                    InstanceMethod("setBottom", &Rectangle::SetBottom),
                    InstanceMethod("getLeft", &Rectangle::GetLeft),
                    InstanceMethod("setLeft", &Rectangle::SetLeft),
                    InstanceMethod("getWidth", &Rectangle::GetWidth),
                    InstanceMethod("setWidth", &Rectangle::SetWidth),
                    InstanceMethod("getHeight", &Rectangle::GetHeight),
                    InstanceMethod("setHeight", &Rectangle::SetHeight) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Rectangle", constructor);
  }

  Napi::Value FromArray(const CallbackInfo&);
  Napi::Value GetBottom(const CallbackInfo&);
  void SetBottom(const CallbackInfo&);
  Napi::Value GetLeft(const CallbackInfo&);
  void SetLeft(const CallbackInfo&);
  Napi::Value GetWidth(const CallbackInfo&);
  void SetWidth(const CallbackInfo&);
  Napi::Value GetHeight(const CallbackInfo&);
  void SetHeight(const CallbackInfo&);

private:
  PoDoFo::PdfRect* _rectangle;
};

#endif // NPDF_PDFRECT_HPP
