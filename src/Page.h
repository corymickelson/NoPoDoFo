//
// Created by red on 9/7/17.
//

#ifndef NPDF_PDFPAGE_HPP
#define NPDF_PDFPAGE_HPP

#include <napi.h>
#include <podofo/podofo.h>
#include "Document.h"

using namespace Napi;
using namespace std;

class Page : public Napi::ObjectWrap<Page>
{
public:
  explicit Page(const CallbackInfo &callbackInfo);
  ~Page()
  {
    delete _page;
    delete _parent;
  }
  int pageNumber;
  static Napi::FunctionReference constructor;
  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor =
      DefineClass(env,
                  "PdfPage",
                  {InstanceMethod("getRotation", &Page::GetRotation),
                   InstanceMethod("getNumFields", &Page::GetNumFields),
//                   InstanceMethod("getPageSize", &Page::GetPageSize),
                   InstanceMethod("getField", &Page::GetField),
                   InstanceMethod("getFields", &Page::GetFields),
                   InstanceMethod("getWidth", &Page::GetWidth),
                   InstanceMethod("getHeight", &Page::GetHeight),
                   InstanceMethod("getBottom", &Page::GetBottom),
                   InstanceMethod("getLeft", &Page::GetLeft),
                   InstanceMethod("setWidth", &Page::SetWidth),
                   InstanceMethod("setHeight", &Page::SetHeight),
                   InstanceMethod("setLeft", &Page::SetLeft),
                   InstanceMethod("setBottom", &Page::SetBottom),
                   InstanceMethod("setRotation", &Page::SetRotation),
                   InstanceMethod("addImg", &Page::AddImg)});
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Page", constructor);
  }
  Napi::Value
  GetRotation(const CallbackInfo &);
  Napi::Value
  GetNumFields(const CallbackInfo &);
  Napi::Value
  GetField(const CallbackInfo &);
  void
  SetRotation(const CallbackInfo &);
  Napi::Value
  GetWidth(const CallbackInfo &);
  void
  SetWidth(const CallbackInfo &);
  Napi::Value
  GetHeight(const CallbackInfo &);
  void
  SetHeight(const CallbackInfo &);
  Napi::Value
  GetBottom(const CallbackInfo &);
  void
  SetBottom(const CallbackInfo &);
  Napi::Value
  GetLeft(const CallbackInfo &);
  void
  SetLeft(const CallbackInfo &);
  void
  AddImg(const CallbackInfo &);
  Napi::Value
  GetFields(const CallbackInfo &);
  void
  SetFields(const CallbackInfo &);

private:
  PoDoFo::PdfPage *_page;
  PoDoFo::PdfMemDocument *_parent;

  void
  GetFieldObject(Napi::Object &, PoDoFo::PdfField &);
};

#endif // NPDF_PDFPAGE_HPP
