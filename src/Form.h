//
// Created by red on 9/11/17.
//

//#include "Dictionary.h"
#include "Document.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace PoDoFo;
using namespace Napi;

class Form : public ObjectWrap<Form>
{
public:
  explicit Form(const CallbackInfo&);
  ~Form() { delete doc; }
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "Form",
                  { InstanceMethod("getDictionary", &Form::GetDictionary),
                    InstanceMethod("getObject", &Form::GetObject),
                    InstanceAccessor("needAppearances",
                                     &Form::GetNeedAppearances,
                                     &Form::SetNeedAppearances) });
    target.Set("Form", ctor);
  }
  void SetNeedAppearances(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetNeedAppearances(const CallbackInfo&);
  Napi::Value GetDictionary(const CallbackInfo&);
  Napi::Value GetObject(const CallbackInfo&);
  PdfAcroForm* GetForm() { return doc->GetAcroForm(); }

private:
  PdfMemDocument* doc;
};
