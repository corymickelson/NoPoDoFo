#include "Signer.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

using namespace Napi;
using namespace PoDoFo;

Signer::Signer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(
    info, 2, { napi_valuetype::napi_object, napi_valuetype::napi_object });
  doc = Document::Unwrap(info[0].As<Object>());
  field = SignatureField::Unwrap(info[1].As<Object>());
  if (!doc->LoadedForIncrementalUpdates()) {
    throw Napi::Error::New(info.Env(),
                           "Please reload Document with forUpdates = true.");
  }
  if (info.Length() == 3 && info[2].IsString()) {
    PdfOutputDevice device(info[2].As<String>().Utf8Value().c_str(), true);
    signer = new PdfSignOutputDevice(&device);
    buffer = nullptr;
  } else {
    buffer = new PdfRefCountedBuffer();
    PdfOutputDevice device(buffer);
    signer = new PdfSignOutputDevice(&device);
  }
}

void
Signer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor =
    DefineClass(env, "Signer", { InstanceMethod("sign", &Signer::Sign) });
  target.Set("Signer", ctor);
}

class SignAsync : public AsyncWorker
{
public:
  SignAsync(Function& cb, Signer* self, Buffer<char>& data)
    : AsyncWorker(cb)
    , self(self)
    , input(data)
  {}

private:
  Signer* self;
  bool outBuffer = false;
  Buffer<char> input;

  // AsyncWorker interface
protected:
  void Execute()
  {
    try {
      self->signer->SetSignatureSize(input.Length());
      PdfData signature(input.Data());
      self->field->GetField()->SetSignature(
        *self->signer->GetSignatureBeacon());
      self->doc->GetDocument()->WriteUpdate(self->signer, true);

      if (!self->signer->HasSignaturePosition())
        PODOFO_RAISE_ERROR_INFO(
          ePdfError_SignatureError,
          "Cannot find signature position in the document data");

      self->signer->AdjustByteRange();
      self->signer->Seek(0);
      self->signer->SetSignature(input.Data());
      self->signer->Flush();
      if (self->buffer)
        outBuffer = true;
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    }
  }
  void OnOK()
  {
    HandleScope scope(Env());
    auto value = Buffer<char>::Copy(
      Env(), self->buffer->GetBuffer(), self->buffer->GetSize());
    Callback().Call({ Env().Null(), value });
  }
};

Value
Signer::Sign(const CallbackInfo& info)
{
  if (info.Length() < 2 || !info[0].IsFunction() || !info[1].IsBuffer()) {
    throw Error::New(info.Env(),
                     "SetSignature requires a single argument of type buffer");
  }
  Buffer<char> sig = info[1].As<Buffer<char>>();
  auto cb = info[0].As<Function>();
  auto worker = new SignAsync(cb, this, sig);
  worker->Queue();
  return info.Env().Undefined();
}
