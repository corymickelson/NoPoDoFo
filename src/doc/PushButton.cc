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

#include "PushButton.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference PushButton::Constructor; // NOLINT

PushButton::PushButton(const Napi::CallbackInfo& info)
  : ObjectWrap<PushButton>(info)
  , Field(ePdfField_PushButton, info)
  , Button(Field::GetField())
  , Self(Field::GetField())
{
  Log = spdlog::get("Log");
}

void
PushButton::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
    env,
    "PushButton",
    { InstanceAccessor("rollover",
                       &PushButton::GetRolloverCaption,
                       &PushButton::SetRolloverCaption),
      InstanceAccessor("rolloverAlternate",
                       &PushButton::GetAlternateCaption,
                       &PushButton::SetAlternateCaption),

      InstanceAccessor("AP",
                       &PushButton::GetAppearanceStream,
                       &PushButton::SetAppearanceStream),
      InstanceAccessor("DA",
                       &PushButton::GetDefaultAppearance,
                       &PushButton::SetDefaultAppearance),
      InstanceAccessor("widgetAnnotation", &PushButton::GetAnnotation, nullptr),

      InstanceAccessor("obj", &Field::GetFieldObject, nullptr),
      InstanceAccessor(
        "readOnly", &PushButton::IsReadOnly, &PushButton::SetReadOnly),
      InstanceAccessor(
        "required", &PushButton::IsRequired, &PushButton::SetRequired),
      InstanceAccessor(
        "exported", &PushButton::IsExport, &PushButton::SetExport),
      InstanceAccessor("type", &PushButton::GetType, nullptr),
      InstanceAccessor(
        "fieldName", &PushButton::GetFieldName, &PushButton::SetFieldName),
      InstanceAccessor("alternateName",
                       &PushButton::GetAlternateName,
                       &PushButton::SetAlternateName),
      InstanceAccessor("mappingName",
                       &PushButton::GetMappingName,
                       &PushButton::SetMappingName),
      InstanceAccessor(
        "caption", &PushButton::GetCaption, &PushButton::SetCaption),
      InstanceMethod("setBackgroundColor", &PushButton::SetBackground),
      InstanceMethod("setBorderColor", &PushButton::SetBorder),
      InstanceMethod("setMouseAction", &PushButton::SetMouseAction),
      InstanceMethod("setPageAction", &PushButton::SetPageAction),
      InstanceMethod("setHighlightingMode", &PushButton::SetHighlightingMode)

    });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("PushButton", ctor);
}
Napi::Value
PushButton::GetRolloverCaption(const Napi::CallbackInfo& info)
{
  return String::New(info.Env(),
                     GetPushButton().GetRolloverCaption().GetStringUtf8());
}
Napi::Value
PushButton::GetAlternateCaption(const Napi::CallbackInfo& info)
{
  return String::New(info.Env(),
                     GetPushButton().GetAlternateCaption().GetStringUtf8());
}
void
PushButton::SetRolloverCaption(const Napi::CallbackInfo&,
                               const Napi::Value& value)
{
  GetPushButton().SetCaption(PdfString(value.As<String>().Utf8Value()));
}
void
PushButton::SetAlternateCaption(const Napi::CallbackInfo&,
                                const Napi::Value& value)
{
  GetPushButton().SetAlternateCaption(
    PdfString(value.As<String>().Utf8Value()));
}
}
