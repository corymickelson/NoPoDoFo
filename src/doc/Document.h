//
// Created by red on 9/6/17.
//

#ifndef NPDF_DOCUMENT_H
#define NPDF_DOCUMENT_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace std;

namespace NoPoDoFo {
class Document : public Napi::ObjectWrap<Document>
{
public:
  static Napi::FunctionReference constructor;
  explicit Document(const Napi::CallbackInfo& callbackInfo); // constructor
  ~Document();
  string originPdf;

  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value Load(const Napi::CallbackInfo&);
  Napi::Value GetPageCount(const Napi::CallbackInfo&);
  Napi::Value GetPage(const Napi::CallbackInfo&);
  void MergeDocument(const Napi::CallbackInfo&);
  void DeletePage(const Napi::CallbackInfo&);
  void SetPassword(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetVersion(const Napi::CallbackInfo&);
  Napi::Value IsLinearized(const Napi::CallbackInfo&);
  Napi::Value Write(const Napi::CallbackInfo&);
  Napi::Value WriteBuffer(const Napi::CallbackInfo&);
  Napi::Value GetWriteMode(const Napi::CallbackInfo&);
  void SetEncrypt(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetEncrypt(const Napi::CallbackInfo&);
  Napi::Value GetObjects(const Napi::CallbackInfo&);
  Napi::Value GetTrailer(const Napi::CallbackInfo&);
  Napi::Value IsAllowed(const Napi::CallbackInfo&);
  Napi::Value CreateFont(const Napi::CallbackInfo&);
  static Napi::Value GC(const Napi::CallbackInfo&);

  PoDoFo::PdfMemDocument* GetDocument() { return document; }
  bool LoadedForIncrementalUpdates() { return loadForIncrementalUpdates; }

private:
  bool loadForIncrementalUpdates = false;
  PoDoFo::PdfMemDocument* document;
  bool isExternalInstance = false;
};
}
#endif // NPDF_PDFMEMDOCUMENT_H
