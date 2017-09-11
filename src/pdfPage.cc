//
// Created by red on 9/7/17.
//

#include "pdfPage.hpp"

Napi::FunctionReference Page::constructor;

Page::Page(const CallbackInfo& info)
  : ObjectWrap(info)
{
  PoDoFo::PdfPage* page = info[0].As<Napi::External<PoDoFo::PdfPage>>().Data();
  PoDoFo::PdfMemDocument* parent =
    info[1].As<Napi::External<PoDoFo::PdfMemDocument>>().Data();
  _parent = parent;
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
Napi::Value
Page::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _page->GetPageSize().GetWidth());
}
Napi::Value
Page::GetHeight(const CallbackInfo& info)
{

  return Napi::Number::New(info.Env(), _page->GetPageSize().GetHeight());
}
Napi::Value
Page::GetLeft(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _page->GetPageSize().GetLeft());
}
Napi::Value
Page::GetBottom(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), _page->GetPageSize().GetBottom());
}
void
Page::SetWidth(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double width = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetWidth(width);
}
void
Page::SetHeight(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double height = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetHeight(height);
}
void
Page::SetLeft(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double left = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetLeft(left);
}
void
Page::SetBottom(const CallbackInfo& info)
{
  if (info.Length() > 0) {
    if (!info[0].IsNumber()) {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double bottom = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetBottom(bottom);
}
void
Page::AddImg(const CallbackInfo& info)
{
  double x, y, width, height;
  string imgFile = info[0].As<String>().Utf8Value();
  x = info[1].As<Number>();
  y = info[2].As<Number>();
  width = info[3].As<Number>();
  height = info[4].As<Number>();

  PoDoFo::PdfPainter painter;
  PoDoFo::PdfImage image(_parent);
  image.LoadFromFile(imgFile.c_str());
  painter.SetPage(_page);
  //  painter.DrawImage(x, y, &image, width, height);
  painter.DrawImage(
    0.0, _page->GetPageSize().GetHeight() - image.GetHeight(), &image);
  painter.FinishPage();
}
