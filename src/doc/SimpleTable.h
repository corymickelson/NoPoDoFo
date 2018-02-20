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

#ifndef NPDF_SIMPLETABLE_H
#define NPDF_SIMPLETABLE_H

#include "Document.h"

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {

class SimpleTable : public Napi::ObjectWrap<SimpleTable>
{
public:
  explicit SimpleTable(const Napi::CallbackInfo& callbackInfo);
  ~SimpleTable();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env&, Napi::Object&);
  Napi::Value GetFont(const Napi::CallbackInfo&);
  void SetFont(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetText(const Napi::CallbackInfo&);
  void SetText(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetBorderWidth(const Napi::CallbackInfo&);
  void SetBorderWidth(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetBorderColor(const Napi::CallbackInfo&);
  void SetBorderEnabled(const Napi::CallbackInfo&);
  Napi::Value HasBorders(const Napi::CallbackInfo&);
  Napi::Value GetImage(const Napi::CallbackInfo&);
  Napi::Value HasImage(const Napi::CallbackInfo&);
  Napi::Value GetForegroundColor(const Napi::CallbackInfo&);
  void SetForegroundColor(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetBackgroundColor(const Napi::CallbackInfo&);
  void SetBackgroundColor(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value HasBackgroundColor(const Napi::CallbackInfo&);
  void SetBackgroundEnabled(const Napi::CallbackInfo&);
  Napi::Value GetAlignment(const Napi::CallbackInfo&);
  void SetAlignment(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetVerticalAlignment(const Napi::CallbackInfo&);
  Napi::Value HasWordWrap(const Napi::CallbackInfo&);
  void SetWordWrapEnabled(const Napi::CallbackInfo&, const Napi::Value&);

  // PdfTable
  void Draw(const Napi::CallbackInfo&);
  Napi::Value GetWidth(const Napi::CallbackInfo&);
  void SetTableWidth(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetHeight(const Napi::CallbackInfo&);
  void SetTableHeight(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetCols(const Napi::CallbackInfo&);
  Napi::Value GetRows(const Napi::CallbackInfo&);
  void SetColumnWidths(const Napi::CallbackInfo&);
  void SetColumnWidth(const Napi::CallbackInfo&);
  void SetRowHeights(const Napi::CallbackInfo&);
  void SetRowHeight(const Napi::CallbackInfo&);
  void SetAutoPageBreak(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetAutoPageBreak(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfSimpleTableModel* model = nullptr;
  PoDoFo::PdfTable* table = nullptr;
  Document* doc = nullptr;
  PoDoFo::PdfColor* backgroundColor = nullptr;
  PoDoFo::PdfColor* foregroundColor = nullptr;

  PoDoFo::PdfColor* SetColor(Napi::Array&);
  void GetColor(PoDoFo::PdfColor&, Napi::Array& js);
};
}
#endif // NPDF_SIMPLETABLE_H
