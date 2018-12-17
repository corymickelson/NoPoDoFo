//
// Created by oak on 5/28/18.
//

#include "PushButton.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference PushButton::constructor; // NOLINT

PushButton::PushButton(const Napi::CallbackInfo& info)
  : ObjectWrap<PushButton>(info)
  , Field(ePdfField_PushButton, info)
  , Button(Field::GetField())
  , field(Field::GetField())
{}
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
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
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
