//
// Created by red on 9/11/17.
//

#ifndef NPDF_TEXTFIELD_H
#define NPDF_TEXTFIELD_H

#include "Field.h"
#include "ValidateArguments.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>

using namespace Napi;
using namespace PoDoFo;
using namespace std;

class TextField : public Napi::ObjectWrap<TextField>
{
public:
  explicit TextField(const CallbackInfo&);
  ~TextField() { delete field; }
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "TextField",
                  { InstanceAccessor("text", &TextField::GetText, &TextField::SetText) });
    target.Set("TextField", ctor);
  }
  void SetText(const CallbackInfo&, const Napi::Value &);
  Napi::Value GetText(const CallbackInfo&);

private:
  Field* field;
  PdfTextField GetField() { return PdfTextField(field->GetField()); }
};
#endif // NPDF_TEXTFIELD_H
