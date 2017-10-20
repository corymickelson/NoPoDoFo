//
// Created by red on 10/1/17.
//

#include "Stream.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

using namespace Napi;
using namespace PoDoFo;

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
  Function ctor =
    DefineClass(env,
                "Stream",
                { InstanceMethod("write", &Stream::Write),
                  InstanceMethod("getBuffer", &Stream::GetBuffer),
                  InstanceMethod("getFilteredBuffer", &Stream::GetFilteredBuffer) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Stream", ctor);
}

Napi::Value
Stream::Write(const CallbackInfo& info)
{
  auto resolver = Promise::Resolver::New(info.Env());
  if(info.Length() != 1 && !info[0].IsString()) {
    resolver.Reject(String::New(info.Env(), "write requires a single string argument"));
  }
  string output = info[0].As<String>().Utf8Value();
  try {
    PdfOutputDevice device(output.c_str());
    stream->Write(&device);
    resolver.Resolve(String::New(info.Env(), output));
  } catch (PdfError& err) {
    resolver.Reject(String::New(info.Env(), ErrorHandler::WriteMsg(err)));
  } catch (Napi::Error& err) {
    resolver.Reject(String::New(info.Env(), ErrorHandler::WriteMsg(err)));
  }
  return resolver.Promise();
}

Napi::Value
Stream::GetBuffer(const CallbackInfo& info)
{
  pdf_long bufferLength = stream->GetLength();
  char* copy = static_cast<char*>(malloc(sizeof(char) * bufferLength));
  stream->GetCopy(&copy, &bufferLength);
  auto value = Buffer<char>::Copy(
    info.Env(), copy, static_cast<size_t>(bufferLength));
  free(copy);
  return value;
}

Napi::Value
Stream::GetFilteredBuffer(const CallbackInfo &info)
{
  pdf_long bufferLength = stream->GetLength();
  char* copy = static_cast<char*>(malloc(sizeof(char) * bufferLength));
  stream->GetFilteredCopy(&copy, &bufferLength);
  auto value = Buffer<char>::Copy(
    info.Env(), copy, static_cast<size_t>(bufferLength));
  free(copy);
  return value;
}
