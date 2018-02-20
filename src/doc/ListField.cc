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

namespace NoPoDoFo {

using namespace PoDoFo;
using namespace Napi;

FunctionReference ListField::constructor;

ListField::ListField(const CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto wrap = info[0].As<Object>();
  auto field = Field::Unwrap(wrap);
//  auto listField = new PdfListField(field->GetField());
  list = make_unique<PdfListField>(*new PdfListField(field->GetField()));
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
  list->InsertItem(PdfString(value), PdfString(display));
}

void
ListField::RemoveItem(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  if (static_cast<size_t>(index) > list->GetItemCount() || index < 0) {
    throw Napi::Error::New(info.Env(), "index out of range");
  }
  list->RemoveItem(index);
}

Napi::Value
ListField::GetItem(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  Object item = Object::New(info.Env());
  string value = list->GetItem(index).GetString();
  string display = list->GetItemDisplayText(index).GetString();
  item.Set(String::New(info.Env(), "value"), String::New(info.Env(), value));
  item.Set(String::New(info.Env(), "display"),
           String::New(info.Env(), display));
  return item;
}

Napi::Value
ListField::GetItemCount(const CallbackInfo& info)
{
  return Number::New(info.Env(), list->GetItemCount());
}

void
ListField::SetSelectedItem(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "index must be of type number");
  }
  list->SetSelectedItem(value.As<Number>());
}

Napi::Value
ListField::GetSelectedItem(const CallbackInfo& info)
{
  int index = list->GetSelectedItem();
  return Number::New(info.Env(), index);
}

}
