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

#include "ListField.h"
#include "Field.h"
#include <iostream>

using namespace PoDoFo;
using namespace Napi;

namespace NoPoDoFo {

ListField::ListField(PdfField& field)
  : field(field)
{}

void
ListField::InsertItem(const CallbackInfo& info)
{
  string value = info[0].As<String>().Utf8Value();
  string display = info[1].As<String>().Utf8Value();
  GetListField().InsertItem(PdfString(value), PdfString(display));
}

void
ListField::RemoveItem(const CallbackInfo& info)
{
  int index = info[0].As<Number>();
  if (static_cast<size_t>(index) > GetListField().GetItemCount() || index < 0) {
    throw Napi::Error::New(info.Env(), "index out of range");
  }
  GetListField().RemoveItem(index);
}

Napi::Value
ListField::GetItem(const CallbackInfo& info)
{
  int index = info[0].As<Number>();
  Object item = Object::New(info.Env());
  string value = GetListField().GetItem(index).GetStringUtf8();
  string display = GetListField().GetItemDisplayText(index).GetStringUtf8();
  item.Set(String::New(info.Env(), "value"), String::New(info.Env(), value));
  item.Set(String::New(info.Env(), "display"),
           String::New(info.Env(), display));
  return item;
}

Napi::Value
ListField::GetItemCount(const CallbackInfo& info)
{
  return Number::New(info.Env(), GetListField().GetItemCount());
}

void
ListField::SetSelectedItem(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "index must be of type number");
  }
  GetListField().SetSelectedItem(value.As<Number>());
}

Napi::Value
ListField::GetSelectedItem(const CallbackInfo& info)
{
  int index = GetListField().GetSelectedItem();
  return Number::New(info.Env(), index);
}
Napi::Value
ListField::IsComboBox(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetListField().IsComboBox());
}
void
ListField::SetSpellCheckEnabled(const Napi::CallbackInfo&,
                                const Napi::Value& value)
{
  GetListField().SetSpellcheckingEnabled(value.As<Boolean>());
}
Napi::Value
ListField::IsSpellCheckEnabled(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetListField().IsSpellcheckingEnabled());
}
void
ListField::SetSorted(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetListField().SetSorted(value.As<Boolean>());
}
Napi::Value
ListField::IsSorted(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetListField().IsSorted());
}
void
ListField::SetMultiSelect(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetListField().SetMultiSelect(value.As<Boolean>());
}
Napi::Value
ListField::IsMultiSelect(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetListField().IsMultiSelect());
}
}
