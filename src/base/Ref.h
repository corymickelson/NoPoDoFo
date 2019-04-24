//
// Created by developer on 12/9/18.
//

#ifndef NOPODOFO_REF_H
#define NOPODOFO_REF_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using namespace Napi;

using JsValue = Napi::Value;
using PoDoFo::PdfReference;

namespace NoPoDoFo {
class Ref : public Napi::ObjectWrap<Ref>
{
public:
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  explicit Ref(const CallbackInfo& info);
  explicit Ref(const Ref&) = delete;
  const Ref& operator=(const Ref&) = delete;
  ~Ref();
  JsValue GetObjectNumber(const CallbackInfo&);
  JsValue GetGenerationNumber(const CallbackInfo&);

  PdfReference* Self;
private:
  std::shared_ptr<spdlog::logger> DbgLog;
};

}

#endif // NOPODOFO_REF_H
