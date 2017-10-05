//
// Created by red on 9/26/17
//

#include "SignatureField.h"

using namespace Napi;
using namespace PoDoFo;

SignatureField::SignatureField(const CallbackInfo& info)
  : ObjectWrap<SignatureField>(info)
{
  // page, rect, document
}

void
SignatureField::Initialize(Napi::Env& env, Napi::Object& target)
{}
