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

#include "TextField.h"


namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

FunctionReference TextField::constructor;

TextField::TextField(const CallbackInfo& info)
  : ObjectWrap(info)
{
  try {
    auto fieldObj = info[0].As<Object>();
    auto field = Field::Unwrap(fieldObj);
    PdfTextField v(field->GetField());
    text = make_unique<PdfTextField>(v);
  } catch (PdfError& err) {
    stringstream msg;
    msg << "Failed to instantiate TextField. PoDoFo Error: " << err.GetError()
        << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

void
TextField::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "TextField",
    { InstanceAccessor("text", &TextField::GetText, &TextField::SetText) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("TextField", ctor);
}
void
TextField::SetText(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsString()) {
    throw Napi::Error::New(info.Env(),
                           "TextField value must be of type string");
  }
  string input = value.As<String>().Utf8Value();
  PoDoFo::PdfString v(input);
  (*text).SetText(v);
}

Napi::Value
TextField::GetText(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(),
      (*text).GetText().GetStringUtf8());
}

}
