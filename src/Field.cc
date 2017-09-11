//
// Created by red on 9/10/17.
//

#include "Field.h"

Napi::FunctionReference Field::constructor;

Field::Field(const CallbackInfo &info)
  : ObjectWrap(info)
{
  PoDoFo::PdfField *field = info[0].As<Napi::External<PoDoFo::PdfField>>().Data();
  _field = field;
}

Napi::Value
Field::GetType(const CallbackInfo &info)
{
  string typeStr;
  switch (_field->GetType())
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
  return Napi::String::New(info.Env(), _field->GetFieldName().GetStringUtf8());
}

Napi::Value
Field::GetAlternateName(const CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _field->GetAlternateName().GetStringUtf8());
}

Napi::Value
Field::GetMappingName(const CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _field->GetMappingName().GetStringUtf8());
}

void
Field::SetAlternateName(const CallbackInfo &info)
{
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_string});
  PdfString value(info[0].As<String>().Utf8Value().c_str());
  _field->SetAlternateName(value);
}

void
Field::SetMappingName(const CallbackInfo &info)
{
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_string});
  PdfString value(info[0].As<String>().Utf8Value().c_str());
  _field->SetMappingName(value);
}

void
Field::SetRequired(const CallbackInfo &info)
{
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_boolean});
  bool value = info[0].As<Boolean>();
  _field->SetRequired(value);
}

Napi::Value
Field::IsRequired(const CallbackInfo &info)
{
  return Napi::Boolean::New(info.Env(), _field->IsRequired());
}
