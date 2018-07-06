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
#include "Document.h"
#include "SignatureField.h"
#include "StreamDocument.h"

using namespace Napi;
using namespace PoDoFo;

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::unique_ptr;

namespace NoPoDoFo {

FunctionReference Signer::constructor; // NOLINT

/**
 * @note JS new Signer(doc: IDocument, output?: string)
 * @param info
 */
Signer::Signer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , doc(Document::Unwrap(info[0].As<Object>())->GetDocument())
{
  if (info.Length() < 1) {
    Error::New(info.Env(), "Document required to construct Signer")
      .ThrowAsJavaScriptException();
    return;
  }
  if (!info[0].IsObject()) {
    TypeError::New(info.Env(), "Requires Document to construct Signer")
      .ThrowAsJavaScriptException();
    return;
  }
  if (info.Length() >= 2 && info[1].IsString()) {
    output = info[1].As<String>().Utf8Value();
  }
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
  field = SignatureField::Unwrap(info[0].As<Object>())->GetField();
  try {
    field->EnsureSignatureObject();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
Signer::GetField(const CallbackInfo& info)
{
  return SignatureField::constructor.New(
    { External<PdfSignatureField>::New(info.Env(), field.get()) });
}

Napi::Value
Signer::Sign(const CallbackInfo& info)
{
  try {

    string data, output;
    string sigStr = info[0].As<String>().Utf8Value();
    auto sigStrLength = static_cast<pdf_long>(sigStr.size());

    if (field->GetFieldName().GetStringUtf8().empty()) {
      field->SetFieldName("NoPoDoFo::SignatureField");
    }

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

    field->SetSignatureDate(PdfDate());
    field->SetSignature(*signer.GetSignatureBeacon());
    doc.WriteUpdate(&signer, true);

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
  return info.Env().Undefined();
}

class SignAsync : public AsyncWorker
{
public:
  SignAsync(Function& cb, Signer& self, string data)
    : AsyncWorker(cb)
    , self(self)
    , signature(std::move(data))
  {}
  //  ~SignAsync()
  //  {
  //    HandleScope scope(Env());
  //    self = nullptr;
  //    delete buffer;
  //  }

private:
  Signer& self;
  string signature;
  string output;
  PdfRefCountedBuffer buffer;

  // AsyncWorker interface
protected:
  void Execute() override
  {
    try {
      auto sigStrLength = static_cast<pdf_long>(signature.size());
      self.field->SetFieldName("signer.sign");
      PdfOutputDevice outputDevice =
        self.output.empty() ? PdfOutputDevice(&buffer)
                             : PdfOutputDevice(self.output.c_str(), true);
      PdfSignOutputDevice signer(&outputDevice);
      signer.SetSignatureSize(static_cast<size_t>(sigStrLength));

      self.field->SetSignatureDate(PdfDate());
      self.field->SetSignature(*signer.GetSignatureBeacon());
      self.doc.WriteUpdate(&signer, true);

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
    } else {
      Callback().Call(
        { Env().Undefined(),
          Buffer<char>::Copy(Env(), buffer.GetBuffer(), buffer.GetSize()) });
    }
  }
};

/**
 * @note JS sign(signature:string|Buffer, cb: Function)
 * @param info
 * @return
 */
Value
Signer::SignWorker(const CallbackInfo& info)
{
  string data, output;
  Function cb = info[1].As<Function>();
  data = info[0].As<String>().Utf8Value();
  SignAsync* worker = new SignAsync(cb, *this, data);
  worker->Queue();
  return info.Env().Undefined();
}
}
