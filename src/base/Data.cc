#include "Data.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

namespace NoPoDoFo {
using namespace PoDoFo;
using namespace Napi;

FunctionReference Data::constructor;

Data::Data(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() == 1 && info[0].Type() == napi_external) {
    self = info[0].As<External<PdfData>>().Data();
  } else if (info.Length() == 1 && info[0].IsString()) {
    string strData = info[0].As<String>().Utf8Value();
    self = new PdfData(strData.c_str());
  } else {
    throw Napi::TypeError::New(info.Env(),
                               "Data requires parameter of string or external");
  }
}

Data::~Data()
{
  if (self != nullptr) {
    HandleScope scope(Env());
    delete self;
  }
}

void
Data::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor =
    DefineClass(env,
                "Data",
                { InstanceAccessor("value", &Data::Value, nullptr),
                  InstanceMethod("write", &Data::Write) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Data", ctor);
}

void
Data::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_string });
  string output = info[0].As<String>().Utf8Value();
  if (output.empty() || output == "") {
    throw Error::New(info.Env(), "output must be valid path");
  }
  try {
    PdfOutputDevice device(output.c_str());
    self->Write(&device, ePdfWriteMode_Default);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
Data::Value(const CallbackInfo& info)
{
  return String::New(info.Env(), self->data());
}
}
