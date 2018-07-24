//
// Created by cory on 7/23/18.
//

#include "Date.h"
#include "../ValidateArguments.h"
#include <iostream>

using namespace Napi;
using namespace PoDoFo;

using option = tl::optional<napi_valuetype>;
using std::vector;
using tl::nullopt;
using std::cout;
using std::endl;

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
  Function ctor =
    DefineClass(env, "Date", { InstanceMethod("toString", &Date::ToString) });
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
}
