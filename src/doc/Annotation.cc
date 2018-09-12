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

#include "Annotation.h"
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Color.h"
#include "../base/XObject.h"
#include "Action.h"
#include "Destination.h"
#include "Document.h"
#include "FileSpec.h"
#include "Page.h"
#include "Rect.h"
#include "StreamDocument.h"
#include <algorithm>

using std::cout;
using std::endl;
using std::find;
using std::make_unique;
using std::stringstream;

namespace NoPoDoFo {

FunctionReference Annotation::constructor; // NOLINT

Annotation::Annotation(const CallbackInfo& info)
  : ObjectWrap(info)
  , annot(*info[0].As<External<PdfAnnotation>>().Data())
{}

void
Annotation::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Annotation",
    {
      InstanceAccessor("flags", &Annotation::GetFlags, &Annotation::SetFlags),
      InstanceAccessor(
        "action", &Annotation::GetAction, &Annotation::SetAction),
      InstanceAccessor("open", &Annotation::GetOpen, &Annotation::SetOpen),
      InstanceAccessor("title", &Annotation::GetTitle, &Annotation::SetTitle),
      InstanceAccessor(
        "content", &Annotation::GetContent, &Annotation::SetContent),
      InstanceAccessor("destination",
                       &Annotation::GetDestination,
                       &Annotation::SetDestination),
      InstanceAccessor(
        "quadPoints", &Annotation::GetQuadPoints, &Annotation::SetQuadPoints),
      InstanceAccessor("color", &Annotation::GetColor, &Annotation::SetColor),
      InstanceAccessor(
        "attachment", &Annotation::GetAttachment, &Annotation::SetAttachment),

      InstanceMethod("hasAppearanceStream", &Annotation::HasAppearanceStream),
      InstanceMethod("setAppearanceStream", &Annotation::SetAppearanceStream),
      InstanceMethod("setBorderStyle", &Annotation::SetBorderStyle),
      InstanceMethod("getType", &Annotation::GetType),
    });

  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Annotation", ctor);
}
void
Annotation::SetFlags(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::TypeError::New(
      info.Env(), "SetFlag must be an instance of NpdfAnnotationType");
  }
  int jsValue = info[0].As<Number>();
  auto flag = static_cast<PoDoFo::EPdfAnnotationFlags>(jsValue);
  GetAnnotation().SetFlags(flag);
}

Napi::Value
Annotation::GetFlags(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), GetAnnotation().GetFlags());
}

Napi::Value
Annotation::HasAppearanceStream(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetAnnotation().HasAppearanceStream());
}

void
Annotation::SetAppearanceStream(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsObject() ||
      !info[0].As<Object>().InstanceOf(XObject::constructor.Value())) {
    TypeError::New(info.Env(),
                   "Requires an instance of XObject as the first argument")
      .ThrowAsJavaScriptException();
    return;
  }
  GetAnnotation().SetAppearanceStream(
    &(XObject::Unwrap(info[0].As<Object>())->GetXObject()));
}

void
Annotation::SetBorderStyle(const CallbackInfo& info)
{
  if (info.Length() < 1 || !info[0].IsObject()) {
    Error::New(info.Env(),
               "Border requires three arguments of type number."
               "Number - horizontal corner radius"
               "Number - vertical corner radius"
               "Number - width")
      .ThrowAsJavaScriptException();
    return;
  }
  double horizontal = info[0].As<Object>().Get("horizontal").As<Number>();
  double vertical = info[0].As<Object>().Get("vertical").As<Number>();
  double width = info[0].As<Object>().Get("width").As<Number>();
  GetAnnotation().SetBorderStyle(horizontal, vertical, width);
}

void
Annotation::SetTitle(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsString()) {
    throw Napi::TypeError::New(
      info.Env(), "SetTitle requires a single argument of type string.");
  }
  try {
    string title = info[0].As<String>().Utf8Value();
    GetAnnotation().SetTitle(PdfString(title));
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo PdfError: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

Napi::Value
Annotation::GetTitle(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), GetAnnotation().GetTitle().GetString());
}

void
Annotation::SetContent(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty()) {
    throw Napi::Error::New(info.Env(),
                           "SetContent requires string \"value\" argument.");
  }
  string content = value.As<String>().Utf8Value();
  GetAnnotation().SetContents(content);
}

Napi::Value
Annotation::GetContent(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
                           GetAnnotation().GetContents().GetStringUtf8());
}

void
Annotation::SetDestination(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.As<Object>().InstanceOf(Destination::constructor.Value())) {
    auto destination = Destination::Unwrap(value.As<Object>());
    GetAnnotation().SetDestination(destination->GetDestination());
  } else {
    TypeError::New(info.Env(), "Requires instance of Destination")
      .ThrowAsJavaScriptException();
  }
}

Napi::Value
Annotation::GetDestination(const CallbackInfo& info)
{
  auto doc = Document::Unwrap(info[0].As<Object>())->base;
  PdfDestination d = GetAnnotation().GetDestination(doc);
  return Destination::constructor.New(
    { External<PdfDestination>::New(info.Env(), &d) });
}

void
Annotation::SetAction(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.As<Object>().InstanceOf(Action::constructor.Value())) {
    TypeError::New(info.Env(), "Requires instance of Action")
      .ThrowAsJavaScriptException();
    return;
  }
  auto action = Action::Unwrap(value.As<Object>());
  GetAnnotation().SetAction(*action->GetAction());
}

Napi::Value
Annotation::GetAction(const CallbackInfo& info)
{
  if (!GetAnnotation().HasAction()) {
    return info.Env().Null();
  }
  PdfAction* currentAction = GetAnnotation().GetAction();
  if (currentAction->HasScript()) {
    return Napi::String::New(info.Env(),
                             currentAction->GetScript().GetString());
  } else if (currentAction->HasURI()) {
    return Napi::String::New(info.Env(), currentAction->GetURI().GetString());
  } else {
    throw Napi::Error::New(info.Env(), "Action not set");
  }
}

void
Annotation::SetOpen(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::Error::New(info.Env(), "Requires Boolean type");
  }
  GetAnnotation().SetOpen(value.As<Boolean>());
}

Napi::Value
Annotation::GetOpen(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), GetAnnotation().GetOpen());
}

void
Annotation::SetColor(const CallbackInfo& info, const Napi::Value& value)
{
  // SetNoPoDoFoColor(value, GetAnnotation().SetColor) vector<NPDFColorFormat>
  vector<NPDFColorFormat> types = { NPDFColorFormat::RGB,
                                    NPDFColorFormat::GreyScale,
                                    NPDFColorFormat::CMYK };
  NPDFColorAccessor(
    Color::Unwrap(value.As<Object>())->color, types, GetAnnotation().SetColor)
}

Napi::Value
Annotation::GetColor(const CallbackInfo& info)
{
  auto color = GetAnnotation().GetColor();
  switch (color.size()) {
    case 1: { // greyscale
      return Color::constructor.New(
        { Number::New(info.Env(), color[0].GetNumber()) });
    }
    case 3: { // rgb
      return Color::constructor.New(
        { Number::New(info.Env(), color[0].GetNumber()),
          Number::New(info.Env(), color[1].GetNumber()),
          Number::New(info.Env(), color[2].GetNumber()) });
    }
    case 4: { // cmyk
      return Color::constructor.New(
        { Number::New(info.Env(), color[0].GetNumber()),
          Number::New(info.Env(), color[1].GetNumber()),
          Number::New(info.Env(), color[2].GetNumber()),
          Number::New(info.Env(), color[3].GetNumber()) });
    }
    default:
      Error::New(info.Env(), "Unable to get color")
        .ThrowAsJavaScriptException();
      break;
  }
}

Napi::Value
Annotation::GetType(const CallbackInfo& info)
{
  string jsType;
  switch (GetAnnotation().GetType()) {
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_3D: {
      jsType = "3D";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Caret: {
      jsType = "Caret";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Circle: {
      jsType = "Circle";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_FileAttachement: {
      jsType = "FileAttachment";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_FreeText: {
      jsType = "FreeText";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Highlight: {
      jsType = "HighLight";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Ink: {
      jsType = "Ink";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Line: {
      jsType = "Line";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Link: {
      jsType = "Link";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Movie: {
      jsType = "Movie";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_PolyLine: {
      jsType = "PolyLine";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Polygon: {
      jsType = "Polygon";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Popup: {
      jsType = "Popup";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_PrinterMark: {
      jsType = "PrintMark";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_RichMedia: {
      jsType = "RichMedia";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Screen: {
      jsType = "Screen";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Sound: {
      jsType = "Sound";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Square: {
      jsType = "Square";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Squiggly: {
      jsType = "Squiggly";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Stamp: {
      jsType = "Stamp";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_StrikeOut: {
      jsType = "StikeOut";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Text: {
      jsType = "Text";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_TrapNet: {
      jsType = "TrapNet";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Unknown: {
      jsType = "Unknown";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Underline: {
      jsType = "Underline";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Watermark: {
      jsType = "Watermark";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_WebMedia: {
      jsType = "WebMedia";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Widget: {
      jsType = "Widget";
      break;
    }
  }
  return Napi::String::New(info.Env(), jsType);
}

void
Annotation::SetQuadPoints(const CallbackInfo& info, const Napi::Value& value)
{
  PdfArray points;
  auto nArray = value.As<Array>();
  for (uint32_t i = 0; i < nArray.Length(); i++) {
    auto item = nArray.Get(i);
    if (!item.IsNumber()) {
      throw Error::New(info.Env(), "QuadPoints must be integer values");
    }
    points.push_back(PdfObject(item.As<Number>().DoubleValue()));
  }
  try {
    GetAnnotation().SetQuadPoints(points);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Annotation::GetQuadPoints(const CallbackInfo& info)
{
  auto jsArray = Array::New(info.Env());
  uint32_t i = 0;
  for (auto& item : GetAnnotation().GetQuadPoints()) {
    auto value = Number::New(info.Env(), item.GetNumber());
    jsArray.Set(i, Number::New(info.Env(), value));
    ++i;
  }
  return jsArray;
}
Napi::Value
Annotation::GetAttachment(const CallbackInfo& info)
{
  if (!GetAnnotation().HasFileAttachement()) {
    return info.Env().Null();
  }
  auto file = GetAnnotation().GetFileAttachement()->GetObject();
  return FileSpec::constructor.New({ External<PdfObject>::New(
    info.Env(), new PdfObject(*file), [](Napi::Env env, PdfObject* data) {
      HandleScope scope(env);
      delete data;
    }) });
}
void
Annotation::SetAttachment(const CallbackInfo& info, const Napi::Value& value)
{}
}
