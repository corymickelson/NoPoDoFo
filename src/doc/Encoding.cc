//
// Created by red on 11/11/17.
//

#include "Encoding.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Dictionary.h"
#include "Font.h"

using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference Encoding::constructor;

Encoding::Encoding(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_external });
  encoding = info[0].As<External<PdfEncoding>>().Data();
}
void
Encoding::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Encoding",
    { InstanceMethod("addToDictionary", &Encoding::AddToDictionary),
      InstanceMethod("convertToUnicode", &Encoding::ConvertToUnicode),
      InstanceMethod("convertToEncoding", &Encoding::ConvertToEncoding),
      InstanceMethod("data", &Encoding::GetData) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Encoding", ctor);
}
Napi::Value
Encoding::AddToDictionary(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto wrap = info[0].As<Object>();
  Dictionary* d = Dictionary::Unwrap(wrap);
  PdfDictionary* dict = d->GetDictionary();
  encoding->AddToDictionary(*dict);
  delete d;
  delete dict;
  return info.Env().Undefined();
}
Napi::Value
Encoding::ConvertToUnicode(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_object });
  string content = info[0].As<String>().Utf8Value();
  Font* font = Font::Unwrap(info[1].As<Object>());
  PdfString buffer =
    encoding->ConvertToUnicode(PdfString(content), font->GetPoDoFoFont());
  return String::New(info.Env(), buffer.GetStringUtf8());
}
Napi::Value
Encoding::ConvertToEncoding(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_object });
  string content = info[0].As<String>().Utf8Value();
  Font* font = Font::Unwrap(info[1].As<Object>());
  PdfRefCountedBuffer buffer =
    encoding->ConvertToEncoding(PdfString(content), font->GetPoDoFoFont());
  return String::New(info.Env(), buffer.GetBuffer());
}
Napi::Value
Encoding::GetData(const Napi::CallbackInfo& info)
{
  return Value();
}
Encoding::~Encoding() {
  if(encoding != nullptr) {
    HandleScope scope(Env());
    encoding = nullptr;
  }
}
}
