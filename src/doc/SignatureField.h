//
// Created by red on 9/26/17
//

#ifndef NPDF_SIGNATUREFIELD_H
#define NPDF_SIGNATUREFIELD_H

#include <napi.h>
#include <podofo/podofo.h>

class SignatureField : public Napi::ObjectWrap<SignatureField>
{
public:
  explicit SignatureField(const Napi::CallbackInfo&);
  ~SignatureField() { delete field; }
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetAppearanceStream(const Napi::CallbackInfo&);
  void SetSignature(const Napi::CallbackInfo&);
  void SetReason(const Napi::CallbackInfo&);
  void SetLocation(const Napi::CallbackInfo&);
  void SetCreator(const Napi::CallbackInfo&);
  void SetDate(const Napi::CallbackInfo&);
  void AddCertificateReference(const Napi::CallbackInfo&);
  Napi::Value GetSignatureObject(const Napi::CallbackInfo&);
  Napi::Value EnsureSignatureObject(const Napi::CallbackInfo&);

  PoDoFo::PdfSignatureField* GetField() { return field; }

private:
  PoDoFo::PdfSignatureField* field;
};

#endif
