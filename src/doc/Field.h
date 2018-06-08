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

#ifndef NPDF_PDFFIELD_H
#define NPDF_PDFFIELD_H

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>

using std::cout;
using std::endl;
using std::string;

namespace NoPoDoFo {
class Field
{
public:
  explicit Field(PoDoFo::EPdfField type, const Napi::CallbackInfo& info);
  Napi::Value GetType(const Napi::CallbackInfo&);
  Napi::Value GetFieldName(const Napi::CallbackInfo&);
  void SetFieldName(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetAlternateName(const Napi::CallbackInfo&);
  Napi::Value GetMappingName(const Napi::CallbackInfo&);
  void SetAlternateName(const Napi::CallbackInfo&, const Napi::Value&);
  void SetMappingName(const Napi::CallbackInfo&, const Napi::Value&);
  void SetRequired(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsRequired(const Napi::CallbackInfo&);
  void SetReadOnly(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsReadOnly(const Napi::CallbackInfo&);
  void SetExport(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsExport(const Napi::CallbackInfo&);
  Napi::Value SetBackground(const Napi::CallbackInfo&);
  Napi::Value SetBorder(const Napi::CallbackInfo&);
  Napi::Value SetHighlightingMode(const Napi::CallbackInfo&);
  Napi::Value SetMouseAction(const Napi::CallbackInfo&);
  Napi::Value SetPageAction(const Napi::CallbackInfo&);

  std::shared_ptr<PoDoFo::PdfField> GetField()
  {
    auto shared = field;
    return shared;
  }

  string fieldName;
  string fieldType;

protected:
  string TypeString();

private:
  std::shared_ptr<PoDoFo::PdfField> field;
};
}
#endif // NPDF_PDFFIELD_H
