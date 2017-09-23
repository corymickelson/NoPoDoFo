//
// Created by red on 9/11/17.
//

#include "Document.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace PoDoFo;
using namespace Napi;

class Form : public ObjectWrap<Form>
{
public:
  explicit Form(const CallbackInfo&);
  ~Form() { delete _form; }
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env, "Form", {});
    target.Set("Form", ctor);
  }

private:
  PdfAcroForm* _form;
};
