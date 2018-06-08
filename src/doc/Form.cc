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
{
  if (info[0].IsObject() &&
      info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
    cout << "Using Document instance" << endl;
    doc = Document::Unwrap(info[0].As<Object>())->GetBaseDocument();
    isMemDoc = true;
  } else if (info[0].Type() == napi_external) {
    cout << "Using BaseDocument" << endl;
    auto base = info[0].As<External<BaseDocument>>().Data();
    isMemDoc = !base->created();
    doc = base->GetBaseDocument();
  }
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
      InstanceAccessor("DR", &Form::GetResource, &Form::SetResource),
      InstanceAccessor("Fonts", &Form::GetFont, &Form::SetFont) });
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
  doc->GetAcroForm()->SetNeedAppearances(value.As<Boolean>());
}

Napi::Value
Form::GetNeedAppearances(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(),
                            doc->GetAcroForm()->GetNeedAppearances());
}

Napi::Value
Form::GetFormDictionary(const CallbackInfo& info)
{
  auto obj = doc->GetAcroForm()->GetObject();
  auto ptr =
    Dictionary::constructor.New({ External<PdfObject>::New(info.Env(), obj) });
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
      drObj = doc->GetObjects()->GetObject(drObj->GetReference());
    }
    return Dictionary::constructor.New(
      { External<PdfObject>::New(info.Env(), drObj) });
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
      auto formDict = doc->GetAcroForm()->GetObject()->GetDictionary();
      auto dr = Obj::Unwrap(value.As<Object>());
      if (dr->GetObject()->GetDataType() != ePdfDataType_Dictionary)
        TypeError::New(
          info.Env(),
          "Default resource must be an instance of NoPoDoFo::Dictionary")
          .ThrowAsJavaScriptException();
      if (formDict.HasKey(Name::DR))
        formDict.RemoveKey(Name::DR);
      formDict.AddKey(PdfName(Name::DR), dr->GetObject()->Reference());
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
  auto d = doc->GetAcroForm()->GetObject()->GetDictionary();
  if (d.HasKey(Name::CO)) {
    auto co = d.GetKey(Name::CO);
    if (!co->IsArray()) {
      TypeError::New(info.Env(), "CO expected an array");
    } else {
      auto arr = co->GetArray();
      for (const auto& item : arr) {
        if (item.IsReference()) {
          auto value = doc->GetObjects()->GetObject(item.GetReference());
          auto nObj = Obj::constructor.New(
            { External<PdfObject>::New(info.Env(), value) });
          if (!value->IsDictionary()) {
            js.Set(n, nObj);
            n++;
          } else {
            js.Set(n,
                   Dictionary::constructor.New(
                     { External<PdfObject>::New(info.Env(), value) }));
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

/**
 * @todo: fix upcasting for PdfMemDocument::GetFont
 * @param info
 * @return
 */
Napi::Value
Form::GetFont(const CallbackInfo& info)
{
  if (!isMemDoc) {
    TypeError::New(info.Env(),
                   "GetFont is only available from a PdfMemDocument")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  auto memDoc = std::static_pointer_cast<PdfMemDocument>(doc);
  auto js = Array::New(info.Env());
  uint32_t n = 0;
  if (GetDictionary()->HasKey(Name::DR)) {
    PdfObject* drObj = GetDictionary()->GetKey(Name::DR);
    auto fd = drObj->GetDictionary().GetKey(Name::FONT)->GetDictionary();
    for (auto k : fd.GetKeys()) {
      PdfObject* fontObject;
      if (k.second->IsReference()) {
        fontObject = doc->GetObjects()->GetObject(k.second->GetReference());
      } else {
        fontObject = k.second;
      }
      auto font = memDoc->GetFont(fontObject);
      cout << "Font: " << font->GetFontMetrics()->GetFontname() << endl;
      js.Set(
        n, Font::constructor.New({ External<PdfFont>::New(info.Env(), font) }));
      n++;
    }
    return js;
  }
  return info.Env().Null();
}

void
Form::SetFont(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsArray()) {
    TypeError::New(info.Env(), "Requires an array of Font objects")
      .ThrowAsJavaScriptException();
  }
  auto arr = value.As<Array>();
  for (uint32_t i = 0; i < arr.Length(); i++) {
    auto item = arr.Get(i);
    if (item.As<Object>().InstanceOf(Font::constructor.Value())) {
      auto font = Font::Unwrap(item.As<Object>());
      AddFont(font->GetFont());
    }
  }
}

void
Form::AddFont(PdfFont* font)
{

  if (GetDictionary()->HasKey(Name::DR)) {
    PdfObject* drObj = GetDictionary()->GetKey(Name::DR);
    //    auto dr = Dictionary::Resolve(doc->GetDocument(), drObj);
    auto fd = drObj->GetDictionary().GetKey(Name::FONT)->IsReference()
                ? doc->GetObjects()
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
