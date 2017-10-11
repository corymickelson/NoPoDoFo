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
  static void Initialize(Napi::Env& env, Napi::Object& target);

private:
};

#endif
