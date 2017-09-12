//
// Created by red on 9/11/17.
//

#include "Rect.h"
Napi::FunctionReference Rect::constructor;

Rect::Rect(const CallbackInfo &callbackInfo) : ObjectWrap(callbackInfo)
{

}
