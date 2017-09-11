//
// Created by red on 9/11/17.
//

#include "TextField.h"
#include "ValidateArguments.h"

Napi::FunctionReference TextField::constructor;

TextField::TextField(const CallbackInfo &info) : ObjectWrap(info)
{
  PoDoFo::PdfField *base = info[0].As<Napi::External<PoDoFo::PdfField>>().Data();
  _base = base;
  PoDoFo::PdfTextField field(*base);
  _field = &field;
}

void
TextField::SetText(const CallbackInfo &info)
{
  AssertFunctionArgs(info, 1, {napi_valuetype::napi_string});
  string input = info[0].As<String>().Utf8Value();
  PoDoFo::PdfString value(input);
  _field->SetText(value);
}
Napi::Value
TextField::GetText(const CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _field->GetText().GetStringUtf8());
}
