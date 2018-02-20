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

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
FunctionReference Stream::constructor;

Stream::Stream(const CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_external });
  stream = info[0].As<External<PdfStream>>().Data();
}

void
Stream::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Stream",
    { InstanceMethod("write", &Stream::Write),
      InstanceAccessor("data", &Stream::GetBuffer, nullptr),
      InstanceAccessor("filteredData", &Stream::GetFilteredBuffer, nullptr) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Stream", ctor);
}

Napi::Value
Stream::GetBuffer(const CallbackInfo& info)
{
  pdf_long bufferLength = stream->GetLength();
  char* copy = static_cast<char*>(
    malloc(sizeof(char) * static_cast<unsigned long>(bufferLength)));
  stream->GetCopy(&copy, &bufferLength);
  auto value =
    Buffer<char>::Copy(info.Env(), copy, static_cast<size_t>(bufferLength));
  free(copy);
  return value;
}

Napi::Value
Stream::GetFilteredBuffer(const CallbackInfo& info)
{
  pdf_long bufferLength = stream->GetLength();
  char* copy = static_cast<char*>(
    malloc(sizeof(char) * static_cast<unsigned long>(bufferLength)));
  stream->GetFilteredCopy(&copy, &bufferLength);
  auto value =
    Buffer<char>::Copy(info.Env(), copy, static_cast<size_t>(bufferLength));
  free(copy);
  return value;
}

class StreamWriteAsync : public Napi::AsyncWorker
{
public:
  StreamWriteAsync(Napi::Function& cb, Stream* stream, string arg)
    : AsyncWorker(cb)
    , stream(stream)
    , arg(std::move(arg))
  {
  }
  ~StreamWriteAsync() {}

private:
  Stream* stream;
  string arg;

  // AsyncWorker interface
protected:
  void Execute()
  {
    try {
      PdfOutputDevice device(arg.c_str());
      stream->GetStream()->Write(&device);
    } catch (PdfError& err) {
      SetError(String::New(Env(), ErrorHandler::WriteMsg(err)));
    }
  }
  void OnOK()
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), arg) });
  }
};

Napi::Value
Stream::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_function });
  string output = info[0].As<String>().Utf8Value();
  Function cb = info[1].As<Function>();
  StreamWriteAsync* worker = new StreamWriteAsync(cb, this, output);
  worker->Queue();
  return info.Env().Undefined();
}
Stream::~Stream()
{
  if (stream != nullptr) {
    HandleScope scope(Env());
    delete stream;
  }
}
}
