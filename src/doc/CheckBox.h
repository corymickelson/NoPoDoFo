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

class CheckBox : public ObjectWrap<CheckBox>
{
public:
  CheckBox(const CallbackInfo& callbackInfo);
  ~CheckBox() { delete box; }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "CheckBox",
                  { InstanceAccessor(
                    "checked", &CheckBox::IsChecked, &CheckBox::SetChecked) });

    target.Set("CheckBox", ctor);
  }
  Napi::Value IsChecked(const CallbackInfo&);
  void SetChecked(const CallbackInfo&, const Napi::Value&);

private:
  PdfCheckBox* box;
};
#endif // NPDF_CHECKBOX_H
