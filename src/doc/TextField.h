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

#ifndef NPDF_TEXTFIELD_H
#define NPDF_TEXTFIELD_H

#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>

namespace NoPoDoFo {
using namespace std;
class TextField
  : public Napi::ObjectWrap<TextField>
  , public Field
{
public:
  static Napi::FunctionReference constructor;
  explicit TextField(const Napi::CallbackInfo&);
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetText(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value Text(const Napi::CallbackInfo&);
  void SetMaxLen(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetMaxLen(const Napi::CallbackInfo&);
  void SetMultiLine(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsMultiLine(const Napi::CallbackInfo&);
  void SetPasswordField(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsPasswordField(const Napi::CallbackInfo&);
  void SetFileField(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsFileField(const Napi::CallbackInfo&);
  void SetSpellcheckEnabled(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsSpellcheckEnabled(const Napi::CallbackInfo&);
  void SetScrollEnabled(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsScrollEnabled(const Napi::CallbackInfo&);
  void SetCombs(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsCombs(const Napi::CallbackInfo&);
  void SetRichText(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsRichText(const Napi::CallbackInfo&);

  PoDoFo::PdfTextField GetText() { return PoDoFo::PdfTextField(field); }
  PoDoFo::PdfField& field;
};
}
#endif // NPDF_TEXTFIELD_H
