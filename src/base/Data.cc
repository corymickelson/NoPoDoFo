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

#include "Data.h"
#include "../ErrorHandler.h"
#include <spdlog/spdlog.h>

using namespace PoDoFo;
using namespace Napi;

using std::make_unique;
using std::string;

namespace NoPoDoFo {

FunctionReference Data::Constructor; // NOLINT

/**
 * @note JS new Data(data:string|Buffer)
 * @param info
 */
Data::Data(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() != 1) {
    TypeError::New(info.Env(), "Requires a string or Buffer")
      .ThrowAsJavaScriptException();
    return;
  }
  DbgLog = spdlog::get("DbgLog");
  if (info[0].IsString()) {
    const auto strData = info[0].As<String>().Utf8Value();
    Self = make_unique<PdfData>(strData.c_str());
  } else if (info[0].IsBuffer()) {
    auto bData = info[0].As<Buffer<char>>().Data();
    Self = make_unique<PdfData>(bData);
  } else {
    TypeError::New(info.Env(), "Requires a string or Buffer")
      .ThrowAsJavaScriptException();
    return;
  }
}

Data::~Data()
{
  DbgLog->debug("Data Cleanup");
}

void
Data::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(env,
                          "Data",
                          { InstanceAccessor("value", &Data::Value, nullptr),
                            InstanceMethod("write", &Data::Write) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Data", ctor);
}

void
Data::Write(const CallbackInfo& info)
{
  const auto output = info[0].As<String>().Utf8Value();
  if (output.empty()) {
    throw Error::New(info.Env(), "output must be valid path");
  }
  try {
    PdfOutputDevice device(output.c_str());
    Self->Write(&device, ePdfWriteMode_Default);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

JsValue
Data::Value(const CallbackInfo& info)
{
  return String::New(info.Env(), Self->data());
}
}
