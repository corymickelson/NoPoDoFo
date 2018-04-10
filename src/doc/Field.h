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

#include "../ValidateArguments.h"
#include "Page.h"
#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Field : public Napi::ObjectWrap<Field>
{
public:
  explicit Field(const Napi::CallbackInfo& info);
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  Napi::Value GetType(const Napi::CallbackInfo&);
  Napi::Value GetFieldName(const Napi::CallbackInfo&);
  Napi::Value GetAlternateName(const Napi::CallbackInfo&);
  Napi::Value GetMappingName(const Napi::CallbackInfo&);
  void SetAlternateName(const Napi::CallbackInfo&);
  void SetMappingName(const Napi::CallbackInfo&);
  void SetRequired(const Napi::CallbackInfo&);
  Napi::Value IsRequired(const Napi::CallbackInfo&);
  void SetReadOnly(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsReadOnly(const Napi::CallbackInfo&);

  PoDoFo::PdfField& GetField() { return *field; }

private:
  std::unique_ptr<PoDoFo::PdfField> field;
};
}
#endif // NPDF_PDFFIELD_H
