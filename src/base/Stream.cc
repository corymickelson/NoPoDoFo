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

#include "Stream.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Obj.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::string;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Stream::constructor; // NOLINT

Stream::Stream(const CallbackInfo& info)
  : ObjectWrap(info)
  , Self(info.Length() == 1 && info[0].IsExternal()
             ? *info[0].As<External<PdfStream>>().Data()
             : *Obj::Unwrap(info[0].As<Object>())->GetObject().GetStream())
{
  DbgLog = spdlog::get("DbgLog");
}
Stream::~Stream()
{
  DbgLog->debug("Stream Cleanup");
}
void
Stream::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor =
    DefineClass(env,
                "Stream",
                { InstanceMethod("write", &Stream::Write),
                  InstanceMethod("beginAppend", &Stream::BeginAppend),
                  InstanceMethod("append", &Stream::Append),
                  InstanceMethod("endAppend", &Stream::EndAppend),
                  InstanceMethod("inAppendMode", &Stream::IsAppending),
                  InstanceMethod("set", &Stream::Set),
                  InstanceMethod("copy", &Stream::GetCopy) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Stream", ctor);
}

class StreamWriteAsync final : public Napi::AsyncWorker
{
public:
  StreamWriteAsync(Napi::Function& cb, Stream* stream, string arg)
    : AsyncWorker(cb, "stream_write_async", stream->Value())
    , NStrm(stream)
    , Arg(std::move(arg))
  {}

private:
  Stream* NStrm;
  PdfRefCountedBuffer RefBuffer;
  PdfOutputDevice* Device = nullptr;
  string Arg;

  // AsyncWorker interface
protected:
  void Execute() override
  {
    try {
      if (!Arg.empty()) {
        PdfOutputDevice fileDevice(Arg.c_str());
        Device = &fileDevice;
      } else {
        PdfOutputDevice memDevice(&RefBuffer);
        Device = &memDevice;
      }
      NStrm->GetStream().Write(Device);
    } catch (PdfError& err) {
      SetError(String::New(Env(), ErrorHandler::WriteMsg(err)));
    }
  }
  void OnOK() override
  {
    if (Arg.empty()) {
      Callback().Call(
        { Env().Null(),
          Buffer<char>::Copy(Env(), RefBuffer.GetBuffer(), RefBuffer.GetSize()) });
    }
    Callback().Call({ Env().Null(), String::New(Env(), Arg) });
  }
};

JsValue
Stream::Write(const CallbackInfo& info)
{
  string output;
  Function cb;
  const auto opts =
    AssertCallbackInfo(info,
                       { { 0, { option(napi_function), option(napi_string) } },
                         { 1, { nullopt, option(napi_function) } } });
  if (opts[0] == 0) {
    cb = info[0].As<Function>();
  }
  if (opts[0] == 1) {
    output = info[0].As<String>();
  }
  if (opts[1] == 1) {
    cb = info[1].As<Function>();
  }
  StreamWriteAsync* worker = new StreamWriteAsync(cb, this, output);
  worker->Queue();
  return info.Env().Undefined();
}

void
Stream::BeginAppend(const Napi::CallbackInfo& info)
{
  const auto opt = AssertCallbackInfo(
    info,
    { { 0, { nullopt, option(napi_boolean), option(napi_external) } },
      { 1, { nullopt, option(napi_boolean) } },
      { 2, { nullopt, option(napi_boolean) } } });
  auto clearExisting = true;
  auto deleteFilter = true;
  std::vector<EPdfFilter>* filters = nullptr;
  if (opt[0] == 0) {
    GetStream().BeginAppend();
    return;
  } else if (opt[0] == 1 || opt[1] == 1) {
    clearExisting = info[0].As<Boolean>();
  } else if (opt[0] == 2) {
    filters = info[0].As<External<std::vector<EPdfFilter>>>().Data();
  }
  if (opt[2] == 1) {
    deleteFilter = info[2].As<Boolean>();
  }
  if (!filters) {
    GetStream().BeginAppend(clearExisting);
  } else {
    GetStream().BeginAppend(*filters, clearExisting, deleteFilter);
  }
}
void
Stream::Set(const Napi::CallbackInfo& info)
{
  if (info.Length() != 1 || (!info[0].IsString() && !info[0].IsBuffer())) {
    Error::New(info.Env(), "A string or buffer is required")
      .ThrowAsJavaScriptException();
    return;
  }
  if (info[0].IsBuffer()) {
    const auto data = info[0].As<Buffer<char>>().Data();
    GetStream().Set(data);
  } else if (info[0].IsString()) {
    const auto data = info[0].As<String>().Utf8Value();
    GetStream().Set(data.c_str());
  }
}
void
Stream::Append(const Napi::CallbackInfo& info)
{
  if (info.Length() != 1 || (!info[0].IsString() && !info[0].IsBuffer())) {
    Error::New(info.Env(), "A string or buffer is required")
      .ThrowAsJavaScriptException();
    return;
  }

  try {
    if (info[0].IsBuffer()) {
      const auto data = info[0].As<Buffer<char>>();
      GetStream().Append(data.Data(), data.Length());
    } else if (info[0].IsString()) {
      const auto data = info[0].As<String>().Utf8Value();
      GetStream().Append(data);
    }
  } catch (PdfError& err) {
    Error::New(info.Env(), err.what()).ThrowAsJavaScriptException();
  }
}
void
Stream::EndAppend(const Napi::CallbackInfo&)
{
  GetStream().EndAppend();
}
JsValue
Stream::IsAppending(const Napi::CallbackInfo& info)
{
  return Boolean::New(info.Env(), GetStream().IsAppending());
}
JsValue
Stream::Length(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), GetStream().GetLength());
}
JsValue
Stream::GetCopy(const Napi::CallbackInfo& info)
{
  auto filtered = false;
  auto l = GetStream().GetLength();
  auto internalBuffer = static_cast<char*>(podofo_malloc(sizeof(char) * l));
  if (info.Length() == 1 && info[0].IsBoolean()) {
    filtered = info[0].As<Boolean>();
  }
  if (filtered) {
    GetStream().GetFilteredCopy(&internalBuffer, &l);
  } else {
    GetStream().GetCopy(&internalBuffer, &l);
  }
  const auto output = Buffer<char>::Copy(info.Env(), internalBuffer, l);
  podofo_free(internalBuffer);
  return JsValue(output);
}
}
