//
// Created by red on 9/11/17.
//

#include "Rect.h"

using namespace Napi;
using namespace PoDoFo;
namespace NoPoDoFo {
FunctionReference Rect::constructor;

Rect::Rect(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() == 0) {
    rect = new PoDoFo::PdfRect();
  }
  if (info.Length() == 1) {
    if (!info[0].IsObject()) {
      throw Error::New(info.Env(),
                       "Rect requires Page as constructor parameter");
    }
    auto pageObj = info[0].As<Object>();
    if (!pageObj.InstanceOf(Page::constructor.Value())) {
      throw Error::New(info.Env(),
                       "Rect requires Page as constructor parameter");
    }
    Page* page = Page::Unwrap(pageObj);
    rect = new PdfRect(page->GetPage()->GetPageSize());
  }
  if (info.Length() == 4) {
    double left, bottom, width, height;
    for (uint8_t i = 0; i < info.Length(); i++) {
      if (!info[i].IsNumber()) {
        throw Napi::Error::New(info.Env(),
                               "Rect requires (number, number, number, number) "
                               "as constructor parameters.");
      }
    }
    left = info[0].As<Number>();
    bottom = info[1].As<Number>();
    width = info[2].As<Number>();
    height = info[3].As<Number>();
    rect = new PoDoFo::PdfRect(left, bottom, width, height);
  }
}

Rect::~Rect()
{
  if (rect != nullptr) {
    HandleScope scope(Env());
    delete rect;
  }
}
void
Rect::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Rect",
    { InstanceAccessor("left", &Rect::GetLeft, &Rect::SetLeft),
      InstanceAccessor("bottom", &Rect::GetBottom, &Rect::SetBottom),
      InstanceAccessor("width", &Rect::GetWidth, &Rect::SetWidth),
      InstanceAccessor("height", &Rect::GetHeight, &Rect::SetHeight),
      InstanceMethod("intersect", &Rect::Intersect)

    });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Rect", ctor);
}
void
Rect::Intersect(const CallbackInfo& info)
{
  if (info.Length() != 1) {
    throw Napi::Error::New(info.Env(),
                           "Intersect requires a single argument of type Rect");
  }
  auto rectObj = info[0].As<Object>();
  Rect* rectIntersect = Rect::Unwrap(rectObj);
  PdfRect* _rect = rectIntersect->GetRect();
  rect->Intersect(*_rect);
}
Napi::Value
Rect::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect->GetWidth());
}
Napi::Value
Rect::GetHeight(const CallbackInfo& info)
{

  return Napi::Number::New(info.Env(), rect->GetHeight());
}
Napi::Value
Rect::GetLeft(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect->GetLeft());
}
Napi::Value
Rect::GetBottom(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect->GetBottom());
}
void
Rect::SetWidth(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double width = value.As<Number>();
  rect->SetWidth(width);
}
void
Rect::SetHeight(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double height = value.As<Number>();
  rect->SetHeight(height);
}
void
Rect::SetLeft(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double left = value.As<Number>();
  rect->SetLeft(left);
}
void
Rect::SetBottom(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "SetWidth requies a single argument of type number");
  }
  double bottom = value.As<Number>();
  rect->SetBottom(bottom);
}
}
