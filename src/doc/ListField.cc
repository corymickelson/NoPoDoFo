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

using namespace PoDoFo;
using namespace Napi;

namespace NoPoDoFo {

FunctionReference ListField::constructor; // NOLINT

ListField::ListField(const CallbackInfo& info)
  : ObjectWrap(info)
  , field(Field::Unwrap(info[0].As<Object>()))
{}
ListField::~ListField()
{
  HandleScope scope(Env());
  field = nullptr;
}
void
ListField::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "ListField",
    { InstanceAccessor(
        "selected", &ListField::GetSelectedItem, &ListField::SetSelectedItem),
      InstanceAccessor("length", &ListField::GetItemCount, nullptr),
      InstanceAccessor("spellCheckEnabled",
                       &ListField::IsSpellCheckEnabled,
                       &ListField::SetSpellCheckEnabled),
      InstanceAccessor("sorted", &ListField::IsSorted, &ListField::SetSorted),
      InstanceAccessor(
        "multiSelect", &ListField::IsMultiSelect, &ListField::SetMultiSelect),
      InstanceMethod("isComboBox", &ListField::IsComboBox),
      InstanceMethod("insertItem", &ListField::InsertItem),
      InstanceMethod("removeItem", &ListField::RemoveItem),
      InstanceMethod("getItem", &ListField::GetItem) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("ListField", ctor);
}

void
ListField::InsertItem(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_string });
  string value = info[0].As<String>().Utf8Value();
  string display = info[1].As<String>().Utf8Value();
  GetField().InsertItem(PdfString(value), PdfString(display));
}

void
ListField::RemoveItem(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  if (static_cast<size_t>(index) > GetField().GetItemCount() || index < 0) {
    throw Napi::Error::New(info.Env(), "index out of range");
  }
  GetField().RemoveItem(index);
}

Napi::Value
ListField::GetItem(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  Object item = Object::New(info.Env());
  string value = GetField().GetItem(index).GetString();
  string display = GetField().GetItemDisplayText(index).GetString();
  item.Set(String::New(info.Env(), "value"), String::New(info.Env(), value));
  item.Set(String::New(info.Env(), "display"),
           String::New(info.Env(), display));
  return item;
}

Napi::Value
ListField::GetItemCount(const CallbackInfo& info)
{
  return Number::New(info.Env(), GetField().GetItemCount());
}

void
ListField::SetSelectedItem(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "index must be of type number");
  }
  GetField().SetSelectedItem(value.As<Number>());
}

Napi::Value
ListField::GetSelectedItem(const CallbackInfo& info)
{
  int index = GetField().GetSelectedItem();
  return Number::New(info.Env(), index);
}
Napi::Value
ListField::IsComboBox(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetField().IsComboBox());
}
void
ListField::SetSpellCheckEnabled(const Napi::CallbackInfo&,
                                const Napi::Value& value)
{
  GetField().SetSpellcheckingEnabled(value.As<Boolean>());
}
Napi::Value
ListField::IsSpellCheckEnabled(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetField().IsSpellcheckingEnabled());
}
void
ListField::SetSorted(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetField().SetSorted(value.As<Boolean>());
}
Napi::Value
ListField::IsSorted(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetField().IsSorted());
}
void
ListField::SetMultiSelect(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetField().SetMultiSelect(value.As<Boolean>());
}
Napi::Value
ListField::IsMultiSelect(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetField().IsMultiSelect());
}
}
