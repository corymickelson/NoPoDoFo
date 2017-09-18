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

class Page : public Napi::ObjectWrap<Page>
{
public:
  explicit Page(const CallbackInfo& callbackInfo);
  ~Page()
  {
    delete _page;
    delete _parent;
  }
  int pageNumber;
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor =
      DefineClass(env,
                  "PdfPage",
                  { InstanceMethod("getRotation", &Page::GetRotation),
                    InstanceMethod("getNumFields", &Page::GetNumFields),
                    InstanceMethod("setRotation", &Page::SetRotation),
                    InstanceMethod("getFields", &Page::GetFields),
                    InstanceMethod("getFieldIndex", &Page::GetFieldIndex) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Page", constructor);
  }
  Napi::Value GetRotation(const CallbackInfo&);
  Napi::Value GetNumFields(const CallbackInfo&);
  void SetRotation(const CallbackInfo&);

  Napi::Value GetFields(const CallbackInfo&);
  Napi::Value GetFieldIndex(const CallbackInfo&);
  PoDoFo::PdfPage* GetPage() { return _page; }
  PoDoFo::PdfMemDocument* GetDocument() { return _parent; }

private:
  PoDoFo::PdfPage* _page;
  PoDoFo::PdfMemDocument* _parent;
  void GetFieldObject(Napi::Object&, PoDoFo::PdfField&);
};

#endif // NPDF_PDFPAGE_HPP
