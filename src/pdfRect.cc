//
// Created by red on 9/8/17.
//

#include "pdfRect.hpp"

Napi::FunctionReference Rectangle::constructor;

Rectangle::Rectangle(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  PoDoFo::PdfRect* pdfRect;
  if (info.Length() == 4 && info[1].IsNumber()) {
    // PoDoFo::PdfRect(double,double,double,double)
  } else if (info.Length() == 1)
    pdfRect = info[0].As<Napi::External<PoDoFo::PdfRect>>().Data();
  _rectangle = pdfRect;
}

Napi::Value
Rectangle::FromArray(const CallbackInfo& info)
{
  if (info.Length() == 0) {
    throw;
  }
  if (info.Length() == 1 && !info[0].IsArray()) {
    throw;
  }
  Napi::Array dim = info[0].As<Array>();
  if (dim.Length() < 4) {
    throw;
  }
  PoDoFo::PdfArray* pdfArray; // = new PoDoFo::PdfArray();
  for (uint32_t i = 0; i < dim.Length(); i++) {
    Napi::Value p = dim[i];
    if (p.Type() != napi_number) {
      break;
    }
    double point = p.As<Napi::Number>();
    if (isnan(point)) {
      break;
    }
    pdfArray->push_back(PoDoFo::PdfObject(p));
  }
  _rectangle->FromArray(*pdfArray);
  return info.This();
}

Napi::Value
Rectangle::GetBottom(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _rectangle->GetBottom());
}

void
Rectangle::SetBottom(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsNumber()) {
    throw Napi::Error::New(info.Env(), "");
  }
  double value = info[0].As<Number>();
  _rectangle->SetBottom(value);
}

Napi::Value
Rectangle::GetHeight(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _rectangle->GetHeight());
}

void
Rectangle::SetHeight(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsNumber()) {
    throw Napi::Error::New(info.Env(), "");
  }
  double value = info[0].As<Number>();
  _rectangle->SetHeight(value);
}

Napi::Value
Rectangle::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _rectangle->GetWidth());
}

void
Rectangle::SetWidth(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsNumber()) {
    throw Napi::Error::New(info.Env(), "");
  }
  double value = info[0].As<Number>();
  _rectangle->SetWidth(value);
}

Napi::Value
Rectangle::GetLeft(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _rectangle->GetLeft());
}

void
Rectangle::SetLeft(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsNumber()) {
    throw Napi::Error::New(info.Env(), "");
  }
  double value = info[0].As<Number>();
  _rectangle->SetLeft(value);
}
