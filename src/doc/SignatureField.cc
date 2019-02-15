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
#include "../base/Date.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "../base/XObject.h"
#include "../doc/Annotation.h"
#include "../doc/Document.h"
#include "StreamDocument.h"

using namespace Napi;
using namespace PoDoFo;
using std::make_shared;

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

  try {
    field->EnsureSignatureObject();
    signatureInfo = SignatureInfo{ {} };
    if (field->GetSignatureObject()->IsDictionary()) {
      auto signatureDict = field->GetSignatureObject()->GetDictionary();
      if (signatureDict.HasKey(Name::BYTERANGE) &&
          signatureDict.GetKey(Name::BYTERANGE)->IsArray()) {
        PdfArray byteRange = signatureDict.GetKey(Name::BYTERANGE)->GetArray();
        for (auto& i : byteRange) {
          signatureInfo.range.emplace_back(i.GetNumber());
        }
      }
      if (signatureDict.HasKey(Name::CONTENTS) &&
          (signatureDict.GetKey(Name::CONTENTS)->IsString() ||
           signatureDict.GetKey(Name::CONTENTS)->IsHexString())) {
        PdfObject contents = *signatureDict.GetKey(Name::CONTENTS);
        string out;
        contents.ToString(out);
        signatureInfo.contents = out;
      }
    } else {
      cout << "Could not parse Signature Object" << endl;
    }
  } catch (PdfError& err) {
    cout << "Signature Object has not been set on this field" << endl;
  }
}

void
SignatureField::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "SignatureField",
    { InstanceAccessor("info", &SignatureField::GetInfo, nullptr),
      InstanceAccessor(
        "widgetAnnotation", &SignatureField::GetAnnotation, nullptr),
      InstanceAccessor("obj", &SignatureField::GetFieldObject, nullptr),
      InstanceMethod("setAppearanceStream",
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

Napi::Value
SignatureField::GetFieldObject(const Napi::CallbackInfo& info)
{
  auto o = field->GetFieldObject();
  return Obj::constructor.New({ External<PdfObject>::New(info.Env(), o) });
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
  } else if (info.Length() == 1 && info[0].IsObject() &&
             info[0].As<Object>().InstanceOf(Date::constructor.Value())) {
    GetField()->SetSignatureDate(Date::Unwrap(info[0].As<Object>())->GetDate());
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
  if (!GetField()->GetSignatureObject()) {
    cout << "Signature Object NULL" << endl;
    return info.Env().Undefined();
  }
  auto o =
    External<PdfObject>::New(info.Env(), GetField()->GetSignatureObject());
  return Obj::constructor.New({ o });
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
Napi::Value
SignatureField::GetInfo(const Napi::CallbackInfo& info)
{
  Object infoObj = Object::New(info.Env());
  if (!signatureInfo.range.empty()) {
    Napi::Array byteRange = Napi::Array::New(info.Env());
    for (uint32_t i = 0; i < signatureInfo.range.size(); i++) {
      byteRange.Set(i, Number::New(info.Env(), signatureInfo.range[i]));
    }
    infoObj.Set("byteRange", byteRange);
  } else {
    infoObj.Set("byteRange", info.Env().Undefined());
  }
  if (!signatureInfo.contents.empty()) {
    infoObj.Set("signature", String::New(info.Env(), signatureInfo.contents));
  } else {
    infoObj.Set("signature", info.Env().Undefined());
  }
  return infoObj;
}
Napi::Value
SignatureField::GetAnnotation(const Napi::CallbackInfo& info)
{
  PdfAnnotation* annot = field->GetWidgetAnnotation();
  return Annotation::constructor.New(
    { External<PdfAnnotation>::New(info.Env(), annot) });
}
}
