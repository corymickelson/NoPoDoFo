//
// Created by red on 9/11/17.
//

#include "Rect.h"

Rect::Rect(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() == 0) {
    rect = *new PoDoFo::PdfRect();
  }
  if (info.Length() == 1) {
    if (info[0].IsObject() == false) {
      throw Error::New(info.Env(),
                       "Rect requires Page as constructor parameter");
    }
    Object pageObj = info[0].As<Object>();
    Page* page = Page::Unwrap(pageObj);
    rect = page->GetPage()->GetPageSize();
  }
  if (info.Length() == 4) {
    double left, bottom, width, height;
    for (uint8_t i = 0; i < info.Length(); i++) {
      if (info[i].IsNumber() == false) {
        throw Napi::Error::New(info.Env(),
                               "Rect requires (number, number, number, number) "
                               "as constructor parameters.");
      }
    }
    left = info[0].As<Number>();
    bottom = info[1].As<Number>();
    width = info[2].As<Number>();
    height = info[3].As<Number>();
    rect = *new PoDoFo::PdfRect(left, bottom, width, height);
  }
}
void
Rect::FromArray(const CallbackInfo& info)
{
  try {
    AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
    PdfArray array;
    Array coordinates = info[0].As<Array>();
    if (coordinates.Length() != 4) {
      for (uint32_t i = 0; i < coordinates.Length(); ++i) {
        double point = coordinates.Get(i).As<Number>();
        array.push_back(point);
      }
    }
    rect.FromArray(array);
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo Error: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}
void
Rect::Intersect(const CallbackInfo& info)
{
  if (info.Length() != 1) {
    throw Napi::Error::New(info.Env(),
                           "Intersect requires a single argument of type Rect");
  }
  Object rectObj = info[0].As<Object>();
  Rect* rectIntersect = Rect::Unwrap(rectObj);
  PdfRect rect = rectIntersect->GetRect();
  Rect::GetRect().Intersect(rect);
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
