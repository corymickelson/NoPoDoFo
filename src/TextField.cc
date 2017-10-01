//
// Created by red on 9/11/17.
//

#include "TextField.h"

TextField::TextField(const CallbackInfo& info)
  : ObjectWrap(info)
{
  try {

    auto fieldObj = info[0].As<Object>();
    field = Field::Unwrap(fieldObj);
  } catch (PdfError& err) {
    stringstream msg;
    msg << "Failed to instantiate TextField. PoDoFo Error: " << err.GetError()
        << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

void
TextField::SetText(const CallbackInfo& info, const Napi::Value &value)
{
  if(!value.IsString()) {
    throw Napi::Error::New(info.Env(), "TextField value must be of type string");
  }
  string input = value.As<String>().Utf8Value();
  PoDoFo::PdfString text(input);
  TextField::GetField().SetText(text);
}

Napi::Value
TextField::GetText(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           TextField::GetField().GetText().GetStringUtf8());
}
