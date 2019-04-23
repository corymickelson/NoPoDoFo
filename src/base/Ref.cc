//
// Created by developer on 12/9/18.
//

#include "Ref.h"
#include <iosfwd>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace Napi;
using std::cout;
using std::endl;

namespace NoPoDoFo {

FunctionReference Ref::constructor; // NOLINT

Ref::Ref(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() == 2 && info[0].IsNumber() && info[1].IsNumber()) {
    self = new PdfReference(info[0].As<Number>(),
                            static_cast<const PoDoFo::pdf_gennum>(
                              info[1].As<Number>().Uint32Value()));
  } else if (info.Length() == 1 && info[0].IsExternal()) {
    cout << "Creating a new PdfReference Copy" << endl;
    self = new PdfReference(*info[0].As<External<PdfReference>>().Data());
  } else {
    Error::New(info.Env(),
               "References can only be created from already existing objects")
      .ThrowAsJavaScriptException();
  }
  dbglog = spdlog::get("dbglog");
}
Ref::~Ref()
{
  std::stringstream dbgMsg;
  dbgMsg << "Cleaning up Ref " << self->ObjectNumber() << " : "
       << self->GenerationNumber() << endl;
  dbglog->debug(dbgMsg.str());
  delete self;
}
void
Ref::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Ref",
    { InstanceAccessor("gennum", &Ref::GetGenerationNumber, nullptr),
      InstanceAccessor("objnum", &Ref::GetObjectNumber, nullptr) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Ref", ctor);
}
value
Ref::GetObjectNumber(const CallbackInfo& info)
{
  return Number::New(info.Env(), self->ObjectNumber());
}
value
Ref::GetGenerationNumber(const CallbackInfo& info)
{
  return Number::New(info.Env(), self->GenerationNumber());
}
}
