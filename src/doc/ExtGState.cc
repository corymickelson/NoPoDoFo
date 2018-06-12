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

#include "ExtGState.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Document.h"
#include "StreamDocument.h"
#include <algorithm>

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

using std::find;
using std::make_unique;
using std::string;
using std::vector;

FunctionReference ExtGState::constructor; // NOLINT

ExtGState::ExtGState(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  auto o = info[0].As<Object>();
  if (o.InstanceOf(Document::constructor.Value())) {
    auto d = Document::Unwrap(o);
    self = make_unique<PdfExtGState>(&d->GetDocument());
  } else if (o.InstanceOf(StreamDocument::constructor.Value())) {
    auto d = StreamDocument::Unwrap(o);
    self = make_unique<PdfExtGState>(d->base);
  } else {
    Error::New(
      info.Env(),
      "Requires an instance of StreamDocument or Document for construction")
      .ThrowAsJavaScriptException();
  }
}

void
ExtGState::Initialize(Napi::Env& env, Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "ExtGState",
    { InstanceMethod("setFillOpacity", &ExtGState::SetFillOpacity),
      InstanceMethod("setBlendMode", &ExtGState::SetBlendMode),
      InstanceMethod("setOverprint", &ExtGState::SetOverprint),
      InstanceMethod("SetFillOverprint", &ExtGState::SetFillOverprint),
      InstanceMethod("setStrokeOverprint", &ExtGState::SetStrokeOverprint),
      InstanceMethod("setStrokeOpacity", &ExtGState::SetStrokeOpacity),
      InstanceMethod("setNonZeroOverprint", &ExtGState::SetNonZeroOverprint),
      InstanceMethod("setRenderingIntent", &ExtGState::SetRenderingIntent),
      InstanceMethod("setFrequency", &ExtGState::SetFrequency) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("ExtGState", ctor);
}

void
ExtGState::SetFillOpacity(const CallbackInfo& info)
{
  float value = info[0].As<Number>().FloatValue();
  self->SetFillOpacity(value);
}

void
ExtGState::SetStrokeOpacity(const CallbackInfo& info)
{
  float value = info[0].As<Number>().FloatValue();
  self->SetStrokeOpacity(value);
}

// todo:Fix this
void
ExtGState::SetBlendMode(const CallbackInfo& info)
{
  string value = info[0].As<String>().Utf8Value();
  vector<string> candidateValues = { "Normal",     "Multiply",   "Screen",
                                     "Overlay",    "Darken",     "Lighten",
                                     "ColorDodge", "ColorBurn",  "HardLight",
                                     "SoftLight",  "Difference", "Exclusion",
                                     "Hue",        "Saturation", "Color",
                                     "Luminosity" };
  if (find(candidateValues.begin(), candidateValues.end(), value) !=
      candidateValues.end()) {
    self->SetBlendMode(value.c_str());
  } else {
    throw Error::New(info.Env(),
                     "Blend mode must be one of type NPdfBlendMode");
  }
}

void
ExtGState::SetOverprint(const CallbackInfo& info)
{
  self->SetOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetFillOverprint(const CallbackInfo& info)
{
  self->SetFillOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetStrokeOverprint(const CallbackInfo& info)
{
  self->SetStrokeOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetNonZeroOverprint(const CallbackInfo& info)
{
  self->SetNonZeroOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetRenderingIntent(const CallbackInfo& info)
{
  string v = info[0].As<String>().Utf8Value();
  vector<string> candidateValues = {
    "AbsoluteColorimetric", "RelativeColorimetric", "Perceptual", "Saturation"
  };
  if (find(candidateValues.begin(), candidateValues.end(), v) !=
      candidateValues.end()) {
    self->SetRenderingIntent(v.c_str());
  } else {
    throw Error::New(info.Env(),
                     "rendering must be of type NPdfRenderingIntent");
  }
}

void
ExtGState::SetFrequency(const CallbackInfo& info)
{
  double v = info[0].As<Number>();
  self->SetFrequency(v);
}
}
