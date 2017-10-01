//
// Created by red on 9/7/17.
//

#ifndef NPDF_PDFPAGE_HPP
#define NPDF_PDFPAGE_HPP

#define CONVERSION_CONSTANT 0.002834645669291339

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace std;
using namespace PoDoFo;

class Page : public Napi::ObjectWrap<Page>
{
public:
  explicit Page(const CallbackInfo& callbackInfo);
  ~Page()
  {
    delete page;
    delete parent;
  }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor = DefineClass(
      env,
      "Page",
      { InstanceAccessor("rotation", &Page::GetRotation, &Page::SetRotation),
        InstanceAccessor("trimBox", &Page::GetTrimBox, &Page::SetTrimBox),
        InstanceAccessor("number", &Page::GetPageNumber, nullptr),
        InstanceAccessor("width", &Page::GetPageWidth, &Page::SetPageWidth),
        InstanceAccessor("height", &Page::GetPageHeight, &Page::SetPageHeight),

        InstanceMethod("getNumFields", &Page::GetNumFields),
        InstanceMethod("getFields", &Page::GetFields),
        InstanceMethod("getFieldIndex", &Page::GetFieldIndex),
        InstanceMethod("getContents", &Page::GetContents),
        InstanceMethod("getResources", &Page::GetResources),
        InstanceMethod("getMediaBox", &Page::GetMediaBox),
        InstanceMethod("getBleedBox", &Page::GetBleedBox),
        InstanceMethod("getArtBox", &Page::GetArtBox),
        InstanceMethod("createAnnotation", &Page::CreateAnnotation),
        InstanceMethod("getAnnotation", &Page::GetAnnotation),
        InstanceMethod("getNumAnnots", &Page::GetNumAnnots),
        InstanceMethod("deleteAnnotation", &Page::DeleteAnnotation) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Page", constructor);
  }
  Napi::Value GetRotation(const CallbackInfo&);
  Napi::Value GetNumFields(const CallbackInfo&);
  void SetRotation(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetFields(const CallbackInfo&);
  Napi::Value GetFieldIndex(const CallbackInfo&);
  void SetPageWidth(const CallbackInfo&, const Napi::Value&);
  void SetPageHeight(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetPageWidth(const CallbackInfo&);
  Napi::Value GetPageHeight(const CallbackInfo&);
  Napi::Value GetTrimBox(const CallbackInfo&);
  void SetTrimBox(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetPageNumber(const CallbackInfo&);
  Napi::Value GetContents(const CallbackInfo&);
  Napi::Value GetResources(const CallbackInfo&);
  Napi::Value GetMediaBox(const CallbackInfo&);
  Napi::Value GetBleedBox(const CallbackInfo&);
  Napi::Value GetArtBox(const CallbackInfo&);
  Napi::Value CreateAnnotation(const CallbackInfo&);
  Napi::Value GetAnnotation(const CallbackInfo &);
  Napi::Value GetNumAnnots(const CallbackInfo&);
  void DeleteAnnotation(const CallbackInfo&);

  PoDoFo::PdfPage* GetPage() { return page; }
  PoDoFo::PdfMemDocument* GetDocument() { return parent; }

private:
  PoDoFo::PdfPage* page;
  PoDoFo::PdfMemDocument* parent;
  void GetFieldObject(Napi::Object&, PoDoFo::PdfField&);
  Napi::Object ExtractAndApplyRectValues(const CallbackInfo&, PdfRect&);
};

#endif // NPDF_PDFPAGE_HPP
