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

using JsValue = Napi::Value;

namespace NoPoDoFo {
class Document
  : public Napi::ObjectWrap<Document>
  , public BaseDocument
{
public:
  static Napi::FunctionReference Constructor;
  static PoDoFo::PdfFont* GetPdfFont(PoDoFo::PdfMemDocument&, string_view);
  static vector<PoDoFo::PdfFont*> GetFonts(PoDoFo::PdfMemDocument&);
  static void Initialize(Napi::Env& env, Napi::Object& target);
  static JsValue GC(const Napi::CallbackInfo&);

	explicit Document(const Napi::CallbackInfo&);
  explicit Document(const Document&) = delete;
  const Document&operator=(const Document&) = delete;
  ~Document();
  JsValue Load(const Napi::CallbackInfo&);
  JsValue CreatePage(const Napi::CallbackInfo&) override;
  void DeletePages(const Napi::CallbackInfo&);
  void SetPassword(const Napi::CallbackInfo&);
  JsValue Write(const Napi::CallbackInfo&);
  void SetEncrypt(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetEncrypt(const Napi::CallbackInfo&);
  JsValue GetTrailer(const Napi::CallbackInfo&);
  JsValue GetCatalog(const Napi::CallbackInfo&);
  JsValue InsertPages(const Napi::CallbackInfo&);
  JsValue HasSignature(const Napi::CallbackInfo&);
  JsValue GetFont(const Napi::CallbackInfo&);
  JsValue ListFonts(const Napi::CallbackInfo&);
  JsValue GetSignatures(const Napi::CallbackInfo&);
  bool LoadedForIncrementalUpdates() const { return LoadForIncrementalUpdates; }
  inline PoDoFo::PdfMemDocument& GetDocument() const
  {
    return *dynamic_cast<PoDoFo::PdfMemDocument*>(Base);
  }

private:
  bool LoadForIncrementalUpdates = false;
  vector<PoDoFo::PdfFont*> Fonts;
};
}
#endif // NPDF_PDFMEMDOCUMENT_H
