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

#include "ComboBox.h"
#include "Field.h"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;

namespace NoPoDoFo {

FunctionReference ComboBox::Constructor; // NOLINT

ComboBox::ComboBox(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , Field(ePdfField_ComboBox, info)
  , ListField(Field::GetField())
  , FormField(Field::GetField())
{
}
ComboBox::~ComboBox()
{
  Field::DbgLog->debug("ComboBox Cleanup");
}
void
ComboBox::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
    env,
    "ComboBox",
    { InstanceAccessor(
        "editable", &ComboBox::GetEditable, &ComboBox::SetEditable),
      InstanceAccessor(
        "AP", &ComboBox::GetAppearanceStream, &ComboBox::SetAppearanceStream),
      InstanceAccessor(
        "DA", &ComboBox::GetDefaultAppearance, &ComboBox::SetDefaultAppearance),

      InstanceAccessor("obj", &Field::GetFieldObject, nullptr),
      InstanceAccessor(
        "selected", &ComboBox::GetSelectedItem, &ComboBox::SetSelectedItem),
      InstanceAccessor("widgetAnnotation", &ComboBox::GetAnnotation, nullptr),
      InstanceAccessor("length", &ComboBox::GetItemCount, nullptr),
      InstanceAccessor("spellCheckEnabled",
                       &ComboBox::IsSpellCheckEnabled,
                       &ComboBox::SetSpellCheckEnabled),
      InstanceAccessor("sorted", &ComboBox::IsSorted, &ComboBox::SetSorted),
      InstanceAccessor(
        "multiSelect", &ComboBox::IsMultiSelect, &ComboBox::SetMultiSelect),
      InstanceAccessor(
        "readOnly", &ComboBox::IsReadOnly, &ComboBox::SetReadOnly),
      InstanceAccessor(
        "required", &ComboBox::IsRequired, &ComboBox::SetRequired),
      InstanceAccessor("exported", &ComboBox::IsExport, &ComboBox::SetExport),
      InstanceAccessor("type", &ComboBox::GetType, nullptr),
      InstanceAccessor(
        "fieldName", &ComboBox::GetFieldName, &ComboBox::SetFieldName),
      InstanceAccessor("alternateName",
                       &ComboBox::GetAlternateName,
                       &ComboBox::SetAlternateName),
      InstanceAccessor(
        "mappingName", &ComboBox::GetMappingName, &ComboBox::SetMappingName),
      InstanceMethod("isComboBox", &ComboBox::IsComboBox),
      InstanceMethod("insertItem", &ComboBox::InsertItem),
      InstanceMethod("removeItem", &ComboBox::RemoveItem),
      InstanceMethod("getItem", &ComboBox::GetItem),

      InstanceMethod("setBackgroundColor", &ComboBox::SetBackground),
      InstanceMethod("setBorderColor", &ComboBox::SetBorder),
      InstanceMethod("setMouseAction", &ComboBox::SetMouseAction),
      InstanceMethod("setPageAction", &ComboBox::SetPageAction),
      InstanceMethod("setHighlightingMode", &ComboBox::SetHighlightingMode)

    });
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("ComboBox", ctor);
}
void
ComboBox::SetEditable(const Napi::CallbackInfo&, const JsValue& value)
{
  GetComboBox().SetEditable(value.As<Napi::Boolean>());
}
JsValue
ComboBox::GetEditable(const Napi::CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetComboBox().IsEditable());
}
}
