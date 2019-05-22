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

#include "ExtGState.h"
#include "../ErrorHandler.h"
#include "Document.h"
#include "StreamDocument.h"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

using std::find;
using std::make_unique;
using std::string;
using std::vector;

FunctionReference ExtGState::Constructor; // NOLINT

ExtGState::ExtGState(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  DbgLog = spdlog::get("DbgLog");
  auto o = info[0].As<Object>();
  if (o.InstanceOf(Document::Constructor.Value())) {
    auto d = Document::Unwrap(o);
    Self = make_unique<PdfExtGState>(&d->GetDocument());
  } else if (o.InstanceOf(StreamDocument::Constructor.Value())) {
    auto d = StreamDocument::Unwrap(o);
    Self = make_unique<PdfExtGState>(d->Base);
  } else {
    Error::New(
      info.Env(),
      "Requires an instance of StreamDocument or Document for construction")
      .ThrowAsJavaScriptException();
  }
}

ExtGState::~ExtGState()
{
  DbgLog->debug("ExtGState Cleanup");
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
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("ExtGState", ctor);
}

void
ExtGState::SetFillOpacity(const CallbackInfo& info)
{
  const float value = info[0].As<Number>().FloatValue();
  Self->SetFillOpacity(value);
}

void
ExtGState::SetStrokeOpacity(const CallbackInfo& info)
{
  const float value = info[0].As<Number>().FloatValue();
  Self->SetStrokeOpacity(value);
}

void
ExtGState::SetBlendMode(const CallbackInfo& info)
{
  const string value = info[0].As<String>().Utf8Value();
  vector<string> candidateValues = { "Normal",     "Multiply",   "Screen",
                                     "Overlay",    "Darken",     "Lighten",
                                     "ColorDodge", "ColorBurn",  "HardLight",
                                     "SoftLight",  "Difference", "Exclusion",
                                     "Hue",        "Saturation", "Color",
                                     "Luminosity" };
  if (find(candidateValues.begin(), candidateValues.end(), value) !=
      candidateValues.end()) {
    Self->SetBlendMode(value.c_str());
  } else {
    throw Error::New(info.Env(),
                     "Blend mode must be one of type NPdfBlendMode");
  }
}

void
ExtGState::SetOverprint(const CallbackInfo& info)
{
  Self->SetOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetFillOverprint(const CallbackInfo& info)
{
  Self->SetFillOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetStrokeOverprint(const CallbackInfo& info)
{
  Self->SetStrokeOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetNonZeroOverprint(const CallbackInfo& info)
{
  Self->SetNonZeroOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetRenderingIntent(const CallbackInfo& info)
{
  const string v = info[0].As<String>().Utf8Value();
  vector<string> candidateValues = {
    "AbsoluteColorimetric", "RelativeColorimetric", "Perceptual", "Saturation"
  };
  if (find(candidateValues.begin(), candidateValues.end(), v) !=
      candidateValues.end()) {
    Self->SetRenderingIntent(v.c_str());
  } else {
    throw Error::New(info.Env(),
                     "rendering must be of type NPdfRenderingIntent");
  }
}

void
ExtGState::SetFrequency(const CallbackInfo& info)
{
  const double v = info[0].As<Number>();
  Self->SetFrequency(v);
}
}
