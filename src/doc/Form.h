//
// Created by red on 9/11/17.
//

#ifndef NPDF_FORM_H
#define NPDF_FORM_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Form : public Napi::ObjectWrap<Form>
{
public:
  explicit Form(const Napi::CallbackInfo&);
  ~Form() { delete doc; }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetNeedAppearances(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetNeedAppearances(const Napi::CallbackInfo&);
  Napi::Value GetObject(const Napi::CallbackInfo&);
  PoDoFo::PdfAcroForm* GetForm() { return doc->GetAcroForm(); }

private:
  PoDoFo::PdfMemDocument* doc;
};
}
#endif
