//
// Created by red on 9/7/17.
//

#ifndef NPDF_PDFPAGE_HPP
#define NPDF_PDFPAGE_HPP

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace std;

class Page : public Napi::ObjectWrap<Page>
{
public:
  explicit Page(const CallbackInfo& callbackInfo);
  ~Page() { delete _page; }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::HandleScope scope(env);
    Napi::Function ctor =
      DefineClass(env,
                  "PdfPage",
                  { InstanceMethod("getRotation", &Page::GetRotation),
                    InstanceMethod("getNumFields", &Page::GetNumFields),
                    InstanceMethod("getField", &Page::GetField),
                    InstanceMethod("setRotation", &Page::SetRotation) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Page", constructor);
  }
  Napi::Value GetRotation(const CallbackInfo&);
  Napi::Value GetNumFields(const CallbackInfo&);
  Napi::Value GetField(const CallbackInfo&);
  void SetRotation(const CallbackInfo&);
  inline PoDoFo::PdfPage* page() { return _page; }

private:
  PoDoFo::PdfPage* _page;
};

#endif // NPDF_PDFPAGE_HPP
