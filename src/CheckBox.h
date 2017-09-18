//
// Created by red on 9/11/17.
//

#ifndef NPDF_CHECKBOX_H
#define NPDF_CHECKBOX_H

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

class CheckBox : public ObjectWrap<CheckBox>
{
public:
  CheckBox(const CallbackInfo& callbackInfo);
  ~CheckBox();
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env, "Annotation", {});

    target.Set("Annotation", ctor);
  }

private:
};
#endif // NPDF_CHECKBOX_H
