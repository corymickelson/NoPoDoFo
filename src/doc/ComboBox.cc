//
// Created by red on 20/12/17.
//

#include "ComboBox.h"
#include "../ValidateArguments.h"
#include "Field.h"

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

FunctionReference ComboBox::constructor;

ComboBox::ComboBox(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_object });
  Field* nField = Field::Unwrap(info[0].As<Object>());
  PdfComboBox v(nField->GetField());
  self = make_unique<PdfComboBox>(v);
}

void
ComboBox::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(env, "ComboBox", {});
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("ComboBox", ctor);
}
}
