#include "Log.h"
#include "ValidateArguments.h"
#include <spdlog/sinks/basic_file_sink.h>

using namespace Napi;

namespace NoPoDoFo {

FunctionReference Log::Constructor; // NOLINT

void
Log::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  const char* klass = "Log";
  auto ctor = DefineClass(
    env,
    klass,
    {
      InstanceAccessor("logLevel", &Log::GetLevel, &Log::SetLevel),
      InstanceMethod("logFile", &Log::InitLog),
      StaticMethod("logOnInterval", &Log::IntervalFlush),
    });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set(klass, ctor);
}
Log::Log(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  Instance = spdlog::get("Log");
}

void
Log::InitLog(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info, { { 0, { option(napi_string) } } });
  const auto output = info[0].As<String>().Utf8Value();
  if (!Instance) {
    Instance = spdlog::basic_logger_mt("Log", output);
  } else {
    if (info.Env().Global().Has("console")) {
      info.Env()
        .Global()
        .Get("console")
        .As<Object>()
        .Get("info")
        .As<Function>()
        .Call(
          { String::New(info.Env(), "Logging has already been initialized") });
    }
  }
}

void
Log::SetLevel(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    TypeError::New(info.Env(),
                   "NoPoDoFo::Configure::SetLevel expected a number")
      .ThrowAsJavaScriptException();
  }
  if (value.As<Number>().Int32Value() > 6 ||
      value.As<Number>().Int32Value() < 0) {
    RangeError::New(info.Env(),
                    "NoPoDoFo::Configure::SetLevel value must be an "
                    "nopodofo.LogLevel or a number between 0 - 6")
      .ThrowAsJavaScriptException();
  }
  Instance->set_level(
    static_cast<spdlog::level::level_enum>(value.As<Number>().Int32Value()));
  Instance->flush_on(
    static_cast<spdlog::level::level_enum>(value.As<Number>().Int32Value()));
}

JsValue
Log::GetLevel(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), Instance->level());
}
void
Log::IntervalFlush(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info, {{0, {napi_number}}});
  spdlog::flush_every(std::chrono::seconds(info[0].As<Number>()));
}

} // namespace NoPoDoFo
