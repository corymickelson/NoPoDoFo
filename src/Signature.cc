//
// Created by red on 9/11/17.
//

#include "Signature.h"
Napi::FunctionReference Signature::constructor;

Signature::Signature(const CallbackInfo &info) : ObjectWrap(info) {}
