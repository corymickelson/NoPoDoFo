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

#include "TextField.h"
#include "../ErrorHandler.h"
#include "Document.h"
#include "StreamDocument.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference TextField::Constructor; // NOLINT

/**
 * @brief TextField::TextField
 * @param info
 */
TextField::TextField(const CallbackInfo& info)
  : ObjectWrap(info)
  , Field(ePdfField_TextField, info)
  , Self(GetField())
{
  DbgLog = spdlog::get("DbgLog");
  if (info[info.Length() - 1].IsObject() &&
      !info[info.Length() - 1].As<Object>().InstanceOf(
        Document::Constructor.Value()) &&
      !info[info.Length() - 1].As<Object>().InstanceOf(
        StreamDocument::Constructor.Value())) {
    auto opts = info[info.Length() - 1].As<Object>();
    if (opts.Has("maxLen")) {
      int maxLen = opts.Get("maxLen").As<Number>();
      GetText().SetMaxLen(maxLen);
    }
    if (opts.Has("multiLine")) {
      bool multiLine = opts.Get("multiLine").As<Boolean>();
      GetText().SetMultiLine(multiLine);
    }
    if (opts.Has("passwordField")) {
      bool passwordField = opts.Get("passwordField").As<Boolean>();
      GetText().SetPasswordField(passwordField);
    }
    if (opts.Has("fileField")) {
      bool fileField = opts.Get("fileField").As<Boolean>();
      GetText().SetFileField(fileField);
    }
    if (opts.Has("spellCheckEnabled")) {
      bool spellCheckEnabled = opts.Get("spellCheckEnabled").As<Boolean>();
      GetText().SetSpellcheckingEnabled(spellCheckEnabled);
    }
    if (opts.Has("scrollEnabled")) {
      bool scrollEnabled = opts.Get("scrollEnabled").As<Boolean>();
      GetText().SetScrollBarsEnabled(scrollEnabled);
    }
    if (opts.Has("combs")) {
      bool combs = opts.Get("combs").As<Boolean>();
      GetText().SetCombs(combs);
    }
    if (opts.Has("richText") && opts.Has("maxLen")) {
      bool richText = opts.Get("richText").As<Boolean>();
      GetText().SetRichText(richText);
    }
  }
}
TextField::~TextField()
{
 DbgLog->debug("TextField Cleanup");
}
void
TextField::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "TextField",
    { InstanceAccessor("text", &TextField::Text, &TextField::SetText),
      InstanceAccessor("maxLen", &TextField::GetMaxLen, &TextField::SetMaxLen),
      InstanceAccessor(
        "AP", &TextField::GetAppearanceStream, &TextField::SetAppearanceStream),
      InstanceAccessor("DA",
                       &TextField::GetDefaultAppearance,
                       &TextField::SetDefaultAppearance),
      InstanceAccessor("alignment",
                       &TextField::GetJustification,
                       &TextField::SetJustification),
      InstanceAccessor("obj", &Field::GetFieldObject, nullptr),
      InstanceAccessor(
        "multiLine", &TextField::IsMultiLine, &TextField::SetMultiLine),
      InstanceAccessor("passwordField",
                       &TextField::IsPasswordField,
                       &TextField::SetPasswordField),
      InstanceAccessor(
        "fileField", &TextField::IsFileField, &TextField::SetFileField),
      InstanceAccessor("spellCheckEnabled",
                       &TextField::IsSpellcheckEnabled,
                       &TextField::SetSpellcheckEnabled),
      InstanceAccessor("scrollEnabled",
                       &TextField::IsScrollEnabled,
                       &TextField::SetScrollEnabled),
      InstanceAccessor("combs", &TextField::IsCombs, &TextField::SetCombs),
      InstanceAccessor(
        "richText", &TextField::IsRichText, &TextField::SetRichText),
      InstanceAccessor(
        "readOnly", &TextField::IsReadOnly, &TextField::SetReadOnly),
      InstanceAccessor(
        "required", &TextField::IsRequired, &TextField::SetRequired),
      InstanceAccessor("exported", &TextField::IsExport, &TextField::SetExport),
      InstanceAccessor("type", &TextField::GetType, nullptr),
      InstanceAccessor(
        "fieldName", &TextField::GetFieldName, &TextField::SetFieldName),
      InstanceAccessor("alternateName",
                       &TextField::GetAlternateName,
                       &TextField::SetAlternateName),
      InstanceAccessor(
        "mappingName", &TextField::GetMappingName, &TextField::SetMappingName),
      InstanceAccessor("widgetAnnotation", &TextField::GetAnnotation, nullptr),
      InstanceMethod("setBackgroundColor", &TextField::SetBackground),
      InstanceMethod("setBorderColor", &TextField::SetBorder),
      InstanceMethod("setMouseAction", &TextField::SetMouseAction),
      InstanceMethod("setPageAction", &TextField::SetPageAction),
      InstanceMethod("setHighlightingMode", &TextField::SetHighlightingMode),
      InstanceMethod("refreshAppearanceStream", &TextField::RefreshAppearanceStream)});
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();

  target.Set("TextField", ctor);
}
void
TextField::SetText(const CallbackInfo& info, const JsValue& value)
{
  if (!value.IsString()) {
    throw Napi::Error::New(info.Env(),
                           "TextField value must be of type string");
  }
  string input = value.As<String>().Utf8Value();
  PoDoFo::PdfString v(input);
  GetText().SetText(v);
}

JsValue
TextField::Text(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), GetText().GetText().GetStringUtf8());
}
void
TextField::SetMaxLen(const Napi::CallbackInfo& info, const JsValue& value)
{
  GetText().SetMaxLen(value.As<Number>().Int64Value());
}
JsValue
TextField::GetMaxLen(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetText().GetMaxLen());
}
void
TextField::SetMultiLine(const Napi::CallbackInfo& info,
                        const JsValue& value)
{
  GetText().SetMultiLine(value.As<Boolean>());
}
JsValue
TextField::IsMultiLine(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetText().IsMultiLine());
}
void
TextField::SetPasswordField(const Napi::CallbackInfo&, const JsValue& value)
{
  GetText().SetPasswordField(value.As<Boolean>());
}
JsValue
TextField::IsPasswordField(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetText().IsPasswordField());
}
void
TextField::SetFileField(const Napi::CallbackInfo&, const JsValue& value)
{
  GetText().SetFileField(value.As<Boolean>());
}
JsValue
TextField::IsFileField(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetText().IsFileField());
}
void
TextField::SetSpellcheckEnabled(const Napi::CallbackInfo&,
                                const JsValue& value)
{
  GetText().SetSpellcheckingEnabled(value.As<Boolean>());
}
JsValue
TextField::IsSpellcheckEnabled(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetText().IsSpellcheckingEnabled());
}
void
TextField::SetScrollEnabled(const Napi::CallbackInfo&, const JsValue& value)
{
  GetText().SetScrollBarsEnabled(value.As<Boolean>());
}
JsValue
TextField::IsScrollEnabled(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetText().IsScrollBarsEnabled());
}
void
TextField::SetCombs(const Napi::CallbackInfo&, const JsValue& value)
{
  GetText().SetCombs(value.As<Boolean>());
}
JsValue
TextField::IsCombs(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetText().IsCombs());
}
void
TextField::SetRichText(const Napi::CallbackInfo&, const JsValue& value)
{
  GetText().SetRichText(value.As<Boolean>());
}
JsValue
TextField::IsRichText(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetText().IsRichText());
}
void
TextField::RefreshAppearanceStream(const Napi::CallbackInfo &info)
{
  try {
    Field::RefreshAppearanceStream();
  } catch(PdfError& pdferr) {
    ErrorHandler(pdferr, info);
  } catch(...) {
    Error::New(info.Env(), "An unknown error occurred").ThrowAsJavaScriptException();
  }
}
}
