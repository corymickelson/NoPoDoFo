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
  auto ctor = DefineClass(env,
                          klass,
                          { InstanceAccessor("enableDebugLogging",
                                             &Configure::GetDebugLogging,
                                             &Configure::EnableDebugLogging),
                            InstanceMethod("logFile", &Configure::LogOutput) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set(klass, ctor);
}
Configure::Configure(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  DbgLog = spdlog::get("DbgLog");
}
void
Configure::EnableDebugLogging(const CallbackInfo& info, const JsValue& value)
{
  if (DbgLog == nullptr) {
    Error::New(info.Env(),
               "Before debug logging can be enabled the output must be set. "
               "See Configure::logFile in the docs")
      .ThrowAsJavaScriptException();
    return;
  }
  if (value.IsBoolean() && value.As<Boolean>() == true) {
    DbgLog->set_level(spdlog::level::debug);
    DbgLog->flush_on(spdlog::level::debug);
  } else {
    DbgLog->set_level(spdlog::level::off);
  }
}
JsValue
Configure::GetDebugLogging(const Napi::CallbackInfo& info)
{
  bool enabled = false;
  if (DbgLog->level() == spdlog::level::debug) {
    enabled = true;
  }
  return Boolean::New(info.Env(), enabled);
}
void
Configure::LogOutput(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info, { { 0, { option(napi_string) } } });
  const auto output = info[0].As<String>().Utf8Value();
  if (!DbgLog) {
    DbgLog = spdlog::basic_logger_mt("DbgLog", output);
  } else {
    if (info.Env().Global().Has("console")) {
      info.Env()
        .Global()
        .Get("console")
        .As<Object>()
        .Get("info")
        .As<Function>()
        .Call({ String::New(info.Env(),
                            "DbgLogging has already been configured") });
    }
  }
}

} // namespace NoPoDoFo
