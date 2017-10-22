//
// Created by red on 9/6/17.
//

#ifndef NPDF_DOCUMENT_H
#define NPDF_DOCUMENT_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace std;

class Document : public Napi::ObjectWrap<Document>
{
public:
  explicit Document(const Napi::CallbackInfo& callbackInfo); // constructor
  ~Document() { delete document; }
  string originPdf;

  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value Load(const Napi::CallbackInfo&);
  Napi::Value LoadBuffer(const Napi::CallbackInfo&);
  Napi::Value GetPageCount(const Napi::CallbackInfo&);
  Napi::Value GetPage(const Napi::CallbackInfo&);
  void MergeDocument(const Napi::CallbackInfo&);
  void DeletePage(const Napi::CallbackInfo&);
  void SetPassword(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetVersion(const Napi::CallbackInfo&);
  Napi::Value IsLinearized(const Napi::CallbackInfo&);
  Napi::Value Write(const Napi::CallbackInfo&);
  Napi::Value GetWriteMode(const Napi::CallbackInfo&);
  void SetEncrypt(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetEncrypt(const Napi::CallbackInfo&);
  Napi::Value CreateEncrypt(const Napi::CallbackInfo&);
  Napi::Value GetObjects(const Napi::CallbackInfo&);
  Napi::Value Authenticate(const Napi::CallbackInfo&);
  Napi::Value GetTrailer(const Napi::CallbackInfo&);

  PoDoFo::PdfMemDocument* GetDocument() { return document; }

private:
  PoDoFo::PdfMemDocument* document;
  void ParseJsEncryptObj(const Napi::CallbackInfo& info,
                         Napi::Object& obj,
                         string& owner,
                         string& user,
                         int& pro,
                         int& algo,
                         int& key);
};

#endif // NPDF_PDFMEMDOCUMENT_H
