#include "Form.h"

Form::Form(const Napi::CallbackInfo& info)
  : ObjectWrap<Form>(info)
{
  if (info.Length() != 1) {
    throw Napi::Error::New(info.Env(), "Form requires document parameter");
  }
  Object docObj = info[0].As<Object>();
  Document* document = Document::Unwrap(docObj);
  doc = document->GetDocument();
  if (!doc->GetAcroForm()) {
    throw Napi::Error::New(info.Env(), "Null Form");
  }
}

void
Form::SetNeedAppearances(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty()) {
    throw Napi::Error::New(info.Env(), "value required");
  }
  if (value.IsBoolean() == false) {
    throw Napi::Error::New(info.Env(), "requires boolean value type");
  }
  GetForm()->SetNeedAppearances(value.As<Boolean>());
}

Napi::Value
Form::GetNeedAppearances(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetForm()->GetNeedAppearances());
}

Value
Form::GetDictionary(const CallbackInfo& info)
{
  auto formDictionary = doc->GetAcroForm()->GetObject()->GetDictionary();
  auto dictInstancePtr =
    Napi::External<PdfDictionary>::New(info.Env(), &formDictionary);
  auto instance = Dictionary::constructor.New({ dictInstancePtr });
  return instance;
}

Napi::Value
Form::GetObject(const CallbackInfo& info)
{
  auto obj = doc->GetAcroForm()->GetObject();
  auto nObj = Napi::External<PdfObject>::New(info.Env(), obj);
  auto objInstance = NObject::constructor.New({ nObj });
  return objInstance;
}
