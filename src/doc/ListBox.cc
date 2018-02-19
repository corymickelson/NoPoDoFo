//
// Created by oak on 2/19/18.
//

#include "ListBox.h"
#include "../ValidateArguments.h"
#include "Field.h"
namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

FunctionReference ListBox::constructor;

ListBox::ListBox(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_object });
  Field* nField = Field::Unwrap(info[0].As<Object>());
//  PdfListBox v(nField->GetField());
  self = make_unique<PdfListBox>(*new PdfListBox(nField->GetField()));
}
void
ListBox::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(env, "ListBox", {});
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("ListBox", ctor);
}
}
