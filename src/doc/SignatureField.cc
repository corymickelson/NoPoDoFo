//
// Created by red on 9/26/17
//

#include "SignatureField.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../doc/Annotation.h"
#include "../doc/Document.h"
#include "../doc/Form.h"

using namespace Napi;
using namespace PoDoFo;

SignatureField::SignatureField(const CallbackInfo& info)
  : ObjectWrap<SignatureField>(info)
{
  try {
    if (info.Length() == 3) {
      AssertFunctionArgs(info,
                         3,
                         { napi_valuetype::napi_object,
                           napi_valuetype::napi_object,
                           napi_valuetype::napi_object });
      auto annot = Annotation::Unwrap(info[0].As<Object>());
      auto form = Form::Unwrap(info[1].As<Object>());
      auto doc = Document::Unwrap(info[2].As<Object>());
      field = new PdfSignatureField(
        annot->GetAnnotation(), form->GetForm(), doc->GetDocument());

    } else if (info.Length() == 1) {
      AssertFunctionArgs(info, 1, { napi_valuetype::napi_external });
      field = info[0].As<External<PdfSignatureField>>().Data();
    }
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
SignatureField::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "SignatureField",
    { InstanceMethod("setAppearanceStream",
                     &SignatureField::SetAppearanceStream),
      InstanceMethod("setSignature", &SignatureField::SetSignature),
      InstanceMethod("setReason", &SignatureField::SetReason),
      InstanceMethod("setLocation", &SignatureField::SetLocation),
      InstanceMethod("setCreator", &SignatureField::SetCreator),
      InstanceMethod("setDate", &SignatureField::SetDate),
      InstanceMethod("addCertificateReference",
                     &SignatureField::AddCertificateReference),
      InstanceMethod("getObject", &SignatureField::GetSignatureObject),
      InstanceMethod("ensureSignatureObject",
                     &SignatureField::EnsureSignatureObject) });
  target.Set("SignatureField", ctor);
}

void
SignatureField::SetAppearanceStream(const CallbackInfo& info)
{}

void
SignatureField::SetSignature(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string data = info[0].As<String>().Utf8Value();
  try {
    signatureBuffer = new PdfData(data.c_str());
    field->SetSignature(*signatureBuffer);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
SignatureField::SetReason(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string reason = info[0].As<String>().Utf8Value();
  field->SetSignatureReason(PdfString(reason));
}

void
SignatureField::SetLocation(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string location = info[0].As<String>().Utf8Value();
  field->SetSignatureLocation(PdfString(location));
}

void
SignatureField::SetCreator(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string creator = info[0].As<String>().Utf8Value();
  field->SetSignatureCreator(PdfName(creator));
}

void
SignatureField::SetDate(const CallbackInfo& info)
{
  field->SetSignatureDate(PdfDate());
}

void
SignatureField::SetFieldName(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  field->SetFieldName(info[0].As<String>().Utf8Value());
}

void
SignatureField::AddCertificateReference(const CallbackInfo& info)
{}

Value
SignatureField::GetSignatureObject(const CallbackInfo& info)
{
  return External<PdfObject>::New(info.Env(), field->GetSignatureObject());
}

Value
SignatureField::EnsureSignatureObject(const CallbackInfo& info)
{
  try {
    field->EnsureSignatureObject();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
