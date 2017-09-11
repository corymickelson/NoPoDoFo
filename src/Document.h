//
// Created by red on 9/6/17.
//

#ifndef NPDF_PDFMEMDOCUMENT_H
#define NPDF_PDFMEMDOCUMENT_H

#include <boost/filesystem.hpp>
#include <napi.h>
#include <podofo/podofo.h>

#include "Page.h"

using namespace boost;
using namespace Napi;
using namespace std;

class Document : public Napi::ObjectWrap<Document>
{
public:
  explicit Document(const CallbackInfo &callbackInfo); // constructor
  ~Document()
  {
    free(_buffer);
    delete _document;
  }
  string originPdf;

  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    Napi::HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "Document",
                  {InstanceMethod("load", &Document::Load),
                   InstanceMethod("getPageCount", &Document::GetPageCount),
                   InstanceMethod("getPage", &Document::GetPage),
                   InstanceMethod("setPassword", &Document::SetPassword),
                   InstanceMethod("getVersion", &Document::GetVersion),
                   InstanceMethod("isLinearized", &Document::IsLinearized),
                   InstanceMethod("write", &Document::Write)});

    target.Set("Document", ctor);
  }

  Napi::Value
  Load(const CallbackInfo &);
  Napi::Value
  GetPageCount(const CallbackInfo &);
  Napi::Value
  GetPage(const CallbackInfo &);
  void
  SetPassword(const CallbackInfo &);
  Napi::Value
  GetVersion(const CallbackInfo &);
  Napi::Value
  IsLinearized(const CallbackInfo &);
  Napi::Value
  Write(const CallbackInfo &);

private:
  PoDoFo::PdfMemDocument *_document;
  void *_buffer;
};

#endif // NPDF_PDFMEMDOCUMENT_H
