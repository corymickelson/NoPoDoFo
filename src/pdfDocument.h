//
// Created by red on 9/6/17.
//

#ifndef NPDF_PDFMEMDOCUMENT_H
#define NPDF_PDFMEMDOCUMENT_H

#include "library.hpp"

class pdfDocument : public Napi::ObjectWrap<pdfDocument> {
public:
  explicit pdfDocument(const CallbackInfo &callbackInfo); // constructor
  ~pdfDocument();
  string originPdf;
  static Napi::FunctionReference constructor;

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

    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();

    target.Set("pdfDocument", ctor);
  }

  Napi::Value Load(const CallbackInfo &);

  Napi::Value GetPageCount(const CallbackInfo &);

  Napi::Value GetPage(const CallbackInfo &);

  void SetPassword(const CallbackInfo &);

  Napi::Value GetVersion(const CallbackInfo &);

  Napi::Value IsLinearized(const CallbackInfo &);

  void Write(const CallbackInfo &);

  inline PoDoFo::PdfMemDocument *document() { return _document; }

private:
  PoDoFo::PdfMemDocument *_document;
};

/*class PdfMemDocument {
public:
        void Init(napi_env env, napi_value exports);
        static void Destructor( napi_env env, void *obj, void *finalize );

private:
        explicit PdfMemDocument(const string &file);
        ~PdfMemDocument( );

        static napi_value New(napi_env, napi_callback_info);
        static napi_value Load(napi_env, napi_callback_info);
        static napi_value GetPageCount(napi_env, napi_callback_info);
        static napi_value GetPage(napi_env, napi_callback_info);
        static napi_value Write(napi_env, napi_callback_info);

        static napi_ref constructor;
        string original;
        napi_env env_;
        napi_ref wrapper_;
};*/

#endif // NPDF_PDFMEMDOCUMENT_H
