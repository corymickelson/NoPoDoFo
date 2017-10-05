//
// Created by red on 10/1/17.
//

#include "Stream.h"

using namespace Napi;
using namespace PoDoFo;

FunctionReference Stream::constructor;

Stream::Stream(const CallbackInfo& info)
  : ObjectWrap(info)
{}

void
Stream::Initialize(Napi::Env& env, Napi::Object& target)
{
  Function ctor =
    DefineClass(env, "Stream", { InstanceMethod("write", &Stream::Write) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Stream", constructor);
}

void
Stream::Write(const CallbackInfo& info)
{}
