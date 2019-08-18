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

#include "Date.h"
#include "../ValidateArguments.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using Option = tl::optional<napi_valuetype>;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Date::Constructor; // NOLINT

/**
 * PdfDate, to construct a new date from JS value the value
 * must be a string in the following format: (D:YYYYMMDDHHmmSSOHH'mm')
 * @param info
 */
Date::Date(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  auto argIndex = AssertCallbackInfo(
    info, { { 0, { option(napi_string), nullopt, option(napi_external) } } });
  Log = spdlog::get("Log");
  if (argIndex[0] == 0) {
    Self = new PdfDate(info[0].As<String>().Utf8Value());
    if (!Self->IsValid()) {
      Error::New(info.Env(),
                 "Invalid Date format. See NoPoDoFo Date docs for details.")
        .ThrowAsJavaScriptException();
    }
  } else if (argIndex[0] == 1) {
    Self = new PdfDate();
  } else if (argIndex[0] == 2) {
    const auto copy = info[0].As<External<PdfDate>>().Data();
    Self = new PdfDate(*copy);
  }
}
Date::~Date()
{
  if(Log != nullptr) Log->debug("Date Cleanup");
  HandleScope scope(Env());
  delete Self;
}
void
Date::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(env,
                              "Date",
                              { InstanceMethod("toString", &Date::ToString),
                                InstanceMethod("isValid", &Date::IsValid) });
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Date", ctor);
}
JsValue
Date::ToString(const CallbackInfo& info)
{
  PdfString value;
  Self->ToString(value);
  return String::New(info.Env(), value.GetStringUtf8());
}
JsValue
Date::IsValid(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), Self->IsValid());
}
}
