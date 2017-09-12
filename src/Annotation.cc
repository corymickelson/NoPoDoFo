//
// Created by red on 9/11/17.
//

#include "Annotation.h"
Napi::FunctionReference Annotation::constructor;

Annotation::Annotation(const CallbackInfo &callbackInfo) : ObjectWrap(callbackInfo)
{

}
