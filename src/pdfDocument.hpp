//
// Created by red on 9/6/17.
//

#ifndef NPDF_PDFMEMDOCUMENT_H
#define NPDF_PDFMEMDOCUMENT_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;
using namespace std;

class pdfDocument : public Napi::ObjectWrap<pdfDocument> {
public:
  explicit pdfDocument(const CallbackInfo &callbackInfo); // constructor
  ~pdfDocument() { delete _document; }
  string originPdf;
  //  static Napi::FunctionReference constructor;

  static void Initialize(Napi::Env &env, Napi::Object &target) {
    Napi::HandleScope scope(env);
    Function ctor =
        DefineClass(env, "pdfDocument",
                    {InstanceMethod("load", &pdfDocument::Load),
                     InstanceMethod("getPageCount", &pdfDocument::GetPageCount),
                     InstanceMethod("getPage", &pdfDocument::GetPage),
                     InstanceMethod("setPassword", &pdfDocument::SetPassword),
                     InstanceMethod("getVersion", &pdfDocument::GetVersion),
                     InstanceMethod("isLinearized", &pdfDocument::IsLinearized),
                     InstanceMethod("write", &pdfDocument::Write)});

    //    constructor = Napi::Persistent(ctor);
    //    constructor.SuppressDestruct();
    target.Set("pdfDocument", ctor);
  }

  Napi::Value Load(const CallbackInfo &);

  Napi::Value GetPageCount(const CallbackInfo &);

  Napi::Value GetPage(const CallbackInfo &);

  void SetPassword(const CallbackInfo &);

  Napi::Value GetVersion(const CallbackInfo &);

  Napi::Value IsLinearized(const CallbackInfo &);

  Napi::Value Write(const CallbackInfo &);

  inline PoDoFo::PdfMemDocument *document() { return _document; }

private:
  PoDoFo::PdfMemDocument *_document;
};

#endif // NPDF_PDFMEMDOCUMENT_H
