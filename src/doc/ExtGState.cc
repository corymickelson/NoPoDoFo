#include "ExtGState.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
FunctionReference ExtGState::constructor;

ExtGState::ExtGState(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_external, napi_valuetype::napi_external });
  self = info[0].As<External<PdfExtGState>>().Data();
  doc = info[0].As<External<PdfMemDocument>>().Data();
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
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  float value = info[0].As<Number>().FloatValue();
  self->SetFillOpacity(value);
}

void
ExtGState::SetStrokeOpacity(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  float value = info[0].As<Number>().FloatValue();
  self->SetStrokeOpacity(value);
}

// todo:Fix this
void
ExtGState::SetBlendMode(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
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
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  self->SetOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetFillOverprint(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  self->SetFillOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetStrokeOverprint(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  self->SetStrokeOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetNonZeroOverprint(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  self->SetNonZeroOverprint(info[0].As<Boolean>());
}

void
ExtGState::SetRenderingIntent(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
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
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  double v = info[0].As<Number>();
  self->SetFrequency(v);
};
}
