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

#include "BaseDocument.h"
#include "../Defines.h"

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::string;
using std::vector;

namespace NoPoDoFo {
class Document
  : public Napi::ObjectWrap<Document>
  , public BaseDocument
{
public:
  static Napi::FunctionReference constructor;
  static PoDoFo::PdfFont* GetPdfFont(PoDoFo::PdfMemDocument&, string_view);
  static vector<PoDoFo::PdfFont*> GetFonts(PoDoFo::PdfMemDocument&);
  static void Initialize(Napi::Env& env, Napi::Object& target);
  static Napi::Value GC(const Napi::CallbackInfo&);

  explicit Document(const Napi::CallbackInfo&);
  ~Document();
  Napi::Value Load(const Napi::CallbackInfo&);
  Napi::Value CreatePage(const Napi::CallbackInfo&) override;
  void DeletePages(const Napi::CallbackInfo&);
  void SetPassword(const Napi::CallbackInfo&);
  Napi::Value Write(const Napi::CallbackInfo&);
  void SetEncrypt(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetEncrypt(const Napi::CallbackInfo&);
  Napi::Value GetTrailer(const Napi::CallbackInfo&);
  Napi::Value GetCatalog(const Napi::CallbackInfo&);
  Napi::Value InsertPages(const Napi::CallbackInfo&);
  Napi::Value HasSignature(const Napi::CallbackInfo&);
  Napi::Value GetFont(const Napi::CallbackInfo&);
  Napi::Value ListFonts(const Napi::CallbackInfo&);
  Napi::Value GetSignatures(const Napi::CallbackInfo&);
  bool LoadedForIncrementalUpdates() { return loadForIncrementalUpdates; }
  inline PoDoFo::PdfMemDocument& GetDocument()
  {
    return *dynamic_cast<PoDoFo::PdfMemDocument*>(Base);
  }

private:
  bool loadForIncrementalUpdates = false;
  vector<PoDoFo::PdfFont*> fonts;
  std::shared_ptr<spdlog::logger> dbglog;
};
}
#endif // NPDF_PDFMEMDOCUMENT_H
