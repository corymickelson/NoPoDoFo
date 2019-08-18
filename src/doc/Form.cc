/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2019
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
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Dictionary.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "../base/Ref.h"
#include "../base/XObject.h"
#include "Document.h"
#include "StreamDocument.h"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::ostringstream;
using std::stringstream;

namespace NoPoDoFo {

FunctionReference Form::Constructor; // NOLINT

Form::Form(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , Create(info[1].As<Boolean>())
  , Doc(info[0].IsExternal()
          ? *info[0].As<External<BaseDocument>>().Data()->Base
          : *(info[0].As<Object>().InstanceOf(Document::Constructor.Value())
                ? Document::Unwrap(info[0].As<Object>())->Base
                : StreamDocument::Unwrap(info[0].As<Object>())->Base))
{
  Log = spdlog::get("Log");
}

Form::~Form()
{
  if(Log != nullptr) Log->debug("Form Cleanup");
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
      InstanceAccessor("DR", &Form::GetResource, &Form::SetResource) });
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Form", ctor);
}
void
Form::SetNeedAppearances(const CallbackInfo& info, const JsValue& value)
{
  if (value.IsEmpty()) {
    throw Napi::Error::New(info.Env(), "value required");
  }
  if (!value.IsBoolean()) {
    throw Napi::Error::New(info.Env(), "requires boolean value type");
  }
  GetForm()->SetNeedAppearances(value.As<Boolean>());
}

JsValue
Form::GetNeedAppearances(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetForm()->GetNeedAppearances());
}

JsValue
Form::GetFormDictionary(const CallbackInfo& info)
{
  PdfDictionary& obj = GetForm()->GetObject()->GetDictionary();
  auto ptr = Dictionary::Constructor.New(
    { External<PdfDictionary>::New(info.Env(), &obj),
      Number::New(info.Env(), 1) });
  return ptr;
}

JsValue
Form::SigFlags(const CallbackInfo& info)
{
  long flag = 0;
  if (GetDictionary()->HasKey(Name::SIG_FLAGS)) {
    flag = GetDictionary()->GetKey(Name::SIG_FLAGS)->GetNumber();
  }
  return Number::New(info.Env(), flag);
}

void
Form::SetSigFlags(const CallbackInfo& info, const JsValue& value)
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

JsValue
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
Form::SetDefaultAppearance(const CallbackInfo& info, const JsValue& value)
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

JsValue
Form::GetResource(const CallbackInfo& info)
{
  if (GetDictionary()->HasKey(Name::DR)) {
    PdfObject* drObj = GetDictionary()->GetKey(Name::DR);
    if (drObj->IsReference()) {
      drObj = Doc.GetObjects()->GetObject(drObj->GetReference());
    }
    return Dictionary::Constructor.New(
      { External<PdfObject>::New(info.Env(), drObj),
        Number::New(info.Env(), 0) });
  }
  return info.Env().Null();
}

void
Form::SetResource(const CallbackInfo& info, const JsValue& value)
{
  if (!value.As<Object>().InstanceOf(Dictionary::Constructor.Value())) {
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

JsValue
Form::GetCalculationOrder(const CallbackInfo& info)
{
  Array js = Array::New(info.Env());
  uint32_t n = 0;
  auto d = GetForm()->GetObject()->GetDictionary();
  if (d.HasKey(Name::CO)) {
    auto co = d.GetKey(Name::CO);
    if (!co->IsArray()) {
      TypeError::New(info.Env(), "CO expected an array").ThrowAsJavaScriptException();
    } else {
      auto arr = co->GetArray();
      for (const auto& item : arr) {
        if (item.IsReference()) {
          auto value = Doc.GetObjects()->GetObject(item.GetReference());
          auto nObj = Obj::Constructor.New(
            { External<PdfObject>::New(info.Env(), value) });
          if (!value->IsDictionary()) {
            js.Set(n, nObj);
            n++;
          } else {
            js.Set(n,
                   Dictionary::Constructor.New(
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
Form::SetCalculationOrder(const CallbackInfo& info, const JsValue& value)
{
  Error::New(info.Env(), "Not yet implemented").ThrowAsJavaScriptException();
}

/**
 * @brief Refresh the appearance stream for all fields on all pages
 * @param info -
 * PdfXObject
 */
void
Form::RefreshAppearances(const CallbackInfo& info)
{
  PdfXObject* xApp = nullptr;
  vector<PdfField*> fields;
  vector<PdfAnnotation*> annots;
  vector<int> opts =
    AssertCallbackInfo(info, { { 0, { napi_object, napi_external } } });
  if (opts[0] == 0) {
    auto obj = info[0].As<Object>();
    if (obj.InstanceOf(Ref::Constructor.Value())) {
      auto r = Ref::Unwrap(obj);
      PdfObject* appObj = Doc.GetObjects()->GetObject(*r->Self);
      PdfXObject x(appObj);
      xApp = &x;
    } else if (obj.InstanceOf(XObject::Constructor.Value())) {
      auto xo = XObject::Unwrap(obj);
      xApp = &xo->GetXObject();
    } else {
      TypeError::New(info.Env(),
                     "Reference to an XObject or an XObject is required")
        .ThrowAsJavaScriptException();
    }
  } else if (opts[0] == 1) {
    xApp = info[0].As<External<PdfXObject>>().Data();
  }
  if (!xApp) {
    TypeError::New(info.Env(),
                   "Reference to an XObject or an XObject is required")
      .ThrowAsJavaScriptException();
  }
  if (!GetDictionary()->HasKey(Name::FIELDS)) {
    return;
  }
  for (int i = 0; i < Doc.GetPageCount(); i++) {
    PdfPage* page = Doc.GetPage(i);
    for (int ii = 0; ii < page->GetNumFields(); ++ii) {
      if (page->GetField(ii).GetWidgetAnnotation()->HasAppearanceStream()) {
        PdfField iiField = page->GetField(ii);
        fields.emplace_back(&iiField);
      }
    }
    for (int ii = 0; ii < page->GetNumAnnots(); ++ii) {
      PdfAnnotation* a = page->GetAnnotation(ii);
      if (a->GetType() == ePdfAnnotation_Widget && a->HasAppearanceStream()) {
        annots.push_back(a);
      }
    }
  }
  for (auto f : fields) {
    if (f->GetType() == ePdfField_ListBox ||
        f->GetType() == ePdfField_ComboBox) {
      f->GetWidgetAnnotation()->SetAppearanceStream(xApp);
    } else if (f->GetType() == ePdfField_CheckBox ||
               f->GetType() == ePdfField_PushButton ||
               f->GetType() == ePdfField_RadioButton) {
      f->GetWidgetAnnotation()->SetAppearanceStream(xApp);
      //    } else if (f->GetType() == ePdfField_Signature) {
      //      auto [da, ap] = apKeys();
    } else if (f->GetType() == ePdfField_TextField) {
      f->GetWidgetAnnotation()->SetAppearanceStream(xApp);
    } else { // ePdfField_Unknown
      Error::New(info.Env(), "Unknown Field Type").ThrowAsJavaScriptException();
    }
  }
}

/**
 * @brief Get AP, V, DV, and DA keys from a field's appearance stream
 * Only keys that exist in the stream are returned in the map.
 * @param f
 * @return std::map<std::string, PoDoFo::PdfObject*>
 */
std::map<std::string, PoDoFo::PdfObject*>
Form::GetFieldAPKeys(PoDoFo::PdfField* f)
{
  std::map<string, PdfObject*> keys;
  if (!f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(
        Name::DA)) {
    f->GetWidgetAnnotation()->GetObject()->GetDictionary().AddKey(Name::DA,
                                                                  PdfString());
  }
  keys.insert(std::pair<string, PdfObject*>(
    Name::DA,
    f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::DA)));
  if (!f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(
        Name::AP)) {

    f->GetWidgetAnnotation()->GetObject()->GetDictionary().AddKey(Name::AP,
                                                                  PdfObject());
  }
  keys.insert(std::pair<string, PdfObject*>(
    Name::AP,
    f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::AP)));
  if (f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(Name::V)) {
    keys.insert(std::pair<string, PdfObject*>(
      Name::V,
      f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::V)));
  }
  if (f->GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(Name::DV)) {
    keys.insert(std::pair<string, PdfObject*>(
      Name::DV,
      f->GetWidgetAnnotation()->GetObject()->MustGetIndirectKey(Name::DV)));
  }
  return keys;
}
}
