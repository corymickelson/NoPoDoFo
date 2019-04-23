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
#include "../ValidateArguments.h"
#include "../base/Dictionary.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "Action.h"
#include "Annotation.h"
#include "Document.h"
#include "Form.h"
#include "Page.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::find;
using std::map;
using std::string;
using std::stringstream;
using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

/**
 * @brief Field::Field
 * @param info
 */
Field::Field(EPdfField type, const CallbackInfo& info)
{
  dbglog = spdlog::get("dbglog");
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
  dbglog->debug("Field Cleanup");
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
      { External<PdfObject>::New(info.Env(), ap), Number::New(info.Env(), 0) });
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

std::map<std::string, PoDoFo::PdfObject*>
Field::GetFieldRefreshKeys(PoDoFo::PdfField* f)
{
  map<string, PdfObject*> keys;
  PdfObject* formDA = f->GetFieldObject()
                        ->GetOwner()
                        ->GetParentDocument()
                        ->GetAcroForm()
                        ->GetObject()
                        ->MustGetIndirectKey(Name::DA);
  if (!formDA || !formDA->GetString().IsValid()) {
    if (!f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(
          Name::DA)) {
      f->GetWidgetAnnotation()->GetObject()->GetDictionary().AddKey(
        Name::DA, PdfString());
    }
    keys.insert(std::pair<string, PdfObject*>(
      Name::DA,
      f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::DA)));
  } else {
    keys.insert(std::pair<string, PdfObject*>(Name::DA, formDA));
  }

  if (!f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(
        Name::AP)) {
    f->GetWidgetAnnotation()->GetObject()->GetDictionary().AddKey(
      Name::AP, PdfDictionary());
    PdfRect xRect;
    xRect.SetLeft(0.0);
    xRect.SetBottom(0.0);
    xRect.SetWidth(f->GetWidgetAnnotation()->GetRect().GetWidth());
    xRect.SetHeight(f->GetWidgetAnnotation()->GetRect().GetHeight());
    PdfXObject x(xRect,
                 f->GetFieldObject()->GetOwner()->GetParentDocument());
    f->GetWidgetAnnotation()
      ->GetObject()
      ->MustGetIndirectKey(Name::AP)
      ->GetDictionary()
      .AddKey(Name::N, x.GetObjectReference());
  }
  keys.insert(std::pair<string, PdfObject*>(
    Name::AP,
    f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::AP)));
  if (f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(Name::V)) {
    keys.insert(std::pair<string, PdfObject*>(
      Name::V,
      f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::V)));
  }
  if (f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(Name::DV)) {
    keys.insert(std::pair<string, PdfObject*>(
      Name::DV,
      f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::DV)));
  }
  return keys;
}

/**
 * @todo: how to handle fonts that do not have an object (default fonts)
 */
void
Field::RefreshAppearanceStream()
{
  stringstream ss;
  PdfLocaleImbue(ss);
  map<string, PdfObject*> apKeys;
  PdfRefCountedBuffer buffer;
  apKeys = GetFieldRefreshKeys(&GetField());
  if (apKeys.find(Name::V) == apKeys.end() &&
      apKeys.find(Name::DV) == apKeys.end()) {
    throw std::exception();
  }
  if (!apKeys.find(Name::V)->second->GetString().IsValid() ||
      apKeys.find(Name::V)->second->GetString().GetCharacterLength() <= 0) {
    throw std::exception();
  }
  PdfXObject xObj(apKeys.find(Name::AP)->second->MustGetIndirectKey(Name::N));
  xObj.GetContentsForAppending()->GetStream()->BeginAppend();
  PdfOutputDevice device(&buffer);
  apKeys.find(Name::V)->second->GetString().Write(&device,
                                                  ePdfWriteMode_Compact);
  ss << "/Tx " << BeginMarkedContentOp << endl;
  ss << SaveOp << endl;
  ss << BeginTextOp << endl;
  if (apKeys.find(Name::DA) != apKeys.end()) {
    ss << apKeys.find(Name::DA)->second->GetString().GetString() << endl;
    if (!xObj.GetResources()->GetDictionary().HasKey(Name::FONT)) {
      PdfFont* f = GetDAFont(
        string_view(apKeys.find(Name::DA)->second->GetString().GetString()));
      xObj.AddResource(
        f->GetIdentifier(), f->GetObject()->Reference(), Name::FONT);
    }
    if (field->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(
          Name::DA)) {
      field->GetWidgetAnnotation()->GetObject()->GetDictionary().RemoveKey(
        Name::DA);
    }
    // Add the DA key from apKeys in case the DA was taken from the form
    field->GetWidgetAnnotation()->GetObject()->GetDictionary().AddKey(
      Name::DA, apKeys.find(Name::DA)->second);
  }
  ss << "2.0 2.0 " << TextPosOp << endl;
  ss << buffer.GetBuffer() << ShowTextOp << endl;
  ss << EndTextOp << endl;
  ss << RestoreOp << endl;
  ss << EndMarkedContentOp << endl;

  xObj.GetContentsForAppending()->GetStream()->Append(ss.str());
  xObj.GetContentsForAppending()->GetStream()->EndAppend();

  PdfRect r(0,
            0,
            field->GetWidgetAnnotation()->GetRect().GetWidth(),
            field->GetWidgetAnnotation()->GetRect().GetHeight());
  xObj.GetObject()->GetDictionary().RemoveKey(Name::BBOX);
  PdfVariant ra;
  r.ToVariant(ra);
  xObj.GetObject()->GetDictionary().AddKey(Name::BBOX, ra.GetArray());
}
PoDoFo::PdfFont*
Field::GetDAFont(string_view da)
{
  // 0 0 1 rg /Ft20 12 Tf
  // find Tf
  // go back 2 /space char
  // read from index of second /space to '/' for PdfFont name
  // use Document.listFonts to find the font
  long ftIndex = da.find(FontAndSizeOp);
  if (ftIndex == -1) {
    throw std::exception();
  }
  long nameIndex = da.find("/");
  if (nameIndex == -1) {
    throw std::exception();
  }
  string_view fontAndSize = da.substr(static_cast<size_t>(nameIndex),
                                      static_cast<size_t>(ftIndex - nameIndex));
  long firstSpace = fontAndSize.find_first_of(" ");
  string_view ftName =
    fontAndSize.substr(1, static_cast<size_t>(firstSpace - 1));
  PdfFont* font;
  auto memDoc = dynamic_cast<PdfMemDocument*>(
    field->GetWidgetAnnotation()->GetObject()->GetOwner()->GetParentDocument());
  if ((font = Document::GetPdfFont(*memDoc, ftName)) == nullptr) {
  }
  return font;
}
}
