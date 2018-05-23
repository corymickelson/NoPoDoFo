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

#include <iostream>
#include "CheckBox.h"
#include "Field.h"

namespace NoPoDoFo {
FunctionReference CheckBox::constructor; // NOLINT

using std::cout;
using std::endl;

CheckBox::CheckBox(const CallbackInfo& info)
  : ObjectWrap<CheckBox>(info)
{
  field = Field::Unwrap(info[0].As<Object>())->GetField();
}


void
CheckBox::Initialize(Napi::Env& env, Napi::Object& target)
{
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
  return Napi::Boolean::New(info.Env(), GetField().IsChecked());
}

void
CheckBox::SetChecked(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::TypeError::New(info.Env(),
                               "CheckBox.checked requires boolean value");
  }
  bool checked = value.As<Boolean>();
  GetField().SetChecked(checked);
}
}
