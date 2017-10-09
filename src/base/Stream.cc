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
    DefineClass(env, "Stream", { InstanceMethod("write", &Stream::Write) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Stream", constructor);
}

void
Stream::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  try {
    string output = info[0].As<String>().Utf8Value();
    PdfOutputDevice device(output.c_str());
    stream->Write(&device);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}
