//
// Created by red on 9/23/17.
//

#include "ErrorHandler.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>

using namespace Napi;
using namespace PoDoFo;
using namespace std;

class Dictionary : public ObjectWrap<Dictionary>
{
public:
  explicit Dictionary(const CallbackInfo&);
  ~Dictionary() {}
  static FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(
      env,
      "Dictionary",
      { InstanceMethod("getKey", &Dictionary::GetKey),
        InstanceMethod("getKeys", &Dictionary::GetKeys),
        InstanceMethod("hasKey", &Dictionary::HasKey),
        InstanceMethod("addKey", &Dictionary::AddKey),
        InstanceMethod("removeKey", &Dictionary::RemoveKey),
        InstanceAccessor(
          "immutable", &Dictionary::GetImmutable, &Dictionary::SetImmutable),
        InstanceMethod("clear", &Dictionary::Clear) });

    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Dictionary", constructor);
    //    target.Set("Dictionary", ctor);
  }

  void AddKey(const CallbackInfo&);
  Napi::Value GetKey(const CallbackInfo&);
  Napi::Value GetKeys(const CallbackInfo&);
  Napi::Value RemoveKey(const CallbackInfo&);
  Napi::Value HasKey(const CallbackInfo&);
  Napi::Value Clear(const CallbackInfo&);
  void SetImmutable(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetImmutable(const CallbackInfo&);

private:
  //    PdfDictionary* _dict;
  const PdfObject* _obj;
  PdfDictionary dict = PdfDictionary();
  //  PdfDictionary _dict = *new PdfDictionary();
};
