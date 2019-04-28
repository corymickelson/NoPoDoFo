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
using JsValue = Napi::Value;
namespace NoPoDoFo {
using namespace std;
class TextField final
  : public Napi::ObjectWrap<TextField>
  , public Field
{
public:
  static Napi::FunctionReference Constructor;
  explicit TextField(const Napi::CallbackInfo&);
  ~TextField();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetText(const Napi::CallbackInfo&, const JsValue&);
  JsValue Text(const Napi::CallbackInfo&);
  void SetMaxLen(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetMaxLen(const Napi::CallbackInfo&);
  void SetMultiLine(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsMultiLine(const Napi::CallbackInfo&);
  void SetPasswordField(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsPasswordField(const Napi::CallbackInfo&);
  void SetFileField(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsFileField(const Napi::CallbackInfo&);
  void SetSpellcheckEnabled(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsSpellcheckEnabled(const Napi::CallbackInfo&);
  void SetScrollEnabled(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsScrollEnabled(const Napi::CallbackInfo&);
  void SetCombs(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsCombs(const Napi::CallbackInfo&);
  void SetRichText(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsRichText(const Napi::CallbackInfo&);
  void RefreshAppearanceStream(const Napi::CallbackInfo&);
  PoDoFo::PdfTextField GetText() const { return PoDoFo::PdfTextField(Self); }
  PoDoFo::PdfField& Self;
private:
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_TEXTFIELD_H
