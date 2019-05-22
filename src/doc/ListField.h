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

#ifndef NPDF_LISTFIELD_H
#define NPDF_LISTFIELD_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {
class ListField
{
public:
  explicit ListField(PoDoFo::PdfField&);
  explicit ListField(const ListField&) = delete;
  const ListField& operator=(const ListField&) = delete;
  ~ListField();
  void InsertItem(const Napi::CallbackInfo&);
  void RemoveItem(const Napi::CallbackInfo&);
  JsValue GetItem(const Napi::CallbackInfo&);
  JsValue GetItemCount(const Napi::CallbackInfo&);
  void SetSelectedItem(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetSelectedItem(const Napi::CallbackInfo&);
  JsValue IsComboBox(const Napi::CallbackInfo&);
  void SetSpellCheckEnabled(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsSpellCheckEnabled(const Napi::CallbackInfo&);
  void SetSorted(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsSorted(const Napi::CallbackInfo&);
  void SetMultiSelect(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsMultiSelect(const Napi::CallbackInfo&);
  PoDoFo::PdfListField GetListField() { return PoDoFo::PdfListField(Self); }

private:
  PoDoFo::PdfField& Self;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
