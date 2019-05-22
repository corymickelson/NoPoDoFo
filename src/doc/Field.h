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

#ifndef NPDF_PDFFIELD_H
#define NPDF_PDFFIELD_H

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>
#include "../Defines.h" 

using std::cout;
using std::endl;
using std::string;
using std::vector;
using JsValue = Value;

namespace NoPoDoFo {

typedef struct AppearanceCtx
{
  PdfDocument* Doc;
  PdfFont* Font;
  int FontSize;
  int PageIndex;
} AppearanceCtx;
class Field
{
public:
  explicit Field(EPdfField type, const CallbackInfo& info);
	explicit Field(const Field&) = delete;
	const Field&operator=(const Field&) = delete;
  virtual ~Field();
  JsValue GetType(const CallbackInfo&);
  JsValue GetFieldName(const CallbackInfo&);
  void SetFieldName(const CallbackInfo&, const Value&);
  JsValue GetAlternateName(const CallbackInfo&);
  JsValue GetMappingName(const CallbackInfo&);
  void SetAlternateName(const CallbackInfo&, const Value&);
  void SetMappingName(const CallbackInfo&, const Value&);
  void SetRequired(const CallbackInfo&, const Value&);
  JsValue IsRequired(const CallbackInfo&);
  void SetReadOnly(const CallbackInfo&, const Value&);
  JsValue IsReadOnly(const CallbackInfo&);
  void SetExport(const CallbackInfo&, const Value&);
  JsValue IsExport(const CallbackInfo&);
  void SetBackground(const CallbackInfo&);
  void SetBorder(const CallbackInfo&);
  void SetHighlightingMode(const CallbackInfo&);
  void SetMouseAction(const CallbackInfo&);
  void SetPageAction(const CallbackInfo&);
  JsValue GetAnnotation(const CallbackInfo&);
  JsValue GetAppearanceStream(const CallbackInfo&);
  void SetAppearanceStream(const CallbackInfo&, const Value&);
  JsValue GetDefaultAppearance(const CallbackInfo&);
  void SetDefaultAppearance(const CallbackInfo&, const Value&);
  JsValue GetJustification(const CallbackInfo&);
  void SetJustification(const CallbackInfo&, const Value&);
  JsValue GetFieldObject(const CallbackInfo&);
  virtual void RefreshAppearanceStream();
  PdfField& GetField() const { return *Self; }
  PdfDictionary& GetFieldDictionary() const
  {
    return Self->GetFieldObject()->GetDictionary();
  }
  std::map<std::string, PdfObject*> GetFieldRefreshKeys(
    PdfField*);
  PdfFont* GetDAFont(string_view);
  string FieldName;
  string FieldType;

protected:
  string TypeString();
	std::shared_ptr<spdlog::logger> DbgLog;
private:
  PdfField* Self;
  vector<PdfObject*> Children;
};
}
#endif // NPDF_PDFFIELD_H
