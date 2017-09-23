#include "Form.h"

Form::Form(const Napi::CallbackInfo& info)
  : ObjectWrap<Form>(info)
{
  if (info.Length() != 1) {
    throw Napi::Error::New(info.Env(), "Form requires document parameter");
  }
  Object docObj = info[0].As<Object>();
  Document* document = Document::Unwrap(docObj);
  _form = document->GetDocument()->GetAcroForm();
}
