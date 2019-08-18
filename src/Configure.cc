#include "Configure.h"
#include "ValidateArguments.h"
#include <spdlog/sinks/basic_file_sink.h>

using namespace Napi;

namespace NoPoDoFo {

FunctionReference Configure::Constructor; // NOLINT

void
Configure::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  const char* klass = "Configure";
  auto ctor = DefineClass(
    env,
    klass,
    {
      InstanceAccessor("logLevel", &Configure::GetLevel, &Configure::SetLevel),
      InstanceMethod("logFile", &Configure::InitLog),
      InstanceMethod("logOnInterval", &Configure::IntervalFlush),
    });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set(klass, ctor);
}
Configure::Configure(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  Log = spdlog::get("Log");
}

void
Configure::InitLog(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info, { { 0, { option(napi_string) } } });
  const auto output = info[0].As<String>().Utf8Value();
  if (!Log) {
    Log = spdlog::basic_logger_mt("Log", output);
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
Configure::SetLevel(const Napi::CallbackInfo& info, const Napi::Value& value)
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
  Log->set_level(
    static_cast<spdlog::level::level_enum>(value.As<Number>().Int32Value()));
  Log->flush_on(
    static_cast<spdlog::level::level_enum>(value.As<Number>().Int32Value()));
}

JsValue
Configure::GetLevel(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), Log->level());
}
void
Configure::IntervalFlush(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info, {{0, {napi_number}}});
  spdlog::flush_every(std::chrono::seconds(info[0].As<Number>()));
}

} // namespace NoPoDoFo
