//
// Created by red on 9/10/17.
//

#include "Field.h"
#include "Document.h"


Field::Field(const CallbackInfo &info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 2, {napi_valuetype::napi_object, napi_valuetype::napi_number});
  auto pageObj = info[0].As<Object>();
  int fieldIndex = info[1].As<Number>();
  Page *page = Page::Unwrap(pageObj);
  _page = page->GetPage();
  PdfField pdfField = _page->GetField(fieldIndex);
}

Napi::Value
Field::GetType(const CallbackInfo &info)
{
  string typeStr;
  switch (Field::GetField().GetType())
  {
    case PoDoFo::EPdfField::ePdfField_CheckBox:typeStr = "CheckBox";
      break;
    case PoDoFo::EPdfField::ePdfField_ComboBox:typeStr = "ComboBox";
      break;
    case PoDoFo::EPdfField::ePdfField_ListBox:typeStr = "ListBox";
      break;
    case PoDoFo::EPdfField::ePdfField_PushButton:typeStr = "PushButton";
      break;
    case PoDoFo::EPdfField::ePdfField_RadioButton:typeStr = "RadioButton";
      break;
    case PoDoFo::EPdfField::ePdfField_Signature:typeStr = "Signature";
      break;
    case PoDoFo::EPdfField::ePdfField_TextField:typeStr = "TextField";
      break;
    case PoDoFo::EPdfField::ePdfField_Unknown:throw Napi::Error::New(info.Env(), "Pdf Field Unknown");
  }
  return Napi::String::New(info.Env(), typeStr);
}

Napi::Value
Field::GetFieldName(const CallbackInfo &info)
{
  return Napi::String::New(info.Env(), Field::GetField().GetFieldName().GetStringUtf8());
}

Napi::Value
Field::GetAlternateName(const CallbackInfo &info)
{
  return Napi::String::New(info.Env(), Field::GetField().GetAlternateName().GetStringUtf8());
}

Napi::Value
Field::GetMappingName(const CallbackInfo &info)
{
  return Napi::String::New(info.Env(), Field::GetField().GetMappingName().GetStringUtf8());
}

void
Field::SetAlternateName(const CallbackInfo &info)
{
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_string});
  PdfString value(info[0].As<String>().Utf8Value().c_str());
  Field::GetField().SetAlternateName(value);
}

void
Field::SetMappingName(const CallbackInfo &info)
{
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_string});
  PdfString value(info[0].As<String>().Utf8Value().c_str());
  Field::GetField().SetMappingName(value);
}

void
Field::SetRequired(const CallbackInfo &info)
{
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_boolean});
  bool value = info[0].As<Boolean>();
  Field::GetField().SetRequired(value);
}

Napi::Value
Field::IsRequired(const CallbackInfo &info)
{
  try
  {
    Napi::Boolean::New(info.Env(), Field::GetField().IsRequired());
  }
  catch (PdfError &err)
  {
    stringstream msg;
    msg << "PoDoFo Failure: " << err.GetError() << " " << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}
