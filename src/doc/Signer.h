#ifndef NPDF_SIGNER_H
#define NPDF_SIGNER_H

#include "Document.h"
#include "SignatureField.h"
#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Signer : public Napi::ObjectWrap<Signer>
{
public:
  static Napi::FunctionReference constructor;
  explicit Signer(const Napi::CallbackInfo&);
  ~Signer();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetSignatureSize(const Napi::CallbackInfo&);
  void SetSignatureSize(const Napi::CallbackInfo&, const Napi::Value&);
  void SetSignature(const Napi::CallbackInfo&);
  void Flush(const Napi::CallbackInfo&);
  void Seek(const Napi::CallbackInfo&);
  Napi::Value Read(const Napi::CallbackInfo&);
  void Write(const Napi::CallbackInfo&);
  Napi::Value GetLength(const Napi::CallbackInfo&);
  void AdjustByteRange(const Napi::CallbackInfo&);
  Napi::Value GetSignatureBeacon(const Napi::CallbackInfo&);
  Napi::Value HasSignaturePosition(const Napi::CallbackInfo&);

  Napi::Value Sign(const Napi::CallbackInfo&);

  Document* doc;
  SignatureField* field;
  PoDoFo::PdfOutputDevice* device;
  PoDoFo::PdfSignOutputDevice* signer;
  PoDoFo::PdfRefCountedBuffer* buffer;

private:
};
}
#endif
