//
// Created by red on 9/11/17.
//

#ifndef NPDF_TEXTFIELD_H
#define NPDF_TEXTFIELD_H

#include "../ValidateArguments.h"
#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>

using namespace std;

class TextField : public Napi::ObjectWrap<TextField>
{
public:
  explicit TextField(const Napi::CallbackInfo&);
  ~TextField() { delete field; }
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void SetText(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetText(const Napi::CallbackInfo&);

private:
  Field* field;
  PoDoFo::PdfTextField GetField()
  {
    return PoDoFo::PdfTextField(field->GetField());
  }
};
#endif // NPDF_TEXTFIELD_H
