//
// Created by oak on 5/19/18.
//

#ifndef NPDF_ACTION_H
#define NPDF_ACTION_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Action : public Napi::ObjectWrap<Action>
{
public:
  explicit Action(const Napi::CallbackInfo& info);
  explicit Action(const Action&) = delete;
  const Action& operator=(const Action&) = delete;
  ~Action();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetUri(const Napi::CallbackInfo&);
  JsValue GetScript(const Napi::CallbackInfo&);
  JsValue GetType(const Napi::CallbackInfo&);
  void SetUri(const Napi::CallbackInfo&, const JsValue&);
  void SetScript(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetObject(const Napi::CallbackInfo&);
  void AddToDictionary(const Napi::CallbackInfo&);
  PoDoFo::PdfAction& GetAction() const { return *Act; }

private:
  PoDoFo::PdfAction* Act;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_ACTION_H
