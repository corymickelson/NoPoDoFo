//
// Created by red on 9/11/17.
//

#include "TextField.h"
#include "ValidateArguments.h"

// Napi::FunctionReference TextField::constructor;

TextField::TextField(const CallbackInfo& info)
  : ObjectWrap(info)
{
  try {
    Object fieldObj = info[0].As<Object>();
    _field = Field::Unwrap(fieldObj);
    //    auto textFieldPtr =
    //    info[0].As<External<PoDoFo::PdfTextField>>().Data();
    //    _field = textFieldPtr;
  } catch (PdfError& err) {
    stringstream msg;
    msg << "Failed to instantiate TextField. PoDoFo Error: " << err.GetError()
        << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

void
TextField::SetText(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string input = info[0].As<String>().Utf8Value();
  PoDoFo::PdfString value(input);
  //  _field->SetText(value);
  TextField::GetField().SetText(value);
}

Napi::Value
TextField::GetText(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           TextField::GetField().GetText().GetStringUtf8());
}
