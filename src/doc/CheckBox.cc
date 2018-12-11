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
#include "Field.h"
#include <iostream>

using std::cout;
using std::endl;
using std::make_unique;

namespace NoPoDoFo {

FunctionReference CheckBox::constructor; // NOLINT

CheckBox::CheckBox(const CallbackInfo& info)
  : ObjectWrap<CheckBox>(info)
  , Field(ePdfField_CheckBox, info)
  , Button(Field::GetField())
  , field(Field::GetField())
{}

void
CheckBox::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Checkbox",
    { InstanceAccessor("checked", &CheckBox::IsChecked, &CheckBox::SetChecked),
      InstanceAccessor("AP", &CheckBox::GetAppearanceStream, &CheckBox::SetAppearanceStream),
      InstanceAccessor("DA", &CheckBox::GetDefaultAppearance, &CheckBox::SetDefaultAppearance),
      InstanceAccessor(
        "readOnly", &CheckBox::IsReadOnly, &CheckBox::SetReadOnly),
        InstanceAccessor("widgetAnnotation", &CheckBox::GetAnnotation, nullptr),
      InstanceAccessor(
        "required", &CheckBox::IsRequired, &CheckBox::SetRequired),
      InstanceAccessor("exported", &CheckBox::IsExport, &CheckBox::SetExport),
      InstanceAccessor("type", &CheckBox::GetType, nullptr),
      InstanceAccessor(
        "fieldName", &CheckBox::GetFieldName, &CheckBox::SetFieldName),
      InstanceAccessor("alternateName",
                       &CheckBox::GetAlternateName,
                       &CheckBox::SetAlternateName),
      InstanceAccessor(
        "mappingName", &CheckBox::GetMappingName, &CheckBox::SetMappingName),
      InstanceAccessor("caption", &CheckBox::GetCaption, &CheckBox::SetCaption),
      InstanceMethod("setBackgroundColor", &CheckBox::SetBackground),
      InstanceMethod("setBorderColor", &CheckBox::SetBorder),
      InstanceMethod("setMouseAction", &CheckBox::SetMouseAction),
      InstanceMethod("setPageAction", &CheckBox::SetPageAction),
      InstanceMethod("setHighlightingMode", &CheckBox::SetHighlightingMode) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Checkbox", ctor);
}
Napi::Value
CheckBox::IsChecked(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetCheckBox().IsChecked());
}

void
CheckBox::SetChecked(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::TypeError::New(info.Env(),
                               "CheckBox.checked requires boolean value");
  }
  bool checked = value.As<Boolean>();
  GetCheckBox().SetChecked(checked);
}
}
