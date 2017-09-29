#include "Form.h"
#include "Document.h"
#include "NObject.h"

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

Napi::Value
Form::GetObject(const CallbackInfo& info)
{
  auto obj = doc->GetAcroForm()->GetObject();
  auto nObj = Napi::External<PdfObject>::New(info.Env(), obj);
  auto objInstance = NObject::constructor.New({ nObj });
  return objInstance;
}
