//
// Created by red on 9/7/17.
//

#ifndef NPDF_PDFPAGE_HPP
#define NPDF_PDFPAGE_HPP

#define CONVERSION_CONSTANT 0.002834645669291339

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {

using namespace std;

class Page : public Napi::ObjectWrap<Page>
{
public:
  explicit Page(const Napi::CallbackInfo& callbackInfo);
  ~Page();

  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetRotation(const Napi::CallbackInfo&);
  Napi::Value GetNumFields(const Napi::CallbackInfo&);
  void SetRotation(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetField(const Napi::CallbackInfo&);
  Napi::Value GetFields(const Napi::CallbackInfo&);
  Napi::Value GetFieldIndex(const Napi::CallbackInfo&);
  void SetPageWidth(const Napi::CallbackInfo&, const Napi::Value&);
  void SetPageHeight(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetPageWidth(const Napi::CallbackInfo&);
  Napi::Value GetPageHeight(const Napi::CallbackInfo&);
  Napi::Value GetTrimBox(const Napi::CallbackInfo&);
  void SetTrimBox(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetPageNumber(const Napi::CallbackInfo&);
  Napi::Value GetContents(const Napi::CallbackInfo&);
  Napi::Value GetResources(const Napi::CallbackInfo&);
  Napi::Value GetMediaBox(const Napi::CallbackInfo&);
  Napi::Value GetBleedBox(const Napi::CallbackInfo&);
  Napi::Value GetArtBox(const Napi::CallbackInfo&);
  Napi::Value CreateAnnotation(const Napi::CallbackInfo&);
  Napi::Value GetAnnotation(const Napi::CallbackInfo&);
  Napi::Value GetNumAnnots(const Napi::CallbackInfo&);
  void DeleteAnnotation(const Napi::CallbackInfo&);

  PoDoFo::PdfPage* GetPage() { return page; }

private:
  PoDoFo::PdfPage* page;
  void GetFieldObject(Napi::Object&, PoDoFo::PdfField&);
  Napi::Object ExtractAndApplyRectValues(const Napi::CallbackInfo&,
                                         PoDoFo::PdfRect&);
};
}
#endif // NPDF_PDFPAGE_HPP
