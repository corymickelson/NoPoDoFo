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

#include "Form.h"
#include "../ErrorHandler.h"
#include "../base/Dictionary.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "Document.h"
#include "StreamDocument.h"
#include <iostream>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::ostringstream;
using std::stringstream;

namespace NoPoDoFo {

FunctionReference Form::constructor; // NOLINT

Form::Form(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , create(info[1].As<Boolean>())
  , doc(info[0].IsExternal()
          ? *info[0].As<External<BaseDocument>>().Data()->base
          : *(info[0].As<Object>().InstanceOf(Document::constructor.Value())
                ? Document::Unwrap(info[0].As<Object>())->base
                : StreamDocument::Unwrap(info[0].As<Object>())->base))
{}

void
Form::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Form",
    { InstanceAccessor("dictionary", &Form::GetFormDictionary, nullptr),
      InstanceAccessor("SigFlags", &Form::SigFlags, &Form::SetSigFlags),
      InstanceAccessor("needAppearances",
                       &Form::GetNeedAppearances,
                       &Form::SetNeedAppearances),
      InstanceAccessor(
        "DA", &Form::GetDefaultAppearance, &Form::SetDefaultAppearance),
      InstanceAccessor(
        "CO", &Form::GetCalculationOrder, &Form::SetCalculationOrder),
      InstanceAccessor("DR", &Form::GetResource, &Form::SetResource) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
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
Form::GetFormDictionary(const CallbackInfo& info)
{
  PdfDictionary& obj = GetForm()->GetObject()->GetDictionary();
  auto ptr = Dictionary::constructor.New(
    { External<PdfDictionary>::New(info.Env(), &obj),
      Number::New(info.Env(), 1) });
  return ptr;
}

Napi::Value
Form::SigFlags(const CallbackInfo& info)
{
  long flag = 0;
  if (GetDictionary()->HasKey(Name::SIG_FLAGS)) {
    flag = GetDictionary()->GetKey(Name::SIG_FLAGS)->GetNumber();
  }
  return Number::New(info.Env(), flag);
}

void
Form::SetSigFlags(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    TypeError::New(info.Env(), "value must be of type number")
      .ThrowAsJavaScriptException();
  } else {
    pdf_int64 f = value.As<Number>().Int64Value();
    if (f < 1 || f > 3) {
      RangeError::New(info.Env(),
                      "SigFlags value can only be 1, 2, or 3. See NPDFSigFlags "
                      "for more information.")
        .ThrowAsJavaScriptException();
    } else {
      if (GetDictionary()->HasKey(Name::SIG_FLAGS)) {
        GetDictionary()->RemoveKey(Name::SIG_FLAGS);
      }
      try {
        GetDictionary()->AddKey(Name::SIG_FLAGS, PdfVariant(f));
      } catch (PdfError& err) {
        ErrorHandler(err, info);
      }
    }
  }
}

Napi::Value
Form::GetDefaultAppearance(const CallbackInfo& info)
{
  if (GetDictionary()->HasKey(Name::DA)) {
    auto da = GetDictionary()->GetKey(Name::DA);
    if (da->IsString()) {
      return String::New(info.Env(), da->GetString().GetStringUtf8());
    } else {
      return String::New(info.Env(), "");
    }
  } else {
    return info.Env().Null();
  }
}

void
Form::SetDefaultAppearance(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsString()) {
    Error::New(info.Env(), "Value must be a string")
      .ThrowAsJavaScriptException();
  } else {
    try {
      if (GetDictionary()->HasKey(Name::DA)) {
        GetDictionary()->RemoveKey(Name::DA);
      }
      GetDictionary()->AddKey(PdfName(Name::DA),
                              PdfString(value.As<String>().Utf8Value()));
    } catch (PdfError& err) {
      ErrorHandler(err, info);
    }
  }
}

Napi::Value
Form::GetResource(const CallbackInfo& info)
{
  if (GetDictionary()->HasKey(Name::DR)) {
    PdfObject* drObj = GetDictionary()->GetKey(Name::DR);
    if (drObj->IsReference()) {
      drObj = doc.GetObjects()->GetObject(drObj->GetReference());
    }
    return Dictionary::constructor.New(
      { External<PdfObject>::New(info.Env(), drObj),
        Number::New(info.Env(), 0) });
  }
  return info.Env().Null();
}

void
Form::SetResource(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.As<Object>().InstanceOf(Dictionary::constructor.Value())) {
    TypeError::New(
      info.Env(),
      "Default resource must be an instance of NoPoDoFo::Dictionary")
      .ThrowAsJavaScriptException();
  } else {
    try {
      auto formDict = GetForm()->GetObject()->GetDictionary();
      auto dr = Obj::Unwrap(value.As<Object>());
      if (dr->GetObject().GetDataType() != ePdfDataType_Dictionary)
        TypeError::New(
          info.Env(),
          "Default resource must be an instance of NoPoDoFo::Dictionary")
          .ThrowAsJavaScriptException();
      if (formDict.HasKey(Name::DR))
        formDict.RemoveKey(Name::DR);
      formDict.AddKey(PdfName(Name::DR), dr->GetObject().Reference());
    } catch (PdfError& err) {
      ErrorHandler(err, info);
    }
  }
}

Napi::Value
Form::GetCalculationOrder(const CallbackInfo& info)
{
  Array js = Array::New(info.Env());
  uint32_t n = 0;
  auto d = GetForm()->GetObject()->GetDictionary();
  if (d.HasKey(Name::CO)) {
    auto co = d.GetKey(Name::CO);
    if (!co->IsArray()) {
      TypeError::New(info.Env(), "CO expected an array");
    } else {
      auto arr = co->GetArray();
      for (const auto& item : arr) {
        if (item.IsReference()) {
          auto value = doc.GetObjects()->GetObject(item.GetReference());
          auto nObj = Obj::constructor.New(
            { External<PdfObject>::New(info.Env(), value) });
          if (!value->IsDictionary()) {
            js.Set(n, nObj);
            n++;
          } else {
            js.Set(n,
                   Dictionary::constructor.New(
                     { External<PdfObject>::New(info.Env(), value),
                       Number::New(info.Env(), 0) }));
            n++;
          }
        }
      }
    }
  }
  return js;
}

void
Form::SetCalculationOrder(const CallbackInfo& info, const Napi::Value& value)
{
  Error::New(info.Env(), "Not yet implemented").ThrowAsJavaScriptException();
}

void
Form::AddFont(PdfFont* font)
{

  if (GetDictionary()->HasKey(Name::DR)) {
    PdfObject* drObj = GetDictionary()->GetKey(Name::DR);
    //    auto dr = Dictionary::Resolve(doc->GetDocument(), drObj);
    auto fd = drObj->GetDictionary().GetKey(Name::FONT)->IsReference()
                ? doc.GetObjects()
                    ->GetObject(
                      drObj->GetDictionary().GetKey(Name::FONT)->GetReference())
                    ->GetDictionary()
                : drObj->GetDictionary().GetKey(Name::FONT)->GetDictionary();
    if (!fd.HasKey(font->GetIdentifier())) {
      fd.AddKey(font->GetIdentifier(), font->GetObject()->Reference());
    }
  }
}
}
