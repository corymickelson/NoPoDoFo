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

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;

namespace NoPoDoFo {

FunctionReference Field::constructor; // NOLINT

Field::Field(const CallbackInfo& info)
  : ObjectWrap(info)
  , field(info[0].As<External<PdfField>>().Data())
{
  fieldName = field->GetFieldName().GetStringUtf8();
  fieldType = TypeString();
}

void
Field::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Field",
    { InstanceAccessor("readOnly", &Field::IsReadOnly, &Field::SetReadOnly),
      InstanceAccessor("required", &Field::IsRequired, &Field::SetRequired),
      InstanceAccessor("exported", &Field::IsExport, &Field::SetExport),
      InstanceAccessor("type", &Field::GetType, nullptr),
      InstanceAccessor("fieldName", &Field::GetFieldName, &Field::SetFieldName),
      InstanceAccessor(
        "alternateName", &Field::GetAlternateName, &Field::SetAlternateName),
      InstanceAccessor(
        "mappingName", &Field::GetMappingName, &Field::SetMappingName) });
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

void
Field::SetFieldName(const CallbackInfo&, const Napi::Value& value)
{
  GetField()->SetFieldName(value.As<String>().Utf8Value());
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
Field::SetAlternateName(const CallbackInfo&, const Napi::Value& value)
{
  GetField()->SetAlternateName(value.As<String>().Utf8Value());
}

void
Field::SetMappingName(const CallbackInfo&, const Napi::Value& value)
{
  GetField()->SetMappingName(value.As<String>().Utf8Value());
}

void
Field::SetRequired(const CallbackInfo&, const Napi::Value& value)
{
  GetField()->SetRequired(value.As<Boolean>());
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
Field::SetReadOnly(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetField()->SetReadOnly(value.As<Boolean>());
}
void
Field::SetExport(const Napi::CallbackInfo&, const Napi::Value& value)
{
  GetField()->SetExport(value.As<Boolean>());
}
Napi::Value
Field::IsExport(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetField()->IsExport());
}
/**
 * @note (color:Array<number>, transparent?: boolean)
 * @param info
 * @return
 */
Napi::Value Field::SetBackground(const Napi::CallbackInfo &info) {
  auto js = info[0].As<Array>();
  switch (js.Length()) {
  case 1:
    auto gray = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    GetField()->SetBackgroundColor(gray);
    break;
  case 3:
    auto r = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    auto g = js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    auto b = js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    GetField()->SetBackgroundColor(r,g, b);
    break;
  case 4:
    auto c= js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    auto m= js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    auto y= js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    auto k= js.Get(static_cast<uint32_t>(3)).As<Number>().DoubleValue();
    GetField()->SetBackgroundColor(c, m, y, k);
    break;
  default:
    TypeError::New(info.Env(), "value must be [grayscale], [r,g,b], [c,m,y,k]").ThrowAsJavaScriptException();
    break;
  }
  return info.Env().Undefined();
}
void Field::SetColor(const Napi::Value& value, const std::function<void(const vector<double>)> &fn) {
  auto js = value.As<Array>();
  vector<double> values;
  switch (js.Length()) {
  case 1:
    auto gray = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    values.push_back(gray);
//    GetField()->SetBackgroundColor(gray);
    break;
  case 3:
    auto r = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    values.push_back(r);
    auto g = js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    values.push_back(g);
    auto b = js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    values.push_back(b);
//    GetField()->SetBackgroundColor(r,g, b);
    break;
  case 4:
    auto c= js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    values.push_back(c);
    auto m= js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    values.push_back(m);
    auto y= js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    values.push_back(y);
    auto k= js.Get(static_cast<uint32_t>(3)).As<Number>().DoubleValue();
    values.push_back(k);
//    GetField()->SetBackgroundColor(c, m, y, k);
    break;
  default:
    break;
  }
  fn(values);
}

}
