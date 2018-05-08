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

#include "Field.h"
#include "Document.h"
#include "Page.h"

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;

FunctionReference Field::constructor; // NOLINT

Field::Field(const CallbackInfo& info)
  : ObjectWrap(info)
  , field(info[0].As<External<PdfField>>().Data())
//  , field(make_unique<PdfField>(*info[0].As<External<PdfField>>().Data()))
{
  fieldName = field->GetFieldName().GetStringUtf8();
  fieldType = TypeString();
}

Field::~Field()
{
  cout << "Destructing Field: " << fieldName << " of type: " << fieldType
       << endl;
  //  HandleScope scope(Env());
  //  delete field;
}
void
Field::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Field",
    { InstanceAccessor("readOnly", &Field::IsReadOnly, &Field::SetReadOnly),
      InstanceMethod("getType", &Field::GetType),
      InstanceMethod("getFieldName", &Field::GetFieldName),
      InstanceMethod("getAlternateName", &Field::GetAlternateName),
      InstanceMethod("getMappingName", &Field::GetMappingName),
      InstanceMethod("setAlternateName", &Field::SetAlternateName),
      InstanceMethod("setMappingName", &Field::SetMappingName),
      InstanceMethod("setRequired", &Field::SetRequired),
      InstanceMethod("isRequired", &Field::IsRequired) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Field", ctor);
}

string
Field::TypeString()
{
  string typeStr;
  switch (GetField()->GetType()) {
    case PoDoFo::EPdfField::ePdfField_CheckBox:
      typeStr = "CheckBox";
      break;
    case PoDoFo::EPdfField::ePdfField_ComboBox:
      typeStr = "ComboBox";
      break;
    case PoDoFo::EPdfField::ePdfField_ListBox:
      typeStr = "ListBox";
      break;
    case PoDoFo::EPdfField::ePdfField_PushButton:
      typeStr = "PushButton";
      break;
    case PoDoFo::EPdfField::ePdfField_RadioButton:
      typeStr = "RadioButton";
      break;
    case PoDoFo::EPdfField::ePdfField_Signature:
      typeStr = "Signature";
      break;
    case PoDoFo::EPdfField::ePdfField_TextField:
      typeStr = "TextField";
      break;
    case PoDoFo::EPdfField::ePdfField_Unknown:
      typeStr = "Unknown";
  }
  return typeStr;
}
Napi::Value
Field::GetType(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), fieldType);
}

Napi::Value
Field::GetFieldName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), fieldName);
}

Napi::Value
Field::GetAlternateName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           GetField()->GetAlternateName().GetStringUtf8());
}

Napi::Value
Field::GetMappingName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           GetField()->GetMappingName().GetStringUtf8());
}

void
Field::SetAlternateName(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  PdfString value(info[0].As<String>().Utf8Value().c_str());
  GetField()->SetAlternateName(value);
}

void
Field::SetMappingName(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  PdfString value(info[0].As<String>().Utf8Value().c_str());
  GetField()->SetMappingName(value);
}

void
Field::SetRequired(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  bool value = info[0].As<Boolean>();
  GetField()->SetRequired(value);
}

Napi::Value
Field::IsRequired(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetField()->IsRequired());
}

Napi::Value
Field::IsReadOnly(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetField()->IsReadOnly());
}

void
Field::SetReadOnly(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  GetField()->SetReadOnly(value.As<Boolean>());
}
}
