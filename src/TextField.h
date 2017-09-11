//
// Created by red on 9/11/17.
//

#ifndef NPDF_TEXTFIELD_H
#define NPDF_TEXTFIELD_H

#include <napi.h>
#include <string>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;
using namespace std;

class TextField : public Napi::ObjectWrap<TextField>
{
public:
  TextField(const CallbackInfo &);
  ~TextField()
  {
    delete _base;
    delete _field;
  };
  static Napi::FunctionReference constructor;
  static void
  Initialize(Napi::Env &env, Napi::Object &target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env,
                                "TextField",
                                {InstanceMethod("setText", &TextField::SetText),
                                 InstanceMethod("getText", &TextField::GetText)});
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("TextField", constructor);
  }
  void
  SetText(const CallbackInfo &);
  Napi::Value
  GetText(const CallbackInfo &);
private:
  PoDoFo::PdfField *_base;
  PoDoFo::PdfTextField *_field;
};
#endif //NPDF_TEXTFIELD_H
