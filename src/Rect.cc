//
// Created by red on 9/11/17.
//

#include "Rect.h"

Rect::Rect(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() != 1) {
    throw Error::New(info.Env(), "Rect requires Page as constructor parameter");
  }
  Object pageObj = info[0].As<Object>();
  Page* page = Page::Unwrap(pageObj);
  rect = page->GetPage()->GetPageSize();
}
Napi::Value
Rect::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect.GetWidth());
}
Napi::Value
Rect::GetHeight(const CallbackInfo& info)
{

  return Napi::Number::New(info.Env(), rect.GetHeight());
}
Napi::Value
Rect::GetLeft(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect.GetLeft());
}
Napi::Value
Rect::GetBottom(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), rect.GetBottom());
}
void
Rect::SetWidth(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double width = info[0].As<Number>();
  rect.SetWidth(width);
}
void
Rect::SetHeight(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double height = info[0].As<Number>();
  rect.SetHeight(height);
}
void
Rect::SetLeft(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double left = info[0].As<Number>();
  rect.SetLeft(left);
}
void
Rect::SetBottom(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double bottom = info[0].As<Number>();
  rect.SetBottom(bottom);
}
