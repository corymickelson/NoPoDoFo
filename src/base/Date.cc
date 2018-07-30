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

#include "Date.h"
#include "../ValidateArguments.h"
#include <iostream>

using namespace Napi;
using namespace PoDoFo;

using option = tl::optional<napi_valuetype>;
using std::cout;
using std::endl;
using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Date::constructor; // NOLINT

/**
 * PdfDate, to construct a new date from JS value the value
 * must be a string in the following format: (D:YYYYMMDDHHmmSSOHH'mm')
 * @param info
 */
Date::Date(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  vector<int> argIndex = AssertCallbackInfo(
    info, { { 0, { option(napi_string), nullopt, option(napi_external) } } });
  cout << argIndex[0] << endl;
  if (argIndex[0] == 0) {
    timestamp = new PdfDate(info[0].As<String>().Utf8Value());
    if (!timestamp->IsValid()) {
      Error::New(info.Env(),
                 "Invalid Date format. See NoPoDoFo Date docs for details.")
        .ThrowAsJavaScriptException();
    }
  } else if (argIndex[0] == 1) {
    timestamp = new PdfDate();
  } else if (argIndex[0] == 2) {
    PdfDate* copy = info[0].As<External<PdfDate>>().Data();
    timestamp = new PdfDate(*copy);
  }
}
Date::~Date()
{
  HandleScope scope(Env());
  delete timestamp;
}
void
Date::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(env,
                              "Date",
                              { InstanceMethod("toString", &Date::ToString),
                                InstanceMethod("isValid", &Date::IsValid) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Date", ctor);
}
Napi::Value
Date::ToString(const CallbackInfo& info)
{
  PdfString value;
  timestamp->ToString(value);
  return String::New(info.Env(), value.GetStringUtf8());
}
Napi::Value
Date::IsValid(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), timestamp->IsValid());
}
}
