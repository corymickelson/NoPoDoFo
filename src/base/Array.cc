//
// Created by red on 10/2/17
//

#include "Array.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Obj.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

Napi::FunctionReference Array::constructor;

Array::Array(const CallbackInfo& info)
  : ObjectWrap<Array>(info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_external });
  array = new PdfArray(*info[0].As<External<PdfArray>>().Data());
}

void
Array::Initialize(Napi::Env& env, Napi::Object& target)
{
  Function ctor =
    DefineClass(env,
                "Array",
                { InstanceAccessor("dirty", &Array::IsDirty, &Array::SetDirty),
                  InstanceAccessor("length", &Array::Length, nullptr),
                  InstanceMethod("toArray", &Array::ToArray),
                  InstanceMethod("at", &Array::At),
                  InstanceMethod("contains", &Array::ContainsString),
                  InstanceMethod("indexOf", &Array::GetStringIndex),
                  InstanceMethod("write", &Array::Write),
                  InstanceMethod("push", &Array::Push),
                  InstanceMethod("pop", &Array::Pop) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Array", ctor);
}
Napi::Value
Array::Length(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), array->size());
}
void
Array::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string output = info[0].As<String>().Utf8Value();
  PdfOutputDevice device(output.c_str());
  array->Write(&device, ePdfWriteMode_Default);
}
Napi::Value
Array::ContainsString(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string searchString = info[0].As<String>().Utf8Value();
  bool match = array->ContainsString(searchString);
  return Napi::Boolean::New(info.Env(), match);
}
Napi::Value
Array::GetStringIndex(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string str = info[0].As<String>().Utf8Value();
  return Napi::Number::New(info.Env(), array->GetStringIndex(str));
}

Napi::Value
Array::IsDirty(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), array->IsDirty());
}

Napi::Value
Array::At(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  return GetObjAtIndex(info);
  //  size_t index = info[0].As<Number>().Uint32Value();
  //  PdfObject item = array[index];
  //  auto initPtr = Napi::External<PdfObject>::New(info.Env(), &item);
  //  auto instance = Obj::constructor.New({ initPtr });
  //  return instance;
}

void
Array::SetDirty(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsBoolean()) {
    throw Napi::Error::New(info.Env(), "dirty must be of type boolean");
  }
  array->SetDirty(value.As<Boolean>());
}

void
Array::Push(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto wrapper = info[0].As<Object>();
  if (!wrapper.InstanceOf(Obj::constructor.Value())) {
    throw Error::New(info.Env(), "must be an instance of Obj");
  }
  try {
    auto item = Obj::Unwrap(wrapper);
    array->push_back(item->GetObject());

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
}

Value
Array::Pop(const CallbackInfo& info)
{
  return GetObjAtIndex(info);
}

Value
Array::GetObjAtIndex(const CallbackInfo& info)
{
  size_t index = info[0].As<Number>().Uint32Value();
  PdfObject item = array[index];
  auto initPtr = Napi::External<PdfObject>::New(info.Env(), &item);
  auto instance = Obj::constructor.New({ initPtr });
  return instance;
}

Napi::Value
Array::ToArray(const Napi::CallbackInfo& info)
{
  auto js = Napi::Array::New(info.Env());
  try {
    uint32_t counter = 0;
    for (auto& it : *array) {
      const auto initPtr = External<PdfObject>::New(Env(), &it);
      const auto instance = Obj::constructor.New({ initPtr });
      js.Set(counter, instance);
      counter++;
    }
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return js;
}
Array::~Array()
{
  if (array != nullptr) {
    HandleScope scope(Env());
    delete array;
  }
}
}
