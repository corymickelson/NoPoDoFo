//
// Created by red on 9/7/17.
//

#include "Page.h"
#include "../ErrorHandler.h"
#include "../base/Obj.h"
#include "Annotation.h"
#include "Field.h"
#include "Rect.h"

using namespace Napi;
using namespace PoDoFo;
namespace NoPoDoFo {
Napi::FunctionReference Page::constructor;

Page::Page(const CallbackInfo& info)
  : ObjectWrap(info)
{
  PoDoFo::PdfPage* pagePtr =
    info[0].As<Napi::External<PoDoFo::PdfPage>>().Data();
  PoDoFo::PdfMemDocument* parentPtr =
    info[1].As<Napi::External<PoDoFo::PdfMemDocument>>().Data();
  parent = parentPtr;
  page = pagePtr;
}
void
Page::Initialize(Napi::Env& env, Napi::Object& target)
{
  Napi::HandleScope scope(env);
  Napi::Function ctor = DefineClass(
    env,
    "Page",
    { InstanceAccessor("rotation", &Page::GetRotation, &Page::SetRotation),
      InstanceAccessor("trimBox", &Page::GetTrimBox, &Page::SetTrimBox),
      InstanceAccessor("number", &Page::GetPageNumber, nullptr),
      InstanceAccessor("width", &Page::GetPageWidth, &Page::SetPageWidth),
      InstanceAccessor("height", &Page::GetPageHeight, &Page::SetPageHeight),

      InstanceMethod("getNumFields", &Page::GetNumFields),
      InstanceMethod("getFieldsInfo", &Page::GetFields),
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
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Page", ctor);
}
Napi::Value
Page::GetRotation(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), page->GetRotation());
}
Napi::Value
Page::GetNumFields(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), page->GetNumFields());
}

Napi::Value
Page::GetFields(const CallbackInfo& info)
{
  try {
    auto fields = Napi::Array::New(info.Env());
    for (size_t i = 0; i < static_cast<size_t>(page->GetNumFields()); ++i) {
      auto obj = Napi::Object::New(info.Env());
      auto field = page->GetField(static_cast<int>(i));
      obj.Set("index", Napi::Number::New(info.Env(), i));
      Page::GetFieldObject(obj, field);
      fields.Set(static_cast<uint32_t>(i), obj);
    }
    return fields;
  } catch (PdfError& err) {
    stringstream errMsg;
    errMsg << "An error occured in PoDoFo: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), errMsg.str());
  }
}

void
Page::GetFieldObject(Napi::Object& obj, PoDoFo::PdfField& field)
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
      string comboValue =
        comboBox.GetItem(comboBox.GetSelectedItem()).GetStringUtf8();
      obj.Set("type", "ComboBox");
      obj.Set("selected", comboValue);
      break;
    }
    case ePdfField_ListBox: {
      PdfListBox listBox(field);
      string listValue =
        listBox.GetItem(listBox.GetSelectedItem()).GetStringUtf8();
      obj.Set("type", "ListBox");
      obj.Set("value", listValue);
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
  page->SetRotation(rotate);
}

Napi::Value
Page::GetFieldIndex(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string key = info[0].As<String>().Utf8Value();
  int index = -1;
  for (int i = 0; i < page->GetNumFields(); ++i) {
    PdfField field = page->GetField(i);
    string name = field.GetFieldName().GetStringUtf8();
    string alternate = field.GetAlternateName().GetStringUtf8();
    string mapping = field.GetMappingName().GetStringUtf8();
    if (key == name || key == alternate || key == mapping) {
      index = i;
      break;
    }
  }
  return Napi::Number::New(info.Env(), index);
}

void
Page::SetPageWidth(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Page width must be of type number");
  }
  try {
    int width = value.As<Number>();
    page->SetPageWidth(width);
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
    page->SetPageWidth(height);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Page::GetPageWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), page->GetPageSize().GetWidth());
}

Napi::Value
Page::GetPageHeight(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), page->GetPageSize().GetHeight());
}

Napi::Value
Page::GetTrimBox(const CallbackInfo& info)
{
  PdfRect trimBox = page->GetTrimBox();
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
    page->SetTrimBox(*pdfRect);

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Page::GetPageNumber(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), page->GetPageNumber());
}

Napi::Value
Page::GetContents(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  bool forAppending = info[0].As<Boolean>();
  PdfObject* contentsObj =
    forAppending ? page->GetContentsForAppending() : page->GetContents();
  auto objPtr = Napi::External<PdfObject>::New(info.Env(), contentsObj);
  auto instance = Obj::constructor.New({ objPtr });
  return instance;
}

Napi::Value
Page::GetResources(const CallbackInfo& info)
{
  PdfObject* resources = page->GetResources();
  auto objPtr = Napi::External<PdfObject>::New(info.Env(), resources);
  auto instance = Obj::constructor.New({ objPtr });
  return instance;
}

Napi::Value
Page::GetMediaBox(const CallbackInfo& info)
{
  PdfRect mediaBox = page->GetMediaBox();
  return ExtractAndApplyRectValues(info, mediaBox);
}

Napi::Value
Page::GetBleedBox(const CallbackInfo& info)
{
  PdfRect bleedBox = page->GetBleedBox();
  return ExtractAndApplyRectValues(info, bleedBox);
}

Napi::Value
Page::GetArtBox(const CallbackInfo& info)
{
  PdfRect artBox = page->GetArtBox();
  return ExtractAndApplyRectValues(info, artBox);
}

Napi::Value
Page::GetNumAnnots(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), page->GetNumAnnots());
}

void
Page::DeleteAnnotation(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  try {
    page->DeleteAnnotation(index);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Page::GetAnnotation(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  auto ptr = page->GetAnnotation(index);
  auto instance = Napi::External<PdfAnnotation>::New(info.Env(), ptr);
  return Annotation::constructor.New({ instance });
}
Napi::Value
Page::CreateAnnotation(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_number, napi_valuetype::napi_object });
  int flag = info[0].As<Number>();
  auto type = static_cast<EPdfAnnotation>(flag);
  auto obj = info[1].As<Object>();
  Rect* rect = Rect::Unwrap(obj);
  PdfAnnotation* annot = page->CreateAnnotation(type, *rect->GetRect());
  auto instance = Annotation::constructor.New(
    { External<PdfAnnotation>::New(info.Env(), annot) });
  return instance;
}
Page::~Page()
{
  Napi::HandleScope scope(Env());
  page = nullptr;
  parent = nullptr;
}
}
