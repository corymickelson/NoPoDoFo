//
// Created by cory on 4/29/18.
//

#include "Outline.h"
using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference constructor;

Outline::Outline(const CallbackInfo& info) : ObjectWrap(info) {
  outlines = info[0].As<External<PdfOutlines>>().Data();
}
Outline::~Outline() {
  HandleScope scope(Env());
  delete outlines;
}
void Outline::Initialize(Napi::Env &env, Napi::Object &target) {
  HandleScope scope(env)
  auto ctor = DefineClass(
    env,
    "Outline",
    {});
}

}
