//
// Created by red on 9/6/17.
//

#include "CheckBox.h"

namespace NoPoDoFo {
FunctionReference CheckBox::constructor;

CheckBox::CheckBox(const CallbackInfo& info)
  : ObjectWrap<CheckBox>(info)
{
  if (info.Length() == 0) {
    throw Napi::Error::New(info.Env(),
                           "Requires a single argument of type Field");
  }
  auto fieldObj = info[0].As<Object>();
  Field* field = Field::Unwrap(fieldObj);
  box = new PdfCheckBox(field->GetField());
}

Napi::Value
CheckBox::IsChecked(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), box->IsChecked());
}

void
CheckBox::SetChecked(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::TypeError::New(info.Env(),
                               "CheckBox.checked requires boolean value");
  }
  bool checked = value.As<Boolean>();
  box->SetChecked(checked);
}
}
