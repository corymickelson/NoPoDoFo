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
  const char* name = "Log";
  auto ctor =
    DefineClass(env,
                name,
                {
                  StaticMethod("logOnInterval", &Log::IntervalFlush),
                  InstanceAccessor("logLevel", &Log::GetLevel, &Log::SetLevel),
                  InstanceMethod("logFile", &Log::InitLog),
                });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set(name, ctor);
}
Log::Log(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  Self = spdlog::get("Log");
}

void
Log::InitLog(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info, { { 0, { option(napi_string) } } });
  const auto output = info[0].As<String>().Utf8Value();
  if (!Self) {
    Self = spdlog::basic_logger_mt("Log", output);
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
    TypeError::New(info.Env(), "NoPoDoFo::Log::SetLevel expected a number")
      .ThrowAsJavaScriptException();
  }
  if (value.As<Number>().Int32Value() > 6 ||
      value.As<Number>().Int32Value() < 0) {
    RangeError::New(info.Env(),
                    "NoPoDoFo::Log::SetLevel value must be an "
                    "nopodofo.LogLevel or a number between 0 - 6")
      .ThrowAsJavaScriptException();
  }
  Self->set_level(
    static_cast<spdlog::level::level_enum>(value.As<Number>().Int32Value()));
  Self->flush_on(
    static_cast<spdlog::level::level_enum>(value.As<Number>().Int32Value()));
}

JsValue
Log::GetLevel(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), Self->level());
}
void
Log::IntervalFlush(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info, { { 0, { napi_number } } });
  spdlog::flush_every(std::chrono::seconds(info[0].As<Number>()));
}
Napi::Object
Log::Console(Napi::Env env)
{
  if (env.Global().Has("console")) {
    return env.Global().Get("console").As<Object>();
  } else {
    return {};
  }
}

} // namespace NoPoDoFo
