//
// Created by red on 9/6/17.
//

#include "pdfDocument.h"

/*
This is using the Node C Api

napi_ref PdfMemDocument::constructor;

PdfMemDocument::PdfMemDocument( const string & file )
                : original( file ), env_( nullptr ), wrapper_( nullptr ) {}

PdfMemDocument::~PdfMemDocument() { napi_delete_reference( env_, wrapper_ ); }

void PdfMemDocument::Destructor( napi_env env, void * obj, void * finalize ) {
        reinterpret_cast<PdfMemDocument *>(obj)->~PdfMemDocument();
}

void PdfMemDocument::Init( napi_env env, napi_value exports ) {
        napi_status status;
        napi_property_descriptor properties[] = {

        };
        napi_value cons;
        if (( status = napi_define_class( env, "PdfMemDocument", New,
                                                                          nullptr,
0, properties, &cons )) != napi_ok ) {}
        if (( status = napi_create_reference( env, cons, 1, &constructor )) !=
                napi_ok ) {}
        if (( status = napi_set_named_property( env, exports, "PdfMemDocument",
                                                                                        cons )) != napi_ok ) {}
}

napi_value PdfMemDocument::New( napi_env env, napi_callback_info info ) {
        napi_status status;
        bool isConstructor;
        if (( status = napi_is_construct_call( env, info, &isConstructor )) !=
                napi_ok ) {}
        // invoke as constructor
        if ( isConstructor ) {
                size_t argc = 1;
                napi_value args[argc];
                napi_value js;

                if (( status = napi_get_cb_info( env, info, &argc, args, &js,
                                                                                 nullptr )) != napi_ok ) {}
                napi_valuetype vType;
                size_t originValueResultOut;
                if (( status = napi_typeof( env, args[ 0 ], &vType )) != napi_ok
) {}
                if ( vType == napi_undefined ) {}
                size_t  strSize;
                if (( status = napi_get_value_string_utf8( env, args[ 0 ],
                                                                                                   nullptr, nullptr, &strSize))) {}

                char originValue[strSize];
                if (( status = napi_get_value_string_utf8( env, args[ 0 ],
                                                                                                   originValue, strSize , &originValueResultOut ))) {}
        }
}*/

pdfDocument::pdfDocument(const CallbackInfo &callbackInfo)
    : ObjectWrap(callbackInfo) {}

Napi::Value pdfDocument::Load(const CallbackInfo &info) {
  string filePath = info[0].As<String>().Utf8Value();
  originPdf = filePath;
  //  void* document = malloc(sizeof(PoDoFo::PdfMemDocument)); // does this need
  //  to be allocated first?
  //  PoDoFo::PdfMemDocument x("");
  //  memcpy(document,&x, sizeof(PoDoFo::PdfMemDocument));
  PoDoFo::PdfMemDocument document(filePath.c_str());
  _document = &document;
}

Napi::Value pdfDocument::GetPageCount(const CallbackInfo &info) {
  return Napi::Number::New(info.Env(), _document->GetPageCount());
}

Napi::Value pdfDocument::GetPage(const CallbackInfo &info) { return Value(); }

void pdfDocument::Write(const CallbackInfo &info) {
  string destinationFile = info[0].As<String>().Utf8Value();
  PoDoFo::PdfOutputDevice device(destinationFile.c_str());
  _document->Write(&device);
}

void pdfDocument::SetPassword(const CallbackInfo &info) {
  if (info.Length() < 1) {
  }
  string password = info[0].As<String>().Utf8Value();
  _document->SetPassword(password.c_str());
}

Napi::Value pdfDocument::GetVersion(const CallbackInfo &info) {
  PoDoFo::EPdfVersion versionE = _document->GetPdfVersion();
  double v;
  switch (versionE) {
  case PoDoFo::ePdfVersion_1_1:
    v = 1.1;
    break;
  case PoDoFo::ePdfVersion_1_3:
    v = 1.3;
    break;
  case PoDoFo::ePdfVersion_1_0:
    v = 1.0;
    break;
  case PoDoFo::ePdfVersion_1_2:
    v = 1.2;
    break;
  case PoDoFo::ePdfVersion_1_4:
    v = 1.4;
    break;
  case PoDoFo::ePdfVersion_1_5:
    v = 1.5;
    break;
  case PoDoFo::ePdfVersion_1_6:
    v = 1.6;
    break;
  case PoDoFo::ePdfVersion_1_7:
    v = 1.7;
    break;
  }
  return Napi::Number::New(info.Env(), v);
}

Napi::Value pdfDocument::IsLinearized(const CallbackInfo &info) {
  CheckInfoArgsLength(info, 1);
  return Napi::Boolean::New(info.Env(), _document->IsLinearized());
}
