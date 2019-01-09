//
// Created by oak on 5/19/18.
//

#include "Action.h"
#include "../ValidateArguments.h"
#include "../base/Dictionary.h"
#include "../base/Obj.h"
#include "Document.h"
#include "StreamDocument.h"

using namespace PoDoFo;
using namespace Napi;

using std::vector;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference Action::constructor; // NOLINT

Action::Action(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  vector<int> opts =
    AssertCallbackInfo(info,
                       { { 0, { option(napi_external), option(napi_object) } },
                         { 1, { nullopt, option(napi_number) } } });
  // Create Copy Constructor Action
  if (opts[0] == 0 && info.Length() == 1) {
    action =
      new PdfAction(info[0].As<External<PdfAction>>().Data()->GetObject());
  }
  // Create a new Action
  else if (opts[0] == 1 && opts[1] == 1) {
    PdfDocument* doc;
    if (info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
      doc = Document::Unwrap(info[0].As<Object>())->base;
    } else if (info[0].As<Object>().InstanceOf(
                 StreamDocument::constructor.Value())) {
      doc = StreamDocument::Unwrap(info[0].As<Object>())->base;
    } else {
      Error::New(info.Env(), "Instance of Base is required")
        .ThrowAsJavaScriptException();
      return;
    }
    EPdfAction t = static_cast<EPdfAction>(info[1].As<Number>().Uint32Value());
    action = new PdfAction(t, doc);
  } else {
    TypeError::New(
      info.Env(),
      "Invalid Action constructor args. Please see the docs for more info.")
      .ThrowAsJavaScriptException();
  }
}
Action::~Action()
{
  delete action;
}
void
Action::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
    env,
    "Action",
    { InstanceAccessor("type", &Action::GetType, nullptr),
      InstanceAccessor("uri", &Action::GetUri, &Action::SetUri),
      InstanceAccessor("script", &Action::GetScript, &Action::SetScript),
      InstanceMethod("getObject", &Action::GetObject),
      InstanceMethod("addToDictionary", &Action::AddToDictionary) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Action", ctor);
}
Napi::Value
Action::GetUri(const Napi::CallbackInfo& info)
{
  if (GetAction().HasURI()) {
    return String::New(info.Env(), GetAction().GetURI().GetStringUtf8());
  }
  return info.Env().Null();
}
Napi::Value
Action::GetScript(const Napi::CallbackInfo& info)
{
  if (GetAction().HasScript()) {
    return String::New(info.Env(), GetAction().GetScript().GetStringUtf8());
  }
  return info.Env().Null();
}
Napi::Value
Action::GetType(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<int>(GetAction().GetType()));
}
void
Action::SetUri(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  GetAction().SetURI(PdfString(value.As<String>().Utf8Value()));
}
void
Action::SetScript(const Napi::CallbackInfo& info, const Napi::Value& value)
{
  GetAction().SetScript(PdfString(value.As<String>().Utf8Value()));
}
Napi::Value
Action::GetObject(const Napi::CallbackInfo& info)
{
  PdfObject* o = GetAction().GetObject();
  return Obj::constructor.New({ External<PdfObject>::New(info.Env(), o) });
}
void
Action::AddToDictionary(const Napi::CallbackInfo& info)
{
  auto dictionary = Dictionary::Unwrap(info[0].As<Object>())->GetDictionary();
  GetAction().AddToDictionary(dictionary);
}
} // namespace NoPoDoFo
