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
    Self =
      new PdfAction(info[0].As<External<PdfAction>>().Data()->GetObject());
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
  } else {
    TypeError::New(
      info.Env(),
      "Invalid Action constructor args. Please see the docs for more info.")
      .ThrowAsJavaScriptException();
  }
  DbgLog = spdlog::get("DbgLog");
}
Action::~Action()
{
  DbgLog->debug("Action Cleanup");
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
}
} // namespace NoPoDoFo
