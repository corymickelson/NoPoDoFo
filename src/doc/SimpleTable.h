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

#ifndef NPDF_SIMPLETABLE_H
#define NPDF_SIMPLETABLE_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>
using JsValue = Napi::Value;

namespace NoPoDoFo {

class SimpleTable : public Napi::ObjectWrap<SimpleTable>
{
public:
  explicit SimpleTable(const Napi::CallbackInfo&);
  explicit SimpleTable(const SimpleTable&) = delete;
  const SimpleTable& operator=(const SimpleTable&) = delete;
  ~SimpleTable();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env&, Napi::Object&);
  JsValue GetFont(const Napi::CallbackInfo&);
  void SetFont(const Napi::CallbackInfo&);
  JsValue GetText(const Napi::CallbackInfo&);
  void SetText(const Napi::CallbackInfo&);
  JsValue GetBorderWidth(const Napi::CallbackInfo&);
  void SetBorderWidth(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetBorderColor(const Napi::CallbackInfo&);
  void SetBorderEnabled(const Napi::CallbackInfo&);
  JsValue HasBorders(const Napi::CallbackInfo&);
  JsValue GetImage(const Napi::CallbackInfo&);
  JsValue HasImage(const Napi::CallbackInfo&);
  JsValue GetForegroundColor(const Napi::CallbackInfo&);
  void SetForegroundColor(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetBackgroundColor(const Napi::CallbackInfo&);
  void SetBackgroundColor(const Napi::CallbackInfo&, const JsValue&);
  JsValue HasBackgroundColor(const Napi::CallbackInfo&);
  void SetBackgroundEnabled(const Napi::CallbackInfo&);
  JsValue GetAlignment(const Napi::CallbackInfo&);
  void SetAlignment(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetVerticalAlignment(const Napi::CallbackInfo&);
  JsValue HasWordWrap(const Napi::CallbackInfo&);
  void SetWordWrapEnabled(const Napi::CallbackInfo&, const JsValue&);

  // PdfTable
  void Draw(const Napi::CallbackInfo&);
  JsValue GetWidth(const Napi::CallbackInfo&);
  void SetTableWidth(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetHeight(const Napi::CallbackInfo&);
  void SetTableHeight(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetCols(const Napi::CallbackInfo&);
  JsValue GetRows(const Napi::CallbackInfo&);
  void SetColumnWidths(const Napi::CallbackInfo&);
  void SetColumnWidth(const Napi::CallbackInfo&);
  void SetRowHeights(const Napi::CallbackInfo&);
  void SetRowHeight(const Napi::CallbackInfo&);
  void SetAutoPageBreak(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetAutoPageBreak(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfSimpleTableModel* Model = nullptr;
  PoDoFo::PdfTable* Table = nullptr;
  PoDoFo::PdfDocument* Doc = nullptr;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_SIMPLETABLE_H
