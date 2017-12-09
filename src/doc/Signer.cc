#include "Signer.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Data.h"

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
  if (info.Length() == 2 && info[1].IsString()) {
    device =
      new PdfOutputDevice(info[1].As<String>().Utf8Value().c_str(), true);
    signer = new PdfSignOutputDevice(device);
    buffer = nullptr;
  } else {
    buffer = new PdfRefCountedBuffer();
    device = new PdfOutputDevice(buffer);
    signer = new PdfSignOutputDevice(device);
  }
}

Signer::~Signer()
{
  HandleScope scope(Env());
  doc = nullptr;
  delete signer;
  delete buffer;
  field = nullptr;
  delete device;
}

void
Signer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Signer",
    { InstanceMethod("sign", &Signer::Sign),
      InstanceAccessor(
        "signatureSize", &Signer::GetSignatureSize, &Signer::SetSignatureSize),
      InstanceAccessor("length", &Signer::GetLength, nullptr),
      InstanceMethod("setSignature", &Signer::SetSignature),
      InstanceMethod("getBeacon", &Signer::GetSignatureBeacon),
      InstanceMethod("adjustByteRange", &Signer::AdjustByteRange),
      InstanceMethod("write", &Signer::Write),
      InstanceMethod("read", &Signer::Read),
      InstanceMethod("seek", &Signer::Seek),
      InstanceMethod("flush", &Signer::Flush),
      InstanceMethod("hasSignaturePosition", &Signer::HasSignaturePosition) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Signer", ctor);
}

Value
Signer::GetSignatureSize(const CallbackInfo& info)
{
  return Number::New(info.Env(), signer->GetSignatureSize());
}

void
Signer::SetSignatureSize(const CallbackInfo& info, const Napi::Value& value)
{
  signer->SetSignatureSize(value.As<Number>().Uint32Value());
}

void
Signer::SetSignature(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_object });
  auto wrapper = info[0].As<Object>();
  if (!wrapper.InstanceOf(Data::constructor.Value())) {
    throw Napi::TypeError::New(info.Env(),
                               "requires instance of SignatureField");
  }
  auto data = Data::Unwrap(wrapper);
  try {
    signer->SetSignature(*data->GetData());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Signer::Flush(const CallbackInfo& info)
{
  try {
    signer->Flush();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Signer::Seek(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_number });
  size_t i = info[0].As<Number>().Uint32Value();
  try {
    signer->Seek(i);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Signer::Read(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_number });
  const auto len = info[0].As<Number>().Uint32Value();
  const auto buffer = reinterpret_cast<char*>(calloc(len, sizeof(char)));
  Buffer<char> value;
  try {
    signer->Read(buffer, len);
    value = Buffer<char>::Copy(info.Env(), buffer, len);
    free(buffer);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
  return value;
}

void
Signer::Write(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_string });
  string pBuffer = info[0].As<String>().Utf8Value();
  try {
    signer->Write(pBuffer.c_str(), pBuffer.size());
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
Signer::GetLength(const CallbackInfo& info)
{
  return Number::New(info.Env(), signer->GetLength());
}

void
Signer::AdjustByteRange(const CallbackInfo& info)
{
  try {
    signer->AdjustByteRange();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Napi::Value
Signer::GetSignatureBeacon(const CallbackInfo& info)
{
  const PdfData* data = signer->GetSignatureBeacon();
  auto ext = External<PdfData>::New(
    info.Env(), const_cast<PdfData*>(data), [](Napi::Env env, PdfData* data) {
      HandleScope scope(env);
      delete data;
    });
  return Data::constructor.New({ ext });
}

Napi::Value
Signer::HasSignaturePosition(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), signer->HasSignaturePosition());
}

Napi::Value
Signer::Sign(const CallbackInfo& info)
{
  try {

    string sigStr;
    pdf_long sigStrLength = 0;
    Buffer<char> sigBuffer;
    string data;
    if (info[0].IsString()) {
      sigStr = info[0].As<String>().Utf8Value();
      sigStrLength = static_cast<pdf_long>(sigStr.size());
    } else if (info[0].Type() == napi_external) {
      char* ext = *info[0].As<External<char*>>().Data();
      data = string(ext);
      // sigBuffer = info[0].As<Buffer<char>>();
    }
    PdfSignatureField* pSignField = field->GetField();
    PdfPage* page = doc->GetDocument()->GetPage(0);
    PdfRect rect(0.0, 0.0, 0.0, 0.0);
    PdfAnnotation* pAnnot = page->CreateAnnotation(ePdfAnnotation_Widget, rect);

    pSignField = new PdfSignatureField(
      pAnnot, doc->GetDocument()->GetAcroForm(), doc->GetDocument());
    pSignField->SetFieldName("test");
    pSignField->SetSignatureReason(
      PdfString(reinterpret_cast<const pdf_utf8*>("reason")));

    auto document = doc->GetDocument();
    PdfRefCountedBuffer r;
    PdfOutputDevice outputDevice("/tmp/test.pdf", true /*&r*/);
    PdfSignOutputDevice signer(&outputDevice);
    if (sigStrLength == 0) {
      signer.SetSignatureSize(data.size());
    } else {
      signer.SetSignatureSize(static_cast<size_t>(sigStrLength));
    }

    pSignField->SetSignatureDate(PdfDate());
    pSignField->SetSignature(*signer.GetSignatureBeacon());
    document->WriteUpdate(&signer, true);

    if (!signer.HasSignaturePosition())
      throw Error::New(info.Env(),
                       "Cannot find signature position in the document data");

    signer.AdjustByteRange();
    signer.Seek(0);

    PdfData* signature;
    if (sigStrLength == 0) {
      signature = new PdfData(data.c_str(), data.size());
    } else {
      signature =
        new PdfData(sigStr.c_str(), static_cast<size_t>(sigStrLength));
    }
    signer.SetSignature(*signature);

    signer.Flush();
    return info.Env().Undefined();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
    ErrorHandler(err, info);
  }
}
}
