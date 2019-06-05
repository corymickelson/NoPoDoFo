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
#include "../base/Names.h"
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
 if(DbgLog != nullptr) DbgLog->debug("TextField Cleanup");
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
      InstanceMethod("refreshAppearanceStream", &TextField::WriteAppearanceStream)});
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
TextField::WriteAppearanceStream(const Napi::CallbackInfo &info)
{
  stringstream ss;
  PdfLocaleImbue(ss);
  map<string, PdfObject*> apKeys;
  PdfRefCountedBuffer buffer;
  apKeys = GetFieldRefreshKeys(&GetField());
  if (apKeys.find(Name::V) == apKeys.end() &&
      apKeys.find(Name::DV) == apKeys.end()) {
    throw std::exception();
  }
  if (!apKeys.find(Name::V)->second->GetString().IsValid() ||
      apKeys.find(Name::V)->second->GetString().GetCharacterLength() <= 0) {
    throw std::exception();
  }
  PdfXObject xObj(apKeys.find(Name::AP)->second->MustGetIndirectKey(Name::N));
  xObj.GetContentsForAppending()->GetStream()->BeginAppend();
  PdfOutputDevice device(&buffer);
  apKeys.find(Name::V)->second->GetString().Write(&device,
                                                  ePdfWriteMode_Compact);
  ss << "/Tx " << BEGIN_MARKED_CONTENT_OP << endl;
  ss << SAVE_OP << endl;
  ss << BEGIN_TEXT_OP << endl;
  if (apKeys.find(Name::DA) != apKeys.end()) {
    ss << apKeys.find(Name::DA)->second->GetString().GetString() << endl;
    if (!xObj.GetResources()->GetDictionary().HasKey(Name::FONT)) {
      PdfFont* f = GetDAFont(
        string_view(apKeys.find(Name::DA)->second->GetString().GetString()));
      xObj.AddResource(
        f->GetIdentifier(), f->GetObject()->Reference(), Name::FONT);
    }
    if (Self.GetWidgetAnnotation()->GetObject()->GetDictionary().HasKey(
          Name::DA)) {
      Self.GetWidgetAnnotation()->GetObject()->GetDictionary().RemoveKey(
        Name::DA);
    }
    // Add the DA key from apKeys in case the DA was taken from the form
    Self.GetWidgetAnnotation()->GetObject()->GetDictionary().AddKey(
      Name::DA, apKeys.find(Name::DA)->second);
  }
  ss << "2.0 2.0 " << TEXT_POS_OP << endl;
  ss << buffer.GetBuffer() << SHOW_TEXT_OP << endl;
  ss << END_TEXT_OP << endl;
  ss << RESTORE_OP << endl;
  ss << END_MARKED_CONTENT_OP << endl;

  xObj.GetContentsForAppending()->GetStream()->Append(ss.str());
  xObj.GetContentsForAppending()->GetStream()->EndAppend();

  PdfRect r(0,
            0,
            Self.GetWidgetAnnotation()->GetRect().GetWidth(),
            Self.GetWidgetAnnotation()->GetRect().GetHeight());
  xObj.GetObject()->GetDictionary().RemoveKey(Name::BBOX);
  PdfVariant ra;
  r.ToVariant(ra);
  xObj.GetObject()->GetDictionary().AddKey(Name::BBOX, ra.GetArray());
}
}
