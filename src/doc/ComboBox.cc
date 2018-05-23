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
#include "../ValidateArguments.h"
#include "Field.h"

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;

namespace NoPoDoFo {

FunctionReference ComboBox::constructor; // NOLINT

ComboBox::ComboBox(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , ListField(info)
{
  field = Field::Unwrap(info[0].As<Napi::Object>())->GetField();
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
      InstanceMethod("getItem", &ListField::GetItem)

    });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("ComboBox", ctor);
}
void
ComboBox::SetEditable(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetField().SetEditable(value.As<Napi::Boolean>());
}
Napi::Value
ComboBox::GetEditable(const Napi::CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetField().IsEditable());
}
}
