#include "ListField.h"
#include "Field.h"

using namespace PoDoFo;
using namespace Napi;
namespace NoPoDoFo {
FunctionReference ListField::constructor;

ListField::ListField(const CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto wrap = info[0].As<Object>();
  auto fieldObj = Field::Unwrap(wrap)->GetField();
  field = &fieldObj;
  list = new PdfListField(*field);
}

void
ListField::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "ListField",
    { InstanceAccessor(
        "selected", &ListField::GetSelectedItem, &ListField::SetSelectedItem),
      InstanceAccessor("length", &ListField::GetItemCount, nullptr),
      InstanceMethod("insertItem", &ListField::InsertItem),
      InstanceMethod("removeItem", &ListField::RemoveItem),
      InstanceMethod("getItem", &ListField::GetItem) });
  target.Set("ListField", ctor);
}

void
ListField::InsertItem(const CallbackInfo& info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_string });
  string value = info[0].As<String>().Utf8Value();
  string display = info[1].As<String>().Utf8Value();
  list->InsertItem(PdfString(value), PdfString(display));
}

void
ListField::RemoveItem(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  if (index > list->GetItemCount() || index < 0) {
    throw Napi::Error::New(info.Env(), "index out of range");
  }
  list->RemoveItem(index);
}

Napi::Value
ListField::GetItem(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = info[0].As<Number>();
  Object item = Object::New(info.Env());
  string value = list->GetItem(index).GetString();
  string display = list->GetItemDisplayText(index).GetString();
  item.Set(String::New(info.Env(), "value"), String::New(info.Env(), value));
  item.Set(String::New(info.Env(), "display"),
           String::New(info.Env(), display));
  return item;
}

Napi::Value
ListField::GetItemCount(const CallbackInfo& info)
{
  return Number::New(info.Env(), list->GetItemCount());
}

void
ListField::SetSelectedItem(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsNumber()) {
    throw Napi::Error::New(info.Env(), "index must be of type number");
  }
  list->SetSelectedItem(value.As<Number>());
}

Napi::Value
ListField::GetSelectedItem(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int index = list->GetSelectedItem();
  return Number::New(info.Env(), index);
}
}
