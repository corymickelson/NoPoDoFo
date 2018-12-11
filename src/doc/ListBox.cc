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

#include "ListBox.h"
#include "Field.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference ListBox::constructor; // NOLINT

ListBox::ListBox(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , Field(ePdfField_ListBox, info)
  , ListField(Field::GetField())
  , field(Field::GetField())
{}

void
ListBox::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
    env,
    "ListBox",
    { InstanceAccessor(
        "selected", &ListBox::GetSelectedItem, &ListBox::SetSelectedItem),
      InstanceAccessor("length", &ListBox::GetItemCount, nullptr),
      InstanceAccessor("widgetAnnotation", &ListBox::GetAnnotation, nullptr),
      InstanceAccessor("AP", &ListBox::GetAppearanceStream, &ListBox::SetAppearanceStream),
      InstanceAccessor("DA", &ListBox::GetDefaultAppearance, &ListBox::SetDefaultAppearance),
      InstanceAccessor("spellCheckEnabled",
                       &ListBox::IsSpellCheckEnabled,
                       &ListBox::SetSpellCheckEnabled),
      InstanceAccessor("sorted", &ListBox::IsSorted, &ListBox::SetSorted),
      InstanceAccessor(
        "multiSelect", &ListBox::IsMultiSelect, &ListBox::SetMultiSelect),
      InstanceMethod("isComboBox", &ListBox::IsComboBox),
      InstanceMethod("insertItem", &ListBox::InsertItem),
      InstanceMethod("removeItem", &ListBox::RemoveItem),
      InstanceMethod("getItem", &ListBox::GetItem),
      InstanceAccessor("readOnly", &ListBox::IsReadOnly, &ListBox::SetReadOnly),
      InstanceAccessor("required", &ListBox::IsRequired, &ListBox::SetRequired),
      InstanceAccessor("exported", &ListBox::IsExport, &ListBox::SetExport),
      InstanceAccessor("type", &ListBox::GetType, nullptr),
      InstanceAccessor(
        "fieldName", &ListBox::GetFieldName, &ListBox::SetFieldName),
      InstanceAccessor("alternateName",
                       &ListBox::GetAlternateName,
                       &ListBox::SetAlternateName),
      InstanceAccessor(
        "mappingName", &ListBox::GetMappingName, &ListBox::SetMappingName),
      InstanceMethod("setBackgroundColor", &ListBox::SetBackground),
      InstanceMethod("setBorderColor", &ListBox::SetBorder),
      InstanceMethod("setMouseAction", &ListBox::SetMouseAction),
      InstanceMethod("setPageAction", &ListBox::SetPageAction),
      InstanceMethod("setHighlightingMode", &ListBox::SetHighlightingMode) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("ListBox", ctor);
}
}
