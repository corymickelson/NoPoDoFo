//
// Created by oak on 5/28/18.
//

#ifndef NPDF_PUSHBUTTON_H
#define NPDF_PUSHBUTTON_H

#include "Button.h"
#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>
using JsValue = Value;

namespace NoPoDoFo {

class PushButton final
  : public ObjectWrap<PushButton>
    , public Field
    , public Button
{
public:
  static FunctionReference Constructor;
  explicit PushButton(const CallbackInfo& callbackInfo);
  explicit PushButton(const PushButton&) = delete;
  const PushButton& operator=(const PushButton&) = delete;
  static void Initialize(Napi::Env& env, Object& target);
  JsValue GetRolloverCaption(const CallbackInfo&);
  JsValue GetAlternateCaption(const CallbackInfo&);
  void SetRolloverCaption(const CallbackInfo&, const JsValue&);
  void SetAlternateCaption(const CallbackInfo&, const JsValue&);
  PdfPushButton GetPushButton() { return PdfPushButton(Self); }
  PdfField& Self;
private:
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_PUSHBUTTON_H
