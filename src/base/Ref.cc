/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2019
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Ref.h"
#include <iosfwd>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace Napi;
using std::cout;
using std::endl;

namespace NoPoDoFo {

FunctionReference Ref::Constructor; // NOLINT

Ref::Ref(const CallbackInfo& info)
  : ObjectWrap(info)
{
  DbgLog = spdlog::get("DbgLog");
  if (info.Length() == 2 && info[0].IsNumber() && info[1].IsNumber()) {
    Self = new PdfReference(info[0].As<Number>(),
                            static_cast<const PoDoFo::pdf_gennum>(
                              info[1].As<Number>().Uint32Value()));
  } else if (info.Length() == 1 && info[0].IsExternal()) {
    if(DbgLog != nullptr) DbgLog->debug("Creating a new PdfReference Copy");
    Self = new PdfReference(*info[0].As<External<PdfReference>>().Data());
  } else {
    Error::New(info.Env(),
               "References can only be created from already existing objects")
      .ThrowAsJavaScriptException();
  }
}
Ref::~Ref()
{
  std::stringstream dbgMsg;
  dbgMsg << "Cleaning up Ref " << Self->ObjectNumber() << " : "
       << Self->GenerationNumber() << endl;
  if(DbgLog != nullptr) DbgLog->debug(dbgMsg.str());
  delete Self;
}
void
Ref::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
    env,
    "Ref",
    { InstanceAccessor("gennum", &Ref::GetGenerationNumber, nullptr),
      InstanceAccessor("objnum", &Ref::GetObjectNumber, nullptr) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();

  target.Set("Ref", ctor);
}
JsValue
Ref::GetObjectNumber(const CallbackInfo& info)
{
  return Number::New(info.Env(), Self->ObjectNumber());
}
JsValue
Ref::GetGenerationNumber(const CallbackInfo& info)
{
  return Number::New(info.Env(), Self->GenerationNumber());
}
}
