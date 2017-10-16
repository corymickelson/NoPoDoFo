//
// Created by red on 9/13/17.
//

#include "Painter.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Document.h"
#include "Image.h"
#include "Page.h"

using namespace Napi;
using namespace PoDoFo;

Painter::Painter(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{}

void
Painter::Initialize(Napi::Env& env, Napi::Object& target)
{
  Napi::HandleScope scope(env);
  Napi::Function ctor = DefineClass(
    env,
    "Painter",
    { InstanceAccessor("page", &Painter::GetPage, &Painter::SetPage),
      InstanceAccessor(
        "precision", &Painter::GetPrecision, &Painter::SetPrecision),
      InstanceMethod("finishPage", &Painter::FinishPage),
      InstanceMethod("drawText", &Painter::DrawText),
      InstanceMethod("drawImage", &Painter::DrawImage) });
  target.Set("Painter", ctor);
}
void
Painter::SetPage(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsObject()) {
    throw Napi::Error::New(info.Env(), "Page must be an instance of Page.");
  }
  auto pageObj = value.As<Object>();
  Page* pagePtr = Page::Unwrap(pageObj);
  if (!pageObj.InstanceOf(Page::constructor.Value())) {
    throw Napi::Error::New(info.Env(), "Page must be an instance of Page.");
  }
  PoDoFo::PdfPage* page = pagePtr->GetPage();
  document = pagePtr->GetDocument();
  painter = new PoDoFo::PdfPainter();
  painter->SetPage(page);
  pageSize = page->GetPageSize();
}
Napi::Value
Painter::GetPage(const CallbackInfo& info)
{
  auto* page = dynamic_cast<PdfPage*>(painter->GetPage());
  auto pagePtr = Napi::External<PdfPage>::New(info.Env(), page);
  auto docPtr = Napi::External<PdfMemDocument>::New(info.Env(), document);
  auto instance = Page::constructor.New({ pagePtr, docPtr });
  return instance;
}
void
Painter::FinishPage(const CallbackInfo& info)
{
  painter->FinishPage();
}
void
Painter::DrawText(const CallbackInfo& info)
{}
void
Painter::DrawImage(const CallbackInfo& info)
{
  try {
    if (info.Length() < 3) {
      throw Napi::Error::New(
        info.Env(),
        "DrawImage requires a minimum of three parameters: Image, x, y");
    }
    // Image
    auto imgObj = info[0].As<Object>();
    Image* imgInstance = Image::Unwrap(imgObj);
    PdfImage img = imgInstance->GetImage();

    // Coordinates
    double x, y;
    if (!info[1].IsNumber() || !info[2].IsNumber()) {
      throw Napi::Error::New(info.Env(), "coorindates must be of type number");
    }
    x = info[1].As<Number>().DoubleValue();
    y = info[2].As<Number>().DoubleValue();

    // Scaling
    double width = 0.0, height = 0.0;
    if (info.Length() == 5) {
      if (!info[3].IsNumber() || !info[4].IsNumber()) {
        throw Napi::Error::New(info.Env(),
                               "scaling width & height must be of type number");
      }
      width = info[3].As<Number>().DoubleValue();
      height = info[4].As<Number>().DoubleValue();
    }
    if (width != 0.0 && height != 0.0)
      painter->DrawImage(x, y, &img, width, height);
    else
      painter->DrawImage(x, y, &img);

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Painter::GetPrecision(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           static_cast<double>(painter->GetPrecision()));
}
void
Painter::SetPrecision(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Precision must be of type number");
  }
  unsigned short p =
    static_cast<unsigned short>(value.As<Number>().Uint32Value());
  painter->SetPrecision(p);
}
