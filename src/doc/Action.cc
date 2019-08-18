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

#include "Action.h"
#include "../ValidateArguments.h"
#include "../base/Dictionary.h"
#include "../base/Obj.h"
#include "Document.h"
#include "StreamDocument.h"
#include <spdlog/spdlog.h>

using namespace PoDoFo;
using namespace Napi;

using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Action::Constructor; // NOLINT

Action::Action(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  const auto opts =
    AssertCallbackInfo(info,
                       { { 0, { option(napi_external), option(napi_object) } },
                         { 1, { nullopt, option(napi_number) } } });
  // Create Copy Constructor Action
  if (opts[0] == 0 && info.Length() == 1) {
    Self = new PdfAction(info[0].As<External<PdfAction>>().Data()->GetObject());
    Logger(
      "Log", spdlog::level::trace, "PdfAction Copied from external object");
  }
  // Create a new Action
  else if (opts[0] == 1 && opts[1] == 1) {
    PdfDocument* doc;
    if (info[0].As<Object>().InstanceOf(Document::Constructor.Value())) {
      doc = Document::Unwrap(info[0].As<Object>())->Base;
    } else if (info[0].As<Object>().InstanceOf(
                 StreamDocument::Constructor.Value())) {
      doc = StreamDocument::Unwrap(info[0].As<Object>())->Base;
    } else {
      Error::New(info.Env(), "Instance of Base is required")
        .ThrowAsJavaScriptException();
      return;
    }
    const auto t = static_cast<EPdfAction>(info[1].As<Number>().Uint32Value());
    Self = new PdfAction(t, doc);
    Logger("Log", spdlog::level::trace, "new PdfAction created");
  } else {
    TypeError::New(
      info.Env(),
      "Invalid Action constructor args. Please see the docs for more info.")
      .ThrowAsJavaScriptException();
  }
}
Action::~Action()
{
  Logger("Log", spdlog::level::trace, "Action cleanup");
  delete Self;
}
void
Action::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  const auto ctor = DefineClass(
    env,
    "Action",
    { InstanceAccessor("type", &Action::GetType, nullptr),
      InstanceAccessor("uri", &Action::GetUri, &Action::SetUri),
      InstanceAccessor("script", &Action::GetScript, &Action::SetScript),
      InstanceMethod("getObject", &Action::GetObject),
      InstanceMethod("addToDictionary", &Action::AddToDictionary) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Action", ctor);
}
JsValue
Action::GetUri(const Napi::CallbackInfo& info)
{
  if (GetAction().HasURI()) {
    return String::New(info.Env(), GetAction().GetURI().GetStringUtf8());
  }
  return info.Env().Null();
}
JsValue
Action::GetScript(const Napi::CallbackInfo& info)
{
  if (GetAction().HasScript()) {
    return String::New(info.Env(), GetAction().GetScript().GetStringUtf8());
  }
  return info.Env().Null();
}
JsValue
Action::GetType(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<int>(GetAction().GetType()));
}
void
Action::SetUri(const Napi::CallbackInfo& info, const JsValue& value)
{
  GetAction().SetURI(PdfString(value.As<String>().Utf8Value()));
}
void
Action::SetScript(const Napi::CallbackInfo& info, const JsValue& value)
{
  GetAction().SetScript(PdfString(value.As<String>().Utf8Value()));
}
JsValue
Action::GetObject(const Napi::CallbackInfo& info)
{
  const auto o = GetAction().GetObject();
  return Obj::Constructor.New({ External<PdfObject>::New(info.Env(), o) });
}
void
Action::AddToDictionary(const Napi::CallbackInfo& info)
{
  auto dictionary = Dictionary::Unwrap(info[0].As<Object>())->GetDictionary();
  GetAction().AddToDictionary(dictionary);
  if (spdlog::get("Log") != nullptr &&
      spdlog::get("Log")->level() <= spdlog::level::debug) {
    PdfRefCountedBuffer rBuf;
    PdfOutputDevice device(&rBuf);
    dictionary.Write(&device, ePdfWriteMode_Clean);
    Logger("Log",
           spdlog::level::debug,
           "Action::AddToDictionary: Dictionary after operation = {}",
           rBuf.GetBuffer());
  }
}
} // namespace NoPoDoFo
