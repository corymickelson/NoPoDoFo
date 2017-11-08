#ifndef NPDF_SIGNER_H
#define NPDF_SIGNER_H

#include "Document.h"
#include "SignatureField.h"
#include <napi.h>
#include <podofo/podofo.h>

class Signer : public Napi::ObjectWrap<Signer>
{
public:
  Signer(const Napi::CallbackInfo&);
  ~Signer()
  {
    delete doc;
    delete signer;
    delete buffer;
    delete field;
  }
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value Sign(const Napi::CallbackInfo&);

  Document* doc;
  SignatureField* field;
  PoDoFo::PdfSignOutputDevice* signer;
  PoDoFo::PdfRefCountedBuffer* buffer;

private:
};

#endif
