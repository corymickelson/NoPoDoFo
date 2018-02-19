//
// Created by red on 9/11/17.
//

#ifndef NPDF_CHECKBOX_H
#define NPDF_CHECKBOX_H

#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
class CheckBox : public ObjectWrap<CheckBox>
{
public:
  explicit CheckBox(const CallbackInfo& callbackInfo);
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value IsChecked(const CallbackInfo&);
  void SetChecked(const CallbackInfo&, const Napi::Value&);

private:
  std::unique_ptr<PdfCheckBox> box;
};
}
#endif // NPDF_CHECKBOX_H
