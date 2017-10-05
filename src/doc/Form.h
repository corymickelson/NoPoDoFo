//
// Created by red on 9/11/17.
//

#include <napi.h>
#include <podofo/podofo.h>

class Form : public Napi::ObjectWrap<Form>
{
public:
  explicit Form(const Napi::CallbackInfo&);
  ~Form() { delete doc; }
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetNeedAppearances(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetNeedAppearances(const Napi::CallbackInfo&);
  Napi::Value GetObject(const Napi::CallbackInfo&);
  PoDoFo::PdfAcroForm* GetForm() { return doc->GetAcroForm(); }

private:
  PoDoFo::PdfMemDocument* doc;
};
