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
#include "../Defines.h"
#include "../ValidateArguments.h"
#include "../base/Color.h"
#include "../base/Dictionary.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "Action.h"
#include "Annotation.h"
#include "Form.h"
#include "Page.h"
#include <algorithm>
#include <iostream>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::find;
using std::make_shared;
using std::make_unique;
using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

// FunctionReference Field::constructor; // NOLINT

/**
 * @brief Field::Field
 * @param info
 */
Field::Field(EPdfField type, const CallbackInfo& info)
{
  if (info[0].IsExternal()) {
    auto arg = info[0].As<External<PdfField>>().Data();
    field = new PdfField(*arg);
  } else {
    auto arg1 = info[0].As<Object>();
    if (arg1.InstanceOf(Page::constructor.Value())) {
      auto page = Page::Unwrap(arg1);
      if (info[1].IsNumber()) {
        int index = info[1].As<Number>();
        field = new PdfField(page->page.GetField(index));
      }
    } else if (arg1.InstanceOf(Annotation::constructor.Value())) {
      PdfAnnotation* annotation = &Annotation::Unwrap(arg1)->GetAnnotation();
      if (info[1].IsObject() &&
          info[1].As<Object>().InstanceOf(Form::constructor.Value())) {
        PdfAcroForm* form = Form::Unwrap(info[1].As<Object>())->GetForm();
        switch (type) {
          case PoDoFo::ePdfField_PushButton:
            field = new PdfPushButton(annotation, form);
            break;
          case PoDoFo::ePdfField_CheckBox:
            field = new PdfCheckBox(annotation, form);
            break;
          case PoDoFo::ePdfField_TextField:
            field = new PdfTextField(annotation, form);
            break;
          case PoDoFo::ePdfField_ComboBox:
            field = new PdfComboBox(annotation, form);
            break;
          case PoDoFo::ePdfField_ListBox:
            field = new PdfListBox(annotation, form);
            break;
          case PoDoFo::ePdfField_Signature:
          case PoDoFo::ePdfField_RadioButton:
          case PoDoFo::ePdfField_Unknown:
            Error::New(info.Env(),
                       "Field type not yet implemented for new instances")
              .ThrowAsJavaScriptException();
            break;
        }
      }
    } else {
      TypeError::New(info.Env(), "Signature Mismatch")
        .ThrowAsJavaScriptException();
      return;
    }
  }
  fieldName = field->GetFieldName().GetStringUtf8();
  fieldType = TypeString();
}

Field::~Field()
{
  delete field;
  for (auto c : children) {
    delete c;
  }
}

string
Field::TypeString()
{
  string typeStr;
  switch (field->GetType()) {
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

value
Field::GetType(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<int>(field->GetType()));
}

value
Field::GetFieldName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           GetField().GetFieldName().GetStringUtf8());
}

void
Field::SetFieldName(const CallbackInfo&, const Napi::Value& value)
{
  string name = value.As<String>().Utf8Value();
  GetField().SetFieldName(PdfString(name));
}

value
Field::GetAlternateName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           field->GetAlternateName().GetStringUtf8());
}

value
Field::GetMappingName(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), field->GetMappingName().GetStringUtf8());
}

void
Field::SetAlternateName(const CallbackInfo&, const Napi::Value& value)
{
  field->SetAlternateName(value.As<String>().Utf8Value());
}

void
Field::SetMappingName(const CallbackInfo&, const Napi::Value& value)
{
  field->SetMappingName(value.As<String>().Utf8Value());
}

void
Field::SetRequired(const CallbackInfo&, const Napi::Value& value)
{
  field->SetRequired(value.As<Boolean>());
}

value
Field::IsRequired(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), field->IsRequired());
}

value
Field::IsReadOnly(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), field->IsReadOnly());
}

void
Field::SetReadOnly(const Napi::CallbackInfo&, const Napi::Value& value)
{
  field->SetReadOnly(value.As<Boolean>());
}
void
Field::SetExport(const Napi::CallbackInfo&, const Napi::Value& value)
{
  field->SetExport(value.As<Boolean>());
}
value
Field::IsExport(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), field->IsExport());
}
/**
 * @note (color:Array<number>, transparent?: boolean)
 * @param info
 * @return
 */
void
Field::SetBackground(const Napi::CallbackInfo& info)
{
  vector<NPDFColorFormat> types = { NPDFColorFormat::GreyScale,
                                    NPDFColorFormat::RGB,
                                    NPDFColorFormat::CMYK };
  NPDFColorAccessor(Color::Unwrap(info[0].As<Object>())->color,
                    types,
                    field->SetBackgroundColor)
}
void
Field::SetBorder(const Napi::CallbackInfo& info)
{
  vector<NPDFColorFormat> types = { NPDFColorFormat::GreyScale,
                                    NPDFColorFormat::RGB,
                                    NPDFColorFormat::CMYK };
  NPDFColorAccessor(
    Color::Unwrap(info[0].As<Object>())->color, types, field->SetBorderColor)
}

void
Field::SetHighlightingMode(const Napi::CallbackInfo& info)
{
  EPdfHighlightingMode mode =
    static_cast<EPdfHighlightingMode>(info[0].As<Number>().Uint32Value());
  field->SetHighlightingMode(mode);
}
/**
 * @note (type: NPDFMouseActionEnum: 'up'|'down'|'enter'|'exit', action:
 * NPDFAction)
 * @return
 */
void
Field::SetMouseAction(const Napi::CallbackInfo& info)
{
  int onMouse = info[0].As<Number>();
  PdfAction& action = Action::Unwrap(info[1].As<Object>())->GetAction();
  switch (onMouse) {
    case 0: // up
      field->SetMouseUpAction(action);
      break;
    case 1: // down
      field->SetMouseDownAction(action);
      break;
    case 2: // enter
      field->SetMouseEnterAction(action);
      break;
    case 3: // exit
      field->SetMouseLeaveAction(action);
      break;
    default:
      TypeError::New(info.Env(), "Unknown mouse action. See NPDFMouseEvents")
        .ThrowAsJavaScriptException();
  }
}
void
Field::SetPageAction(const Napi::CallbackInfo& info)
{
  int onMouse = info[0].As<Number>();
  PdfAction& action = Action::Unwrap(info[1].As<Object>())->GetAction();
  switch (onMouse) {
    case 0: // open
      field->SetPageOpenAction(action);
      break;
    case 1: // close
      field->SetPageCloseAction(action);
      break;
    case 2: // visible
      field->SetPageVisibleAction(action);
      break;
    case 3: // invisible
      field->SetPageInvisibleAction(action);
      break;
    default:
      TypeError::New(info.Env(), "Unknown mouse action. See NPDFMouseEvents")
        .ThrowAsJavaScriptException();
  }
}
value
Field::GetAnnotation(const Napi::CallbackInfo& info)
{
  PdfAnnotation* annot = field->GetWidgetAnnotation();
  return Annotation::constructor.New(
    { External<PdfAnnotation>::New(info.Env(), annot) });
}
value
Field::GetAppearanceStream(const Napi::CallbackInfo& info)
{
  if (GetFieldDictionary().HasKey(Name::AP)) {
    auto ap = field->GetFieldObject()->MustGetIndirectKey(Name::AP);
    return Dictionary::constructor.New(
      { External<PdfDictionary>::New(info.Env(), &ap->GetDictionary()) });
  } else {
    return info.Env().Null();
  }
}
void
Field::SetAppearanceStream(const Napi::CallbackInfo& info,
                           const Napi::Value& value)
{
  if (GetFieldDictionary().HasKey(Name::AP)) {
    GetFieldDictionary().RemoveKey(Name::AP);
  }
  if (value.IsNull()) {
    cout << "The AP value you've provided is null. Removing field ap object"
         << endl;
  }

  if (value.IsObject() &&
      value.As<Object>().InstanceOf(Dictionary::constructor.Value())) {
    PdfDictionary dict =
      Dictionary::Unwrap(value.As<Object>())->GetDictionary();
    GetFieldDictionary().AddKey(PdfName(Name::AP), dict);
  }
}
value
Field::GetDefaultAppearance(const Napi::CallbackInfo& info)
{
  if (GetFieldDictionary().HasKey(Name::DA)) {
    auto da = field->GetFieldObject()->MustGetIndirectKey(Name::DA);
    return String::New(info.Env(), da->GetString().GetStringUtf8());
  } else {
    return info.Env().Null();
  }
}
void
Field::SetDefaultAppearance(const Napi::CallbackInfo& info,
                            const Napi::Value& value)
{
  if (GetFieldDictionary().HasKey(Name::DA)) {
    GetFieldDictionary().RemoveKey(Name::DA);
  }
  if (value.IsNull()) {
    cout << "The DA value you've provided is null. Removing field DA from "
            "field dictionary"
         << endl;
  }

  if (value.IsString()) {
    GetFieldDictionary().AddKey(PdfName(Name::DA),
                                PdfString(value.As<String>()));
  }
}
value
Field::GetJustification(const Napi::CallbackInfo& info)
{
  if (field->GetFieldObject()->GetDictionary().HasKey(Name::Q)) {
    return Number::New(
      info.Env(),
      field->GetFieldObject()->MustGetIndirectKey(Name::Q)->GetNumber());
  } else {
    return info.Env().Null();
  }
}
void
Field::SetJustification(const Napi::CallbackInfo& info,
                        const Napi::Value& value)
{
  if (value.IsNumber()) {
    pdf_int64 qValue = value.As<Number>();
    if (qValue > 3 || qValue < 0) {
      RangeError::New(info.Env(), "Please see NPDFAlignment for valid values")
        .ThrowAsJavaScriptException();
      return;
    }
    if (GetFieldDictionary().HasKey(Name::Q)) {
      GetFieldDictionary().RemoveKey(Name::Q);
    }
    GetFieldDictionary().AddKey(Name::Q, PdfVariant(qValue));
  }
}

value
Field::GetFieldObject(const CallbackInfo& info)
{
  return Obj::constructor.New(
    { External<PdfObject>::New(info.Env(), field->GetFieldObject()) });
}

}
