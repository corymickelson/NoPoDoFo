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

#include "Encoding.h"
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "../base/Dictionary.h"
#include "Font.h"
#include <spdlog/spdlog.h>

using namespace PoDoFo;
using namespace Napi;

using std::string;

namespace NoPoDoFo {

FunctionReference Encoding::Constructor; // NOLINT

Encoding::Encoding(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , Self(info[0].As<External<PdfEncoding>>().Data())
{
  Log = spdlog::get("Log");
}
Encoding::~Encoding()
{
  Logger(Log, spdlog::level::trace, "Encoding Cleanup");
  if (!Self->IsAutoDelete()) {
    Logger(
      Log, spdlog::level::trace, "Encoding is NOT auto deleted, deleting now");
    delete Self;
  } else {
    Logger(Log,
           spdlog::level::trace,
           "Encoding is an auto deleted object, nothing deleted");
  }
}
void
Encoding::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  const char* name = "Encoding";
  Function ctor = DefineClass(
    env,
    name,
    { InstanceMethod("addToDictionary", &Encoding::AddToDictionary),
      InstanceMethod("convertToUnicode", &Encoding::ConvertToUnicode),
      InstanceMethod("convertToEncoding", &Encoding::ConvertToEncoding) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set(name, ctor);
}
Napi::Value
Encoding::AddToDictionary(const Napi::CallbackInfo& info)
{
  auto wrap = info[0].As<Object>();
  auto d = Dictionary::Unwrap(wrap);
  Self->AddToDictionary(d->GetDictionary());
  return info.Env().Undefined();
}
Napi::Value
Encoding::ConvertToUnicode(const Napi::CallbackInfo& info)
{
  const string content = info[0].As<String>().Utf8Value();
  auto font = Font::Unwrap(info[1].As<Object>());
  const auto buffer =
    Self->ConvertToUnicode(PdfString(content), &font->GetFont());
  return String::New(info.Env(), buffer.GetStringUtf8());
}
Napi::Value
Encoding::ConvertToEncoding(const Napi::CallbackInfo& info)
{
  const string content = info[0].As<String>().Utf8Value();
  auto font = Font::Unwrap(info[1].As<Object>());
  const auto buffer =
    Self->ConvertToEncoding(PdfString(content), &font->GetFont());
  return String::New(info.Env(), buffer.GetBuffer());
}

}
