/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SignatureField.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Data.h"
#include "../base/Names.h"
#include "../base/XObject.h"
#include "../doc/Annotation.h"
#include "../doc/Document.h"
#include "../doc/Form.h"
#include "Signer.h"
#include "StreamDocument.h"

using namespace Napi;
using namespace PoDoFo;
using std::make_shared;
using std::make_unique;

namespace NoPoDoFo {

FunctionReference SignatureField::constructor; // NOLINT

/**
 * @note JS new SignatureField(annotation: IAnnotation, doc: IBase)
 * @param info
 */
SignatureField::SignatureField(const CallbackInfo& info)
  : ObjectWrap<SignatureField>(info)
{
  try {
    // Create a new Signature Field
    if (info.Length() == 2) {
      auto annot = Annotation::Unwrap(info[0].As<Object>());
      auto nObj = info[1].As<Object>();
      if (nObj.InstanceOf(Document::constructor.Value())) {
        field = make_shared<PdfSignatureField>(
          &annot->GetAnnotation(),
          Document::Unwrap(nObj)->base->GetAcroForm(),
          Document::Unwrap(nObj)->base);
      } else if (nObj.InstanceOf(StreamDocument::constructor.Value())) {
        field = make_shared<PdfSignatureField>(
          &annot->GetAnnotation(),
          StreamDocument::Unwrap(nObj)->base->GetAcroForm(),
          StreamDocument::Unwrap(nObj)->base);
      } else {
        TypeError::New(info.Env(), "Document instance required")
          .ThrowAsJavaScriptException();
        return;
      }
    } // Copy an existing Signature field.
    else if (info.Length() == 1 && info[0].IsExternal()) {
      field = make_shared<PdfSignatureField>(
        info[0].As<External<PdfAnnotation>>().Data());
    } else {
      Error::New(info.Env(), "Invalid constructor args")
        .ThrowAsJavaScriptException();
      return;
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
      InstanceMethod("setReason", &SignatureField::SetReason),
      InstanceMethod("setLocation", &SignatureField::SetLocation),
      InstanceMethod("setCreator", &SignatureField::SetCreator),
      InstanceMethod("setDate", &SignatureField::SetDate),
      InstanceMethod("addCertificateReference",
                     &SignatureField::AddCertificateReference),
      InstanceMethod("setFieldName", &SignatureField::SetFieldName),
      InstanceMethod("getObject", &SignatureField::GetSignatureObject),
      InstanceMethod("ensureSignatureObject",
                     &SignatureField::EnsureSignatureObject) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("SignatureField", ctor);
}

void
SignatureField::SetAppearanceStream(const CallbackInfo& info)
{
  XObject* xobj = XObject::Unwrap(info[0].As<Object>());
  field->SetAppearanceStream(&xobj->GetXObject());
}

void
SignatureField::SetReason(const CallbackInfo& info)
{
  string reason = info[0].As<String>().Utf8Value();
  GetField()->SetSignatureReason(PdfString(reason));
}

void
SignatureField::SetLocation(const CallbackInfo& info)
{
  string location = info[0].As<String>().Utf8Value();
  GetField()->SetSignatureLocation(PdfString(location));
}

void
SignatureField::SetCreator(const CallbackInfo& info)
{
  string creator = info[0].As<String>().Utf8Value();
  GetField()->SetSignatureCreator(PdfName(creator));
}

void
SignatureField::SetDate(const CallbackInfo& info)
{
  if (info.Length() == 1 && info[0].IsString()) {
    GetField()->SetSignatureDate(
      PdfDate(PdfString(info[0].As<String>().Utf8Value())));
  } else {
    GetField()->SetSignatureDate(PdfDate());
  }
}

void
SignatureField::SetFieldName(const CallbackInfo& info)
{
  GetField()->SetFieldName(info[0].As<String>().Utf8Value());
}

void
SignatureField::AddCertificateReference(const CallbackInfo& info)
{
  auto body =
    field->GetFieldObject()->GetOwner()->GetParentDocument()->GetObjects();
  auto it = body->begin();
  while (it != body->end()) {
    if ((*it)->IsDictionary()) {
      if ((*it)->GetDictionary().HasKey(NoPoDoFo::Name::TYPE) &&
          (*it)->GetDictionary().GetKey(NoPoDoFo::Name::TYPE)->IsName() &&
          (*it)->GetDictionary().GetKey(NoPoDoFo::Name::TYPE)->GetName() ==
            NoPoDoFo::Name::CATALOG) {
        auto flag = static_cast<PdfSignatureField::EPdfCertPermission>(
          info[0].As<Number>().Int32Value());
        GetField()->AddCertificationReference((*it), flag);
        return;
      }
    }
  }
}

Napi::Value
SignatureField::GetSignatureObject(const CallbackInfo& info)
{
  return External<PdfObject>::New(info.Env(), GetField()->GetSignatureObject());
}

Napi::Value
SignatureField::EnsureSignatureObject(const CallbackInfo& info)
{
  try {
    field->EnsureSignatureObject();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}
}
