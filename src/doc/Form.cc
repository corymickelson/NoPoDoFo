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
#include "Font.h"
#include <iostream>

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::stringstream;

FunctionReference Form::constructor; // NOLINT

Form::Form(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , create(info[1].As<Boolean>())
  , doc(Document::Unwrap(info[0].As<Object>()))
{
  form = new PdfAcroForm(doc->GetDocument(),
                         doc->GetDocument()->GetAcroForm(create)->GetObject());
}
Form::~Form()
{
  cout << "Destructing Form" << endl;
  delete form;
  doc = nullptr;
}
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
      InstanceAccessor("DR", &Form::GetDR, &Form::SetDR),
      InstanceAccessor("fonts", &Form::GetFont, nullptr) });
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
  auto obj = GetForm()->GetObject();
  auto instance =
    External<PdfDictionary>::New(info.Env(), &obj->GetDictionary());
  return Dictionary::constructor.New({ doc->Value(), instance });
}

Napi::Value
Form::SigFlags(const CallbackInfo& info)
{
  int flag = 0;
  if (GetForm()->GetObject()->GetDictionary().HasKey(Name::SIG_FLAGS)) {
    flag = static_cast<int>(
      GetForm()->GetObject()->GetDictionary().GetKeyAsLong(Name::SIG_FLAGS));
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
    double f = value.As<Number>().DoubleValue();
    if (f < 1 || f > 3) {
      RangeError::New(info.Env(),
                      "SigFlags value can only be 1, 2, or 3. See NPDFSigFlags "
                      "for more information.")
        .ThrowAsJavaScriptException();
    } else {
      if (GetForm()->GetObject()->GetDictionary().HasKey(Name::SIG_FLAGS)) {
        try {

          GetForm()->GetObject()->GetDictionary().RemoveKey(Name::SIG_FLAGS);
          GetForm()->GetObject()->GetDictionary().AddKey(Name::SIG_FLAGS,
                                                         PdfObject(f));
        } catch (PdfError& err) {
          ErrorHandler(err, info);
        }
      }
    }
  }
}

Napi::Value
Form::GetDefaultAppearance(const CallbackInfo& info)
{
  if (GetForm()->GetObject()->GetDictionary().HasKey(Name::DA)) {
    auto da = GetForm()->GetObject()->GetDictionary().GetKey(Name::DA);
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
      auto dict = GetForm()->GetObject()->GetDictionary();
      if (dict.HasKey(Name::DA)) {
        dict.RemoveKey(Name::DA);
        dict.AddKey(PdfName(Name::DA),
                    PdfString(value.As<String>().Utf8Value()));
      }
    } catch (PdfError& err) {
      ErrorHandler(err, info);
    }
  }
}

Napi::Value
Form::GetDR(const CallbackInfo& info)
{
  auto d = DR();
  if (d) {
    return Dictionary::constructor.New(
      { doc->Value(), External<PdfDictionary>::New(info.Env(), d) });
  }
  //  if (GetForm()->GetObject()->GetDictionary().HasKey(Name::DR)) {
  //    PdfObject* drObj =
  //    GetForm()->GetObject()->GetDictionary().GetKey(Name::DR); if
  //    (drObj->IsDictionary()) {
  //      return Dictionary::constructor.New(
  //        { doc->Value(),
  //          External<PdfDictionary>::New(info.Env(), &drObj->GetDictionary())
  //          });
  //    } else if (drObj->IsReference()) {
  //      drObj =
  //      doc->GetDocument()->GetObjects().GetObject(drObj->GetReference()); if
  //      (drObj->IsDictionary()) {
  //        return Dictionary::constructor.New(
  //          { doc->Value(),
  //            External<PdfDictionary>::New(info.Env(),
  //                                         &drObj->GetDictionary()) });
  //      }
  //    }
  //  }
  return info.Env().Null();
}

void
Form::SetDR(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.As<Object>().InstanceOf(Dictionary::constructor.Value())) {
    TypeError::New(
      info.Env(),
      "Default resource must be an instance of NoPoDoFo::Dictionary")
      .ThrowAsJavaScriptException();
  } else {
    try {
      auto formDict = GetForm()->GetObject()->GetDictionary();
      auto dr = Dictionary::Unwrap(value.As<Object>())->GetDictionary();
      if (formDict.HasKey(Name::DR))
        formDict.RemoveKey(Name::DR);
      formDict.AddKey(PdfName(Name::DR), dr);
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
      for (auto item : arr) {
        if (item.IsReference()) {
          auto value = new PdfObject(
            *doc->GetDocument()->GetObjects().GetObject(item.GetReference()));
          if (!value->IsDictionary()) {
            js.Set(n,
                   Obj::constructor.New({ External<PdfObject>::New(
                     info.Env(), value, [](Napi::Env env, PdfObject* data) {
                       cout << "Finalizing Obj PdfObject from Form CO" << endl;
                       HandleScope scope(env);
                       delete data;
                       data = nullptr;
                     }) }));
            n++;
          } else {
            js.Set(n,
                   Dictionary::constructor.New(
                     { doc->Value(),
                       External<PdfDictionary>::New(
                         info.Env(), &value->GetDictionary()) }));
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

Napi::Value
Form::GetFont(const CallbackInfo& info)
{
  auto js = Array::New(info.Env());
  uint32_t n = 0;
  auto dr = DR();
  if (dr) {
    if (dr->HasKey(Name::FONT)) {
      auto f = dr->GetKey(Name::FONT);
      auto fd = Dictionary::TryResolve(doc->GetDocument(), f);
      if (fd) {
        auto fdKeys = fd->GetKeys();
        for (auto k : fdKeys) {
          auto fk = Dictionary::TryResolve(doc->GetDocument(), k.second);
          if (fk && fk->GetKey(Name::TYPE)->IsName() &&
              fk->GetKey(Name::TYPE)->GetName() == Name::FONT) {
            js.Set(n,
                   Font::constructor.New(
                     { doc->Value(),
                       External<PdfObject>::New(info.Env(), k.second),
                       Boolean::New(info.Env(), true) }));
          }
        }
        return js;
      }
    }
  }
  return info.Env().Null();
}

PdfDictionary*
Form::DR()
{
  if (GetForm()->GetObject()->GetDictionary().HasKey(Name::DR)) {
    PdfObject* drObj = GetForm()->GetObject()->GetDictionary().GetKey(Name::DR);
    return Dictionary::TryResolve(doc->GetDocument(), drObj);
    //    if (drObj->IsReference()) {
    //      drObj =
    //      doc->GetDocument()->GetObjects().GetObject(drObj->GetReference());
    //    }
    //    return &drObj->GetDictionary();
    //  } else {
    //    return nullptr;
  } else
    return nullptr;
}
}
