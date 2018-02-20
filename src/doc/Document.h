/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 * 
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



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
};
}
#endif // NPDF_PDFMEMDOCUMENT_H
