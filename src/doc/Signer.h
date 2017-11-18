#ifndef NPDF_SIGNER_H
#define NPDF_SIGNER_H

#include "Document.h"
#include "SignatureField.h"
#include <napi.h>
#include <podofo/podofo.h>

class Signer : public Napi::ObjectWrap<Signer>
{
public:
  static Napi::FunctionReference constructor;
  explicit Signer(const Napi::CallbackInfo&);
  ~Signer();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value Sign(const Napi::CallbackInfo&);
  Napi::Value PoDoFoSign(const Napi::CallbackInfo&);

  Document* doc;
  SignatureField* field;
  PoDoFo::PdfOutputDevice* device;
  PoDoFo::PdfSignOutputDevice* signer;
  PoDoFo::PdfRefCountedBuffer* buffer;

private:
};

#endif
