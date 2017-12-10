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
  void SetField(const Napi::CallbackInfo&);
  Napi::Value GetField(const Napi::CallbackInfo&);
  Napi::Value Sign(const Napi::CallbackInfo&);
  Napi::Value SignWorker(const Napi::CallbackInfo&);

  Document* doc;
  SignatureField* field;

private:
  bool hasSigned = false;
};
}
#endif
