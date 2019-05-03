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

#include "Obj.h"
#include "../ErrorHandler.h"
#include "Array.h"
#include "Dictionary.h"
#include "Ref.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::endl;
using std::string;
using std::stringstream;

namespace NoPoDoFo {

FunctionReference Obj::Constructor; // NOLINT

/**
 * Creates a new instance of PdfObject, this object is assigned in the
 * constructor of NoPoDoFo::Object and is removed with the NoPoDoFo::Object
 * instance.
 *
 * @param info
 * @return
 */
PdfObject*
InitObject(const CallbackInfo& info)
{
  if (info.Length() == 0) {
    return new PdfObject();
  }
  if (info.Length() == 1) {
    if (info[0].IsArray()) {
      const auto array = info[0].As<Napi::Array>();
      PdfArray pdfArray;
      for (uint32_t i = 0; i < array.Length(); i++) {
        auto ii = array.Get(i);
        if (ii.IsBoolean()) {
          const bool x = ii.As<Boolean>();
          pdfArray[i] = x;
        }
        if (ii.IsNumber()) {
          pdfArray[i] = ii.As<Number>().DoubleValue();
        }
        if (ii.IsString()) {
          auto x = ii.As<String>().Utf8Value();
          pdfArray[i] = PdfString(x);
        } else {
          Error::New(
            info.Env(),
            "NoPoDoFo currently only supports homogeneous primitive type array")
            .ThrowAsJavaScriptException();
        }
      }
      return new PdfObject(pdfArray);
    }
    if (info[0].IsString()) {
      const PdfString s(info[0].As<String>());
      return new PdfObject(s);
    }
    if (info[0].IsNumber()) {
      return new PdfObject(info[0].As<Number>().DoubleValue());
    }
    if (info[0].IsBoolean()) {
      return new PdfObject(info[0].As<Boolean>());
    }
  }
  return new PdfObject();
}
void
Obj::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor = DefineClass(
    env,
    "Object",
    { InstanceAccessor("stream", &Obj::GetStream, nullptr),
      InstanceAccessor("type", &Obj::GetDataType, nullptr),
      InstanceAccessor("length", &Obj::GetObjectLength, nullptr),
      InstanceAccessor("reference", &Obj::Reference, nullptr),
      InstanceAccessor("immutable", &Obj::GetImmutable, &Obj::SetImmutable),
      InstanceMethod("hasStream", &Obj::HasStream),
      InstanceMethod("getOffset", &Obj::GetByteOffset),
      InstanceMethod("write", &Obj::Write),
      InstanceMethod("flateCompressStream", &Obj::FlateCompressStream),
      InstanceMethod("delayedStreamLoad", &Obj::DelayedStreamLoad),
      InstanceMethod("getBool", &Obj::GetBool),
      InstanceMethod("getDictionary", &Obj::GetDictionary),
      InstanceMethod("getNumber", &Obj::GetNumber),
      InstanceMethod("getReal", &Obj::GetReal),
      InstanceMethod("getString", &Obj::GetString),
      InstanceMethod("getName", &Obj::GetName),
      InstanceMethod("getArray", &Obj::GetArray),
      InstanceMethod("getRawData", &Obj::GetRawData),
      InstanceMethod("clear", &Obj::Clear),
      InstanceMethod("resolveIndirectKey", &Obj::MustGetIndirect) });
  Constructor = Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("Object", ctor);
}

Obj::Obj(const Napi::CallbackInfo& info)
  : ObjectWrap<Obj>(info)
  , NObj(info.Length() == 1 && info[0].IsExternal()
          ? *info[0].As<External<PdfObject>>().Data()
          : *(Init = InitObject(info)))
{
  DbgLog = spdlog::get("DbgLog");
  if(Init != nullptr) {
    DbgLog->debug("New Object Created");
  }
}
Obj::~Obj()
{
  DbgLog->debug("Object Cleanup");
  HandleScope scope(Env());
  delete Init;
}
void
Obj::Clear(const Napi::CallbackInfo& info)
{
  try {
    NObj.Clear();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Obj::GetStream(const CallbackInfo& info)
{
  try {
    if (!NObj.HasStream()) {
      stringstream output;
      output << "/tmp/" << NObj.Reference().GenerationNumber() << "."
             << NObj.Reference().ObjectNumber() << ".txt" << endl;
      const auto outfile = output.str().c_str();
      stringstream msg;
      msg << "This Object does not have a stream associated with it" << endl;
      cout << "Writing Object to: " << outfile << endl;
      DbgLog->debug(msg.str());
      PdfOutputDevice outDevice(outfile);
      NObj.WriteObject(&outDevice, ePdfWriteMode_Clean, nullptr);
      return info.Env().Undefined();
    }
    const auto pStream = dynamic_cast<PdfMemStream*>(NObj.GetStream());
    const auto stream = pStream->Get();
    const auto length = pStream->GetLength();
    const auto value =
      Buffer<char>::Copy(info.Env(), stream, static_cast<size_t>(length));
    return Napi::Value(value);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

JsValue
Obj::HasStream(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), NObj.HasStream());
}

JsValue
Obj::GetObjectLength(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(),
                           NObj.GetObjectLength(ePdfWriteMode_Default));
}

JsValue
Obj::GetImmutable(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), NObj.GetImmutable());
}
void
Obj::SetImmutable(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsBoolean()) {
    try {
      NObj.SetImmutable(value.As<Boolean>());
    } catch (PdfError& err) {
      ErrorHandler(err, info);
    }
  }
}

JsValue
Obj::GetDataType(const CallbackInfo& info)
{
  string js;
  if (NObj.IsArray()) {
    js = "Array";
  } else if (NObj.IsBool()) {
    js = "Boolean";
  } else if (NObj.IsDictionary()) {
    js = "Dictionary";
  } else if (NObj.IsEmpty()) {
    js = "Empty";
  } else if (NObj.IsHexString()) {
    js = "HexString";
  } else if (NObj.IsNull()) {
    js = "Null";
  } else if (NObj.IsNumber()) {
    js = "Number";
  } else if (NObj.IsName()) {
    js = "Name";
  } else if (NObj.IsRawData()) {
    js = "RawData";
  } else if (NObj.IsReal()) {
    js = "Real";
  } else if (NObj.IsReference()) {
    js = "Reference";
  } else if (NObj.IsString()) {
    js = "String";
  } else {
    js = "Unknown";
  }
  return Napi::String::New(info.Env(), js);
}

JsValue
Obj::Reference(const CallbackInfo& info)
{
  auto r = GetObject().Reference();
  return Ref::Constructor.New({ External<PdfReference>::New(info.Env(), &r) });
}

void
Obj::FlateCompressStream(const CallbackInfo& info)
{
  try {
    NObj.FlateCompressStream();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Obj::DelayedStreamLoad(const CallbackInfo& info)
{
  try {
    NObj.DelayedStreamLoad();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

JsValue
Obj::GetNumber(const CallbackInfo& info)
{
  if (!NObj.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }
  return Number::New(info.Env(), NObj.GetNumber());
}

JsValue
Obj::GetReal(const CallbackInfo& info)
{
  if (!NObj.IsReal()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a number");
  }

  return Number::New(info.Env(), NObj.GetReal());
}

JsValue
Obj::GetString(const CallbackInfo& info)
{
  if (!NObj.IsString() && !NObj.IsHexString()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a String");
  }
  return String::New(info.Env(), NObj.GetString().GetStringUtf8());
}

JsValue
Obj::GetName(const CallbackInfo& info)
{
  if (!NObj.IsName()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as a string");
  }
  try {
    const auto name = NObj.GetName().GetName();
    return String::New(info.Env(), name);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

JsValue
Obj::GetArray(const CallbackInfo& info)
{
  if (!NObj.IsArray()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as array");
  }
  const auto instance = Array::Constructor.New(
    { External<PdfArray>::New(info.Env(), &NObj.GetArray()),
      Number::New(info.Env(), 1) });
  return instance;
}

JsValue
Obj::GetBool(const CallbackInfo& info)
{
  if (!NObj.IsNumber()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a boolean");
  }
  return Boolean::New(info.Env(), NObj.GetBool());
}

JsValue
Obj::GetDictionary(const CallbackInfo& info)
{
  if (!NObj.IsDictionary()) {
    throw Napi::Error::New(info.Env(), "Obj only accessible as Dictionary");
  }
  return Dictionary::Constructor.New(
    { External<PdfObject>::New(info.Env(), &NObj), Number::New(info.Env(), 0) });
}

JsValue
Obj::GetRawData(const CallbackInfo& info)
{
  if (!NObj.IsRawData()) {
    throw Napi::Error::New(info.Env(), "Obj not accessible as a buffer");
  }
  const auto data = NObj.GetRawData().data();
  return Napi::Value(Buffer<char>::Copy(info.Env(), data.c_str(), data.length()));
}

class ObjOffsetAsync final : public Napi::AsyncWorker
{
public:
  ObjOffsetAsync(Napi::Function& cb, Obj* obj, string arg)
    : Napi::AsyncWorker(cb)
    , NpdfObj(obj)
    , Arg(std::move(arg))
  {}

protected:
  void Execute() override
  {
    try {
      auto o = NpdfObj->GetObject();
      Value = o.GetByteOffset(Arg.c_str(), ePdfWriteMode_Default);
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Napi::Error& err) {
      SetError(err.Message());
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), Number::New(Env(), Value) });
  }

private:
  Obj* NpdfObj;
  string Arg;
  long Value = -1;
};

JsValue
Obj::GetByteOffset(const CallbackInfo& info)
{
  const auto arg = info[0].As<String>().Utf8Value();
  auto cb = info[1].As<Function>();
  auto worker = new ObjOffsetAsync(cb, this, arg);
  worker->Queue();
  return info.Env().Undefined();
}

class ObjWriteAsync final : public Napi::AsyncWorker
{
public:
  ObjWriteAsync(Napi::Function& cb, Obj* obj, string dest)
		: AsyncWorker(cb), Self(obj)
    , Arg(std::move(dest))
  {}

protected:
  void Execute() override
  {
    try {
      PdfOutputDevice device(Arg.c_str());
			Self->GetObject().WriteObject(&device, ePdfWriteMode_Default, nullptr);
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Napi::Error& err) {
      SetError(err.Message());
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), Arg) });
  }

private:
	Obj *Self;
  string Arg;
};

JsValue
Obj::Write(const CallbackInfo& info)
{
  auto cb = info[1].As<Function>();
  auto worker =
    new ObjWriteAsync(cb, this, info[0].As<String>().Utf8Value());
  worker->Queue();
  return info.Env().Undefined();
}
JsValue
Obj::MustGetIndirect(const CallbackInfo& info)
{
  if (info.Length() != 1 && !info[0].IsString()) {
    TypeError::New(info.Env(),
                   "The name of the indirect key is required, this does a "
                   "lookup in a Dictionary, and resolves"
                   "any Indirects to their Object value")
      .ThrowAsJavaScriptException();
  }
  const auto name = PdfName(info[0].As<String>());
  const auto target = NObj.MustGetIndirectKey(name);
  return Constructor.New({ External<PdfObject>::New(info.Env(), target) });
}

}
