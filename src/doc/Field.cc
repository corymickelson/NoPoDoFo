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
#include "Action.h"
#include "Annotation.h"
#include "Document.h"
#include "Form.h"
#include "Page.h"
#include "Rect.h"
#include "StreamDocument.h"
#include <iostream>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::make_shared;
using std::make_unique;

namespace NoPoDoFo {

// FunctionReference Field::constructor; // NOLINT

/**
 * @brief Field::Field
 * @param info
 */
Field::Field(EPdfField type, const CallbackInfo& info)
{
  if (info.Length() < 2) {
    TypeError::New(info.Env(),
                   "Field constructor args: [[IPage, number], [IAnnotation, "
                   "IForm], [IPage, IRect, IBase]]")
      .ThrowAsJavaScriptException();
    return;
  }
  auto arg1 =
    info[0]
      .As<Object>();
  if (arg1.InstanceOf(Page::constructor.Value())) {
    auto page = Page::Unwrap(arg1);
    if (info[1].IsNumber()) {
      int index = info[1].As<Number>();
      cout << "Constructing field from existing object" << endl;
      field = make_shared<PdfField>(page->GetPage()->GetField(index));
    }
  } else if (arg1.InstanceOf(Annotation::constructor.Value())) {
    PdfAnnotation* annotation = &Annotation::Unwrap(arg1)->GetAnnotation();
    if (info[1].IsObject() &&
        info[1].As<Object>().InstanceOf(Form::constructor.Value())) {
      PdfAcroForm* form = Form::Unwrap(info[1].As<Object>())->GetForm();
      field = make_shared<PdfField>(form->GetObject(), annotation);
    }
  } else {
    TypeError::New(info.Env(), "Signature Mismatch")
      .ThrowAsJavaScriptException();
    return;
  }
  fieldName = field.get()->GetFieldName().GetStringUtf8();
  fieldType = TypeString();
  cout << "Field type: " << fieldType << endl;
}

string
Field::TypeString()
{
  string typeStr;
  switch (field.get()->GetType()) {
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
Field::GetType(const Napi::CallbackInfo &info) {
  return Number::New(info.Env(), static_cast<int>(field->GetType()));
}

Napi::Value
Field::GetFieldName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), fieldName);
}

void
Field::SetFieldName(const CallbackInfo&, const Napi::Value& value)
{
  field.get()->SetFieldName(value.As<String>().Utf8Value());
}

Napi::Value
Field::GetAlternateName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           field.get()->GetAlternateName().GetStringUtf8());
}

Napi::Value
Field::GetMappingName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           field.get()->GetMappingName().GetStringUtf8());
}

void
Field::SetAlternateName(const CallbackInfo&, const Napi::Value& value)
{
  field.get()->SetAlternateName(value.As<String>().Utf8Value());
}

void
Field::SetMappingName(const CallbackInfo&, const Napi::Value& value)
{
  field.get()->SetMappingName(value.As<String>().Utf8Value());
}

void
Field::SetRequired(const CallbackInfo&, const Napi::Value& value)
{
  field.get()->SetRequired(value.As<Boolean>());
}

Napi::Value
Field::IsRequired(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), field.get()->IsRequired());
}

Napi::Value
Field::IsReadOnly(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), field.get()->IsReadOnly());
}

void
Field::SetReadOnly(const Napi::CallbackInfo&, const Napi::Value& value)
{
  field.get()->SetReadOnly(value.As<Boolean>());
}
void
Field::SetExport(const Napi::CallbackInfo&, const Napi::Value& value)
{
  field.get()->SetExport(value.As<Boolean>());
}
Napi::Value
Field::IsExport(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), field.get()->IsExport());
}
/**
 * @note (color:Array<number>, transparent?: boolean)
 * @param info
 * @return
 */
Napi::Value
Field::SetBackground(const Napi::CallbackInfo& info)
{
  auto js = info[0].As<Array>();
  if (js.Length() == 1) {
    double gray = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    field.get()->SetBackgroundColor(gray);
  } else if (js.Length() == 3) {
    double r = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    double g = js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    double b = js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    field.get()->SetBackgroundColor(r, g, b);
  } else if (js.Length() == 4) {
    double c = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    double m = js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    double y = js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    double k = js.Get(static_cast<uint32_t>(3)).As<Number>().DoubleValue();
    field.get()->SetBackgroundColor(c, m, y, k);
  } else {
    TypeError::New(info.Env(), "value must be [grayscale], [r,g,b], [c,m,y,k]")
      .ThrowAsJavaScriptException();
  }
  return info.Env().Undefined();
}
Napi::Value
Field::SetBorder(const Napi::CallbackInfo& info)
{
  auto js = info[0].As<Array>();
  if (js.Length() == 1) {
    double gray = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    field.get()->SetBorderColor(gray);
  } else if (js.Length() == 3) {
    double r = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    double g = js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    double b = js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    field.get()->SetBorderColor(r, g, b);
  } else if (js.Length() == 4) {
    double c = js.Get(static_cast<uint32_t>(0)).As<Number>().DoubleValue();
    double m = js.Get(static_cast<uint32_t>(1)).As<Number>().DoubleValue();
    double y = js.Get(static_cast<uint32_t>(2)).As<Number>().DoubleValue();
    double k = js.Get(static_cast<uint32_t>(3)).As<Number>().DoubleValue();
    field.get()->SetBorderColor(c, m, y, k);
  } else {
    TypeError::New(info.Env(), "value must be [grayscale], [r,g,b], [c,m,y,k]")
      .ThrowAsJavaScriptException();
  }
  return info.Env().Undefined();
}
Napi::Value
Field::SetHighlightingMode(const Napi::CallbackInfo& info)
{
  EPdfHighlightingMode mode =
    static_cast<EPdfHighlightingMode>(info[0].As<Number>().Uint32Value());
  field.get()->SetHighlightingMode(mode);
  return info.Env().Undefined();
}
/**
 * @note (type: NPDFMouseActionEnum: 'up'|'down'|'enter'|'exit', action:
 * NPDFAction)
 * @return
 */
Napi::Value
Field::SetMouseAction(const Napi::CallbackInfo& info)
{
  int onMouse = info[0].As<Number>();
  PdfAction* action = Action::Unwrap(info[1].As<Object>())->GetAction();
  switch (onMouse) {
    case 0: // up
      field.get()->SetMouseUpAction(*action);
      break;
    case 1: // down
      field.get()->SetMouseDownAction(*action);
      break;
    case 2: // enter
      field.get()->SetMouseEnterAction(*action);
      break;
    case 3: // exit
      field.get()->SetMouseLeaveAction(*action);
      break;
    default:
      TypeError::New(info.Env(), "Unknown mouse action. See NPDFMouseEvents")
        .ThrowAsJavaScriptException();
  }
  return info.Env().Undefined();
}
Napi::Value
Field::SetPageAction(const Napi::CallbackInfo& info)
{
  int onMouse = info[0].As<Number>();
  PdfAction* action = Action::Unwrap(info[1].As<Object>())->GetAction();
  switch (onMouse) {
    case 0: // open
      field.get()->SetPageOpenAction(*action);
      break;
    case 1: // close
      field.get()->SetPageCloseAction(*action);
      break;
    case 2: // visible
      field.get()->SetPageVisibleAction(*action);
      break;
    case 3: // invisible
      field.get()->SetPageInvisibleAction(*action);
      break;
    default:
      TypeError::New(info.Env(), "Unknown mouse action. See NPDFMouseEvents")
        .ThrowAsJavaScriptException();
  }
  return info.Env().Undefined();
}
}
