//
// Created by red on 9/7/17.
//

#include "pdfPage.hpp"

Napi::FunctionReference Page::constructor;

Page::Page(const CallbackInfo& info)
  : ObjectWrap(info)
{
  PoDoFo::PdfPage* page = info[0].As<Napi::External<PoDoFo::PdfPage>>().Data();
  _page = page;
}

Napi::Value
Page::GetRotation(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _page->GetRotation());
}

Napi::Value
Page::GetNumFields(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _page->GetNumFields());
}

Napi::Value
Page::GetField(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsNumber()) {
    throw Napi::Error::New(
      info.Env(), "SetRotation takes a single argument of type number.");
  }
  int n = info[0].As<Number>();
  PoDoFo::PdfField field = _page->GetField(n);
}

void
Page::SetRotation(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsNumber()) {
    throw Napi::Error::New(
      info.Env(), "SetRotation takes a single argument of type number.");
  }
  int rotate = info[0].As<Number>();
  _page->SetRotation(rotate);
}
