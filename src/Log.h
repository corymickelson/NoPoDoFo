//
// Created by cmickelson on 5/3/19.
//

#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "spdlog/spdlog.h"
#include <napi.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Log : public Napi::ObjectWrap<Log>
{
public:
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  explicit Log(const Napi::CallbackInfo&);
  explicit Log(const NoPoDoFo::Log& info) = delete;
  const Log& operator=(const Log&) = delete;

  void InitLog(const Napi::CallbackInfo& info);
  void SetLevel(const Napi::CallbackInfo&, const Napi::Value&);
  JsValue GetLevel(const Napi::CallbackInfo&);
  static void IntervalFlush(const Napi::CallbackInfo&);
  static Napi::Object Console(Napi::Env env);

private:
  std::shared_ptr<spdlog::logger> Self;
};

}

#endif // CONFIGURE_H
