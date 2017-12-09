#include "Form.h"
#include "../base/Obj.h"
#include "Document.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
FunctionReference Form::constructor;

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
Form::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(env,
                              "Form",
                              { InstanceMethod("getObject", &Form::GetObject),
                                InstanceAccessor("needAppearances",
                                                 &Form::GetNeedAppearances,
                                                 &Form::SetNeedAppearances) });
  target.Set("Form", ctor);
}
void
Form::SetNeedAppearances(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty()) {
    throw Napi::Error::New(info.Env(), "value required");
  }
  if (!value.IsBoolean()) {
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
  auto nObj = Napi::External<PdfObject>::New(
    info.Env(), obj, [](Napi::Env env, PdfObject* value) {
      HandleScope scope(env);
      delete value;
    });
  auto objInstance = Obj::constructor.New({ nObj });
  return objInstance;
}
Form::~Form()
{
  if (doc != nullptr) {
    HandleScope scope(Env());
    doc = nullptr;
  }
}
}
