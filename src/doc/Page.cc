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

#include "Page.h"
#include "../ErrorHandler.h"
#include "../base/Obj.h"
#include "Annotation.h"
#include "Field.h"

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;

FunctionReference Page::constructor; // NOLINT

Page::Page(const CallbackInfo& info)
  : ObjectWrap(info)
  , doc(Document::Unwrap(info[0].As<Object>()))
  , n(info[1].As<Number>())
{}
Page::~Page()
{
  cout << "Destructing Page" << endl;
  doc = nullptr;
}
void
Page::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Page",
    { InstanceAccessor("rotation", &Page::GetRotation, &Page::SetRotation),
      InstanceAccessor("trimBox", &Page::GetTrimBox, &Page::SetTrimBox),
      InstanceAccessor("number", &Page::GetPageNumber, nullptr),
      InstanceAccessor("width", &Page::GetPageWidth, &Page::SetPageWidth),
      InstanceAccessor("height", &Page::GetPageHeight, &Page::SetPageHeight),
      InstanceMethod("getField", &Page::GetField),
      InstanceMethod("getNumFields", &Page::GetNumFields),
      InstanceMethod("getFieldsInfo", &Page::GetFieldsSummary),
      InstanceMethod("getFieldIndex", &Page::GetFieldIndex),
      InstanceMethod("getContents", &Page::GetContents),
      InstanceMethod("getResources", &Page::GetResources),
      InstanceMethod("getMediaBox", &Page::GetMediaBox),
      InstanceMethod("getBleedBox", &Page::GetBleedBox),
      InstanceMethod("getArtBox", &Page::GetArtBox),
      InstanceMethod("createAnnotation", &Page::CreateAnnotation),
      InstanceMethod("getAnnotation", &Page::GetAnnotation),
      InstanceMethod("getNumAnnots", &Page::GetNumAnnots),
      InstanceMethod("deleteAnnotation", &Page::DeleteAnnotation) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Page", ctor);
}
Napi::Value
Page::GetRotation(const CallbackInfo& info)
{
  return Number::New(info.Env(), GetPage()->GetRotation());
}
Napi::Value
Page::GetNumFields(const CallbackInfo& info)
{
  return Number::New(info.Env(), GetPage()->GetNumFields());
}

Napi::Value
Page::GetField(const CallbackInfo& info)
{
  //  EscapableHandleScope scope(info.Env());
  int index = info[0].As<Number>();
  if (index < 0 || index > GetPage()->GetNumFields()) {
    throw RangeError();
  }
  auto instance = Field::constructor.New(
    { External<PdfField>::New(info.Env(),
                              new PdfField(GetPage()->GetField(index)),
                              [](Napi::Env env, PdfField* data) {
                                HandleScope scope(env);
                                delete data;
                                data = nullptr;
                              }) });
  return instance;
}

Napi::Value
Page::GetFieldsSummary(const CallbackInfo& info)
{
  try {
    EscapableHandleScope scope(info.Env());
    auto fields = Array::New(info.Env());
    for (size_t i = 0; i < static_cast<size_t>(GetPage()->GetNumFields());
         ++i) {
      auto obj = Object::New(info.Env());
      auto field = GetPage()->GetField(static_cast<int>(i));
      obj.Set("index", Number::New(info.Env(), i));
      Page::GetFieldObject(info, obj, field);
      fields.Set(static_cast<uint32_t>(i), obj);
    }
    return scope.Escape(fields);
  } catch (PdfError& err) {
    stringstream errMsg;
    errMsg << "An error occurred in PoDoFo: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), errMsg.str());
  }
}

void
Page::GetFieldObject(const CallbackInfo& info,
                     Napi::Object& obj,
                     PoDoFo::PdfField& field)
{
  string name = field.GetFieldName().GetStringUtf8();
  string alternateName = field.GetAlternateName().GetStringUtf8();
  string mappingName = field.GetMappingName().GetStringUtf8();
  bool required = field.IsRequired();
  bool isWritable = field.IsReadOnly();
  obj.Set("name", name);
  obj.Set("alternateName", alternateName);
  obj.Set("mappingName", mappingName);
  obj.Set("required", required);
  obj.Set("readOnly", isWritable);
  switch (field.GetType()) {
    case ePdfField_TextField: {
      PdfTextField textField(field);
      string fieldValue = textField.GetText().GetStringUtf8();
      long maxLen = textField.GetMaxLen();
      bool multiLine = textField.IsMultiLine();
      obj.Set("value", fieldValue);
      obj.Set("maxLength", static_cast<double>(maxLen));
      obj.Set("isMultiLine", multiLine);
      obj.Set("type", "TextField");
      break;
    }
    case ePdfField_CheckBox: {
      PdfCheckBox checkBox(field);
      bool checkBoxValue = checkBox.IsChecked();
      string checkBoxCaption = checkBox.GetCaption().GetStringUtf8();
      obj.Set("value", checkBoxValue);
      obj.Set("caption", checkBoxCaption);
      obj.Set("type", "CheckBox");
      break;
    }
    case ePdfField_ComboBox: {
      PdfComboBox comboBox(field);
      int displayIndex = comboBox.GetSelectedItem();
      string comboValue =
        displayIndex > -1
          ? comboBox.GetItem(comboBox.GetSelectedItem()).GetStringUtf8()
          : "";
      obj.Set("type", "ComboBox");
      obj.Set("selected", comboValue);
      auto items = Array::New(info.Env());
      for (size_t i = 0; i < comboBox.GetItemCount(); i++) {
        items.Set(
          static_cast<uint32_t>(i),
          comboBox.GetItemDisplayText(static_cast<int>(i)).GetStringUtf8());
      }
      obj.Set("items", items);
      break;
    }
    case ePdfField_ListBox: {
      PdfListBox listBox(field);
      int selected = listBox.GetSelectedItem();
      string listValue =
        selected > -1
          ? listBox.GetItem(listBox.GetSelectedItem()).GetStringUtf8()
          : "";
      obj.Set("type", "ListBox");
      obj.Set("value", listValue);
      auto items = Array::New(info.Env());
      for (size_t i = 0; i < listBox.GetItemCount(); i++) {
        items.Set(
          static_cast<uint32_t>(i),
          listBox.GetItemDisplayText(static_cast<int>(i)).GetStringUtf8());
      }
      obj.Set("items", items);
      break;
    }
    case ePdfField_PushButton: {
      PdfPushButton pushButton(field);
      string pushCaption = pushButton.GetCaption().GetStringUtf8();
      obj.Set("type", "PushButton");
      obj.Set("caption", pushCaption);
      break;
    }
    case ePdfField_RadioButton: {
      obj.Set("type", "RadioButton");
      break;
    }
    case ePdfField_Signature: {
      obj.Set("type", "Signature");
      break;
    }
    case ePdfField_Unknown: {
      obj.Set("type", "Unknown");
      break;
    }
  }
}

Napi::Object
Page::ExtractAndApplyRectValues(const CallbackInfo& info, PdfRect& rect)
{
  Napi::Object obj = Napi::Object::New(info.Env());
  obj.Set("left", Napi::Number::New(info.Env(), rect.GetLeft()));
  obj.Set("bottom", Napi::Number::New(info.Env(), rect.GetBottom()));
  obj.Set("width", Napi::Number::New(info.Env(), rect.GetWidth()));
  obj.Set("height", Napi::Number::New(info.Env(), rect.GetHeight()));
  return obj;
}

void
Page::SetRotation(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Rotation must be number");
  }
  int rotate = value.As<Number>();
  if (rotate < 0 || rotate > 270) {
    throw Napi::Error::New(info.Env(),
                           "Rotate values must be a value of: 0, 90, 180, 270");
  }
  GetPage()->SetRotation(rotate);
}

Napi::Value
Page::GetFieldIndex(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string key = info[0].As<String>().Utf8Value();
  int index = -1;
  for (int i = 0; i < GetPage()->GetNumFields(); ++i) {
    PdfField field = GetPage()->GetField(i);
    string name = field.GetFieldName().GetStringUtf8();
    string alternate = field.GetAlternateName().GetStringUtf8();
    string mapping = field.GetMappingName().GetStringUtf8();
    if (key == name || key == alternate || key == mapping) {
      index = i;
      break;
    }
  }
  return Number::New(info.Env(), index);
}

void
Page::SetPageWidth(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Page width must be of type number");
  }
  try {
    int width = value.As<Number>();
    GetPage()->SetPageWidth(width);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Page::SetPageHeight(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Page height must be of type number");
  }
  try {
    int height = value.As<Number>();
    GetPage()->SetPageWidth(height);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Page::GetPageWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetPage()->GetPageSize().GetWidth());
}

Napi::Value
Page::GetPageHeight(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetPage()->GetPageSize().GetHeight());
}

Napi::Value
Page::GetTrimBox(const CallbackInfo& info)
{
  PdfRect trimBox = GetPage()->GetTrimBox();
  return ExtractAndApplyRectValues(info, trimBox);
}

void
Page::SetTrimBox(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsObject()) {
    throw Napi::Error::New(info.Env(), "trimbox must be an instance of Rect");
  }
  try {
    Rect* rect = Rect::Unwrap(value.As<Object>());
    auto pdfRect = rect->GetRect();
    GetPage()->SetTrimBox(*pdfRect);

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Page::GetPageNumber(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetPage()->GetPageNumber());
}

Napi::Value
Page::GetContents(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  EscapableHandleScope scope(info.Env());
  bool forAppending = info[0].As<Boolean>();
  PdfObject* contentsObj = forAppending ? GetPage()->GetContentsForAppending()
                                        : GetPage()->GetContents();
  auto objPtr = External<PdfObject>::New(info.Env(),
                                         new PdfObject(*contentsObj),
                                         [](Napi::Env env, PdfObject* data) {
                                           HandleScope scope(env);
                                           delete data;
                                           data = nullptr;
                                         });
  auto instance = Obj::constructor.New({ objPtr });
  return instance;
}

Napi::Value
Page::GetResources(const CallbackInfo& info)
{
  EscapableHandleScope scope(info.Env());
  PdfObject* resources = GetPage()->GetResources();
  auto objPtr = External<PdfObject>::New(
    info.Env(), new PdfObject(*resources), [](Napi::Env env, PdfObject* data) {
      HandleScope scope(env);
      delete data;
      data = nullptr;
    });
  auto instance = Obj::constructor.New({ objPtr });
  return instance;
}

Napi::Value
Page::GetMediaBox(const CallbackInfo& info)
{
  PdfRect mediaBox = GetPage()->GetMediaBox();
  return ExtractAndApplyRectValues(info, mediaBox);
}

Napi::Value
Page::GetBleedBox(const CallbackInfo& info)
{
  PdfRect bleedBox = GetPage()->GetBleedBox();
  return ExtractAndApplyRectValues(info, bleedBox);
}

Napi::Value
Page::GetArtBox(const CallbackInfo& info)
{
  PdfRect artBox = GetPage()->GetArtBox();
  return ExtractAndApplyRectValues(info, artBox);
}

Napi::Value
Page::GetNumAnnots(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetPage()->GetNumAnnots());
}

void
Page::DeleteAnnotation(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  try {
    GetPage()->DeleteAnnotation(index);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Page::GetAnnotation(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  EscapableHandleScope scope(info.Env());
  int index = info[0].As<Number>();
  auto ptr = GetPage()->GetAnnotation(index);
  auto instance = External<PdfAnnotation>::New(info.Env(), ptr);
  return Annotation::constructor.New({ instance });
}
Napi::Value
Page::CreateAnnotation(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_number, napi_valuetype::napi_object });
  EscapableHandleScope scope(info.Env());
  int flag = info[0].As<Number>();
  auto type = static_cast<EPdfAnnotation>(flag);
  auto obj = info[1].As<Object>();
  Rect* rect = Rect::Unwrap(obj);
  PdfAnnotation* annot = GetPage()->CreateAnnotation(type, *rect->GetRect());
  auto instance = Annotation::constructor.New(
    { External<PdfAnnotation>::New(info.Env(), annot) });
  return instance;
}
}
