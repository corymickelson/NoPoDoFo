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
  PdfReference* self;
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  explicit Ref(const CallbackInfo& info);
  ~Ref();
  JsValue GetObjectNumber(const CallbackInfo&);
  JsValue GetGenerationNumber(const CallbackInfo&);
private:
  std::shared_ptr<spdlog::logger> dbglog;
};

}

#endif // NOPODOFO_REF_H
