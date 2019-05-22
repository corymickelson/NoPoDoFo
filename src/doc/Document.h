/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2019
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

using JsValue = Value;

namespace NoPoDoFo {
class Document final
  : public ObjectWrap<Document>
  , public BaseDocument
{
public:
  static FunctionReference Constructor;
  static PdfFont* GetPdfFont(PdfMemDocument&, string_view);
  static vector<PdfFont*> GetFonts(PdfMemDocument&);
  static void Initialize(Napi::Env& env, Object& target);
  static JsValue GC(const CallbackInfo&);

	explicit Document(const CallbackInfo&);
  explicit Document(const Document&) = delete;
  const Document&operator=(const Document&) = delete;
  ~Document();
  JsValue Load(const CallbackInfo&);
  JsValue CreatePage(const CallbackInfo&) override;
  void DeletePages(const CallbackInfo&);
  void SetPassword(const CallbackInfo&);
  JsValue Write(const CallbackInfo&);
  void SetEncrypt(const CallbackInfo&, const JsValue&);
  JsValue GetEncrypt(const CallbackInfo&);
  JsValue GetTrailer(const CallbackInfo&);
  JsValue GetCatalog(const CallbackInfo&);
  JsValue InsertPages(const CallbackInfo&);
  JsValue HasSignature(const CallbackInfo&);
  JsValue GetFont(const CallbackInfo&);
  JsValue ListFonts(const CallbackInfo&);
  JsValue GetSignatures(const CallbackInfo&);
  bool LoadedForIncrementalUpdates() const { return LoadForIncrementalUpdates; }
  inline PdfMemDocument& GetDocument() const
  {
    return *dynamic_cast<PdfMemDocument*>(Base);
  }

private:
  bool LoadForIncrementalUpdates = false;
  vector<PdfFont*> Fonts;
};
}
#endif // NPDF_PDFMEMDOCUMENT_H
