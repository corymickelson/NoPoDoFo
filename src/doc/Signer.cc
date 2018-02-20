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

#include "Signer.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
FunctionReference Signer::constructor;

Signer::Signer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_object });
  doc = Document::Unwrap(info[0].As<Object>());
  if (!doc->LoadedForIncrementalUpdates()) {
    throw Napi::Error::New(info.Env(),
                           "Please reload Document with forUpdates = true.");
  }
}

Signer::~Signer()
{
  HandleScope scope(Env());
  if (doc != nullptr)
    doc = nullptr;
  if (field != nullptr)
    field = nullptr;
}

void
Signer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor =
    DefineClass(env,
                "Signer",
                { InstanceMethod("signSync", &Signer::Sign),
                  InstanceMethod("sign", &Signer::SignWorker),
                  InstanceMethod("getField", &Signer::GetField),
                  InstanceMethod("setField", &Signer::SetField) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Signer", ctor);
}

void
Signer::SetField(const CallbackInfo& info)
{
  field = SignatureField::Unwrap(info[0].As<Object>());
  try {
    field->GetField()->EnsureSignatureObject();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
Signer::GetField(const CallbackInfo& info)
{
  return SignatureField::constructor.New(
    { External<PdfSignatureField>::New(info.Env(), field->GetField()) });
}

Napi::Value
Signer::Sign(const CallbackInfo& info)
{
  try {

    string data, output;
    string sigStr = info[0].As<String>().Utf8Value();
    pdf_long sigStrLength = static_cast<pdf_long>(sigStr.size());

    PdfSignatureField* pSignField = field->GetField();
    if (pSignField->GetFieldName().GetStringUtf8().empty()) {
      pSignField->SetFieldName("NoPoDoFo::SignatureField");
    }
    auto document = doc->GetDocument();

    PdfRefCountedBuffer r;
    PdfOutputDevice outputDevice;
    if (info.Length() == 2 && info[1].IsString()) {
      output = info[1].As<String>().Utf8Value();
      outputDevice = *new PdfOutputDevice(output.c_str(), true);
    } else {
      outputDevice = *new PdfOutputDevice(&r);
    }
    PdfSignOutputDevice signer(&outputDevice);
    signer.SetSignatureSize(static_cast<size_t>(sigStrLength));

    pSignField->SetSignatureDate(PdfDate());
    pSignField->SetSignature(*signer.GetSignatureBeacon());
    document->WriteUpdate(&signer, true);

    if (!signer.HasSignaturePosition())
      throw Error::New(info.Env(),
                       "Cannot find signature position in the document data");

    signer.AdjustByteRange();
    signer.Seek(0);

    PdfData signature(sigStr.c_str(), static_cast<size_t>(sigStrLength));
    signer.SetSignature(signature);
    signer.Flush();
    return info.Env().Undefined();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
    ErrorHandler(err, info);
  }
}

class SignAsync : public AsyncWorker
{
public:
  SignAsync(Function& cb, Signer* self, string data, string output)
    : AsyncWorker(cb)
    , self(self)
    , signature(std::move(data))
    , output(std::move(output))
  {
  }
  ~SignAsync()
  {
    HandleScope scope(Env());
    if (self != nullptr) {
      self = nullptr;
    }
    delete buffer;
  }

private:
  Signer* self;
  string signature;
  string output;
  PdfRefCountedBuffer* buffer;

  // AsyncWorker interface
protected:
  void Execute() override
  {
    try {

      string output;
      pdf_long sigStrLength = static_cast<pdf_long>(signature.size());

      PdfSignatureField* pSignField = self->field->GetField();
      pSignField->SetFieldName("signer.sign");
      auto document = self->doc->GetDocument();

      //    PdfRefCountedBuffer r;
      PdfOutputDevice outputDevice;
      if (!output.empty()) {
        outputDevice = *new PdfOutputDevice(output.c_str(), true);
      } else {
        outputDevice = *new PdfOutputDevice(buffer);
      }
      PdfSignOutputDevice signer(&outputDevice);
      signer.SetSignatureSize(static_cast<size_t>(sigStrLength));

      pSignField->SetSignatureDate(PdfDate());
      pSignField->SetSignature(*signer.GetSignatureBeacon());
      document->WriteUpdate(&signer, true);

      if (!signer.HasSignaturePosition())
        throw Error::New(Env(),
                         "Cannot find signature position in the document data");

      signer.AdjustByteRange();
      signer.Seek(0);

      PdfData data(signature.c_str(), static_cast<size_t>(sigStrLength));
      signer.SetSignature(data);
      signer.Flush();
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Error& err) {
      SetError(ErrorHandler::WriteMsg(err));
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    if (!output.empty()) {
      if (FILE* file = fopen(output.c_str(), "r")) {
        fclose(file);
        Callback().Call({ Env().Undefined() });
      } else {
        stringstream msg;
        msg << "Failed to write to: " << output << ", file exists check failed"
            << endl;
        Callback().Call({ String::New(Env(), msg.str()) });
      }
    } else if (buffer != nullptr && buffer->GetSize() > 0) {
      Callback().Call(
        { Env().Undefined(),
          Buffer<char>::Copy(Env(), buffer->GetBuffer(), buffer->GetSize()) });
    }
  }
};

Value
Signer::SignWorker(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_function, napi_string });
  string data, output;
  Function cb = info[0].As<Function>();
  data = info[1].As<String>().Utf8Value();
  if (info.Length() == 3 && info[2].IsString() &&
      info[2].As<String>().Utf8Value().size() > 0) {
    output = info[2].As<String>().Utf8Value();
  } else {
    output = string().empty();
  }
  SignAsync* worker = new SignAsync(cb, this, data, output);
  worker->Queue();
  return info.Env().Undefined();
}
}
