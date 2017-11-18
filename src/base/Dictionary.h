//
// Created by red on 9/23/17.
//
#ifndef NPDF_DICTIONARY_H
#define NPDF_DICTIONARY_H

#include <napi.h>
#include <podofo/podofo.h>
#include <string>

using namespace std;

class Dictionary : public Napi::ObjectWrap<Dictionary>
{
public:
  explicit Dictionary(const Napi::CallbackInfo&);
  ~Dictionary() { delete _obj; }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void AddKey(const Napi::CallbackInfo&);
  Napi::Value GetKey(const Napi::CallbackInfo&);
  Napi::Value GetKeys(const Napi::CallbackInfo&);
  Napi::Value RemoveKey(const Napi::CallbackInfo&);
  Napi::Value HasKey(const Napi::CallbackInfo&);
  Napi::Value Clear(const Napi::CallbackInfo&);
  void SetImmutable(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetImmutable(const Napi::CallbackInfo&);
  void SetDirty(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetDirty(const Napi::CallbackInfo&);
  Napi::Value GetKeyAs(const Napi::CallbackInfo&);
  Napi::Value Write(const Napi::CallbackInfo&);
  void WriteSync(const Napi::CallbackInfo&);
  Napi::Value ToObject(const Napi::CallbackInfo&);

  PoDoFo::PdfDictionary* GetDictionary() { return &dict; }

private:
  PoDoFo::PdfObject* _obj;
  PoDoFo::PdfDictionary dict = PoDoFo::PdfDictionary();
};

#endif
