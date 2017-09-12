//
// Created by red on 9/11/17.
//

#include "Image.h"
Napi::FunctionReference Image::constructor;

Image::Image(const CallbackInfo &callbackInfo) : ObjectWrap(callbackInfo)
{

}
