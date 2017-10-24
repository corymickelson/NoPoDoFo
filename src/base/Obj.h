//
// Created by red on 9/23/17
//

#ifndef NPDF_OBJ_H
#define NPDF_OBJ_H

#include <napi.h>
#include <podofo/podofo.h>

#include <utility>

using namespace std;

class Obj : public Napi::ObjectWrap<Obj>
{
public:
  explicit Obj(const Napi::CallbackInfo&);
  ~Obj() { delete obj; }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetStream(const Napi::CallbackInfo&);
  Napi::Value HasStream(const Napi::CallbackInfo&);
  Napi::Value GetObjectLength(const Napi::CallbackInfo&);
  Napi::Value GetDataType(const Napi::CallbackInfo&);
  Napi::Value GetByteOffset(const Napi::CallbackInfo&);
  Napi::Value Write(const Napi::CallbackInfo&);
  Napi::Value Reference(const Napi::CallbackInfo&);
  void FlateCompressStream(const Napi::CallbackInfo&);
  void DelayedStreamLoad(const Napi::CallbackInfo&);
  Napi::Value GetNumber(const Napi::CallbackInfo&);
  Napi::Value GetReal(const Napi::CallbackInfo&);
  Napi::Value GetString(const Napi::CallbackInfo&);
  Napi::Value GetName(const Napi::CallbackInfo&);
  Napi::Value GetArray(const Napi::CallbackInfo&);
  Napi::Value GetBool(const Napi::CallbackInfo&);
  Napi::Value GetReference(const Napi::CallbackInfo&);
  Napi::Value GetRawData(const Napi::CallbackInfo&);

  const PoDoFo::PdfObject GetObject() { return *obj; }

private:
  //  PoDoFo::PdfObject obj = *new PoDoFo::PdfObject();
  PoDoFo::PdfObject* obj;
};

class ObjWritePromise : public Napi::AsyncResolver
{
public:
  ObjWritePromise(Obj* obj, string dest)
    : AsyncResolver(Env())
    , obj(obj)
    , arg(std::move(dest))
  {}
  ~ObjWritePromise() { delete obj; }

private:
  Obj* obj;
  string arg;
  string value;

  // AsyncResolver interface
protected:
  void Execute();
  Napi::Value GetValue() { return Napi::String::New(Env(), value); }
};

class ObjWriteAsync : public Napi::AsyncWorker
{
public:
  ObjWriteAsync(Napi::Function& cb, Obj* obj, string dest)
    : AsyncWorker(cb)
    , obj(obj)
    , arg(std::move(dest))
  {}
  ~ObjWriteAsync() { delete obj; }

protected:
  void Execute();
  void OnOK();

private:
  Obj* obj;
  string arg;
  const char* eMessage = nullptr;
};

class ObjOffsetAsync : public Napi::AsyncWorker
{
public:
  ObjOffsetAsync(Napi::Function& cb, Obj* obj, string arg)
    : Napi::AsyncWorker(cb)
    , obj(obj)
    , arg(std::move(arg))
  {}
  ~ObjOffsetAsync() { delete obj; }

protected:
  void Execute() override;
  void OnOK() override;

private:
  Obj* obj;
  string arg;
  long value = -1;
  const char* eMessage = nullptr;
};

#endif
