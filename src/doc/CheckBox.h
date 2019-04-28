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

#ifndef NPDF_CHECKBOX_H
#define NPDF_CHECKBOX_H

#include "Button.h"
#include "Field.h"

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

using JsValue = Napi::Value;

namespace NoPoDoFo {
class CheckBox
  : public ObjectWrap<CheckBox>
  , public Field
  , public Button
{
public:
  explicit CheckBox(const CallbackInfo& callbackInfo);
	explicit CheckBox(const CheckBox&) = delete;
	const CheckBox&operator=(const CheckBox&) = delete;
  ~CheckBox();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue IsChecked(const CallbackInfo&);
  void SetChecked(const CallbackInfo&, const JsValue&);

  PoDoFo::PdfCheckBox GetCheckBox() { return PoDoFo::PdfCheckBox(FormField); }
  PoDoFo::PdfField& FormField;
};
}
#endif // NPDF_CHECKBOX_H
