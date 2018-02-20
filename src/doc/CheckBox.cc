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

#include "CheckBox.h"

namespace NoPoDoFo {
FunctionReference CheckBox::constructor;

CheckBox::CheckBox(const CallbackInfo& info)
  : ObjectWrap<CheckBox>(info)
{
  if (info.Length() == 0) {
    throw Napi::Error::New(info.Env(),
                           "Requires a single argument of type Field");
  }
  auto fieldObj = info[0].As<Object>();
  Field* field = Field::Unwrap(fieldObj);
  PdfCheckBox v(field->GetField());
  box = make_unique<PdfCheckBox>(v);
}

void CheckBox::Initialize(Napi::Env &env, Napi::Object &target) {
    HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "CheckBox",
                  { InstanceAccessor(
                    "checked", &CheckBox::IsChecked, &CheckBox::SetChecked) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();

    target.Set("CheckBox", ctor);
  }
Napi::Value
CheckBox::IsChecked(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), box->IsChecked());
}

void
CheckBox::SetChecked(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::TypeError::New(info.Env(),
                               "CheckBox.checked requires boolean value");
  }
  bool checked = value.As<Boolean>();
  box->SetChecked(checked);
}

}
