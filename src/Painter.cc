//
// Created by red on 9/13/17.
//

#include "Painter.h"
#include "Document.h"
Painter::Painter(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
}
void
Painter::SetPage(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto pageObj = info[0].As<Object>();
  Page* pagePtr = Page::Unwrap(pageObj);
  PoDoFo::PdfPage* page = pagePtr->GetPage();
  _document = pagePtr->GetDocument();
  _painter = new PoDoFo::PdfPainter();
  _painter->SetPage(page);
  pageSize = page->GetPageSize();
}
void
Painter::FinishPage(const CallbackInfo& info)
{
  _painter->FinishPage();
}
void
Painter::DrawText(const CallbackInfo& info)
{
}
void
Painter::DrawImage(const CallbackInfo& info)
{
  //  try {
  //    Object imgObj = info[0].As<Object>();
  //    Image* img_ = Image::Unwrap(imgObj);
  //    PdfImage img = img_->GetImage();
  //    //    PdfImage* img = img_->GetImage();
  //    _painter->DrawImage(0.0, pageSize.GetHeight() - img.GetHeight(), &img);

  //  } catch (PdfError& err) {
  //    stringstream msg;
  //    msg << "PoDoFo failure, error code: " << err.GetError() << endl;
  //    throw Napi::Error::New(info.Env(), msg.str());
  //  }
  double x, y, width, height;
  string imgFile = info[0].As<String>().Utf8Value();
  x = info[1].As<Number>();
  y = info[2].As<Number>();
  width = info[3].As<Number>();
  height = info[4].As<Number>();
  PoDoFo::PdfImage image(_document);
  image.LoadFromFile(imgFile.c_str());
  _painter->DrawImage(0.0, pageSize.GetHeight() - image.GetHeight(), &image);
}

Napi::Value
Painter::GetPrecision(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           static_cast<double>(_painter->GetPrecision()));
}
