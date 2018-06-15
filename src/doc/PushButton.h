//
// Created by oak on 5/28/18.
//

#ifndef NPDF_PUSHBUTTON_H
#define NPDF_PUSHBUTTON_H

#include "Button.h"
#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {

class PushButton
  : public Napi::ObjectWrap<PushButton>
  , public Field
  , public Button
{
public:
  static Napi::FunctionReference constructor;
  explicit PushButton(const Napi::CallbackInfo& callbackInfo);
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetRolloverCaption(const Napi::CallbackInfo&);
  Napi::Value GetAlternateCaption(const Napi::CallbackInfo&);
  void SetRolloverCaption(const Napi::CallbackInfo&, const Napi::Value&);
  void SetAlternateCaption(const Napi::CallbackInfo&, const Napi::Value&);
  PoDoFo::PdfPushButton GetPushButton() { return PoDoFo::PdfPushButton(field); }
  PoDoFo::PdfField& field;
};
}
#endif // NPDF_PUSHBUTTON_H
