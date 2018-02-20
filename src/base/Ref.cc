/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
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
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Obj.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
Napi::FunctionReference Ref::constructor;

Ref::Ref(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() == 1 && info[0].IsNumber()) {
    const pdf_objnum i = info[0].As<Number>();
    ref = new PdfReference(i, 0);
  } else if (info.Length() == 1 && info[0].Type() == napi_external) {
    ref = info[0].As<Napi::External<PdfReference>>().Data();
  }
}

void
Ref::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Napi::Function ctor = DefineClass(
    env,
    "Ref",
    { InstanceAccessor(
        "generation", &Ref::GetGenerationNumber, &Ref::SetGenerationNumber),
      InstanceAccessor(
        "objectNumber", &Ref::GetObjectNumber, &Ref::SetObjectNumber),
      InstanceMethod("toString", &Ref::ToString),
      InstanceMethod("write", &Ref::Write),
      InstanceMethod("getObject", &Ref::GetObj),
      InstanceMethod("isIndirect", &Ref::IsIndirect) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Ref", ctor);
}
void
Ref::SetGenerationNumber(const Napi::CallbackInfo& info,
                         const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "Generation number must be of type number");
  }
  int gen = value.As<Napi::Number>();
  ref->SetGenerationNumber(static_cast<const pdf_gennum>(gen));
}
Napi::Value
Ref::GetGenerationNumber(const Napi::CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), ref->GenerationNumber());
}
void
Ref::SetObjectNumber(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Object number must be of type number");
  }
  int objNumber = value.As<Napi::Number>();
  ref->SetObjectNumber(static_cast<const pdf_gennum>(objNumber));
}
Napi::Value
Ref::GetObjectNumber(const Napi::CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), ref->ObjectNumber());
}
Napi::Value
Ref::ToString(const Napi::CallbackInfo& info)
{
  return Napi::String::New(info.Env(), ref->ToString());
}
void
Ref::Write(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string output = info[0].As<Napi::String>().Utf8Value();
  try {
    PdfOutputDevice device(output.c_str());
    ref->Write(&device, ePdfWriteMode_Default);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Ref::IsIndirect(const Napi::CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), ref->IsIndirect());
}

Napi::Value
Ref::GetObj(const CallbackInfo& info)
{
  PdfObject obj(*ref);
  auto instancePtr = External<PdfObject>::New(info.Env(), &obj);
  return Obj::constructor.New({ instancePtr });
}
Ref::~Ref()
{
  if (ref != nullptr) {
    HandleScope scope(Env());
    delete ref;
  }
}
}
