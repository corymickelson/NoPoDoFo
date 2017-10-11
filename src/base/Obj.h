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
  ~Obj() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetStream(const Napi::CallbackInfo&);
  Napi::Value HasStream(const Napi::CallbackInfo&);
  Napi::Value GetObjectLength(const Napi::CallbackInfo&);
  Napi::Value GetDataType(const Napi::CallbackInfo&);
  Napi::Value GetByteOffsetSync(const Napi::CallbackInfo&);
  void GetByteOffset(const Napi::CallbackInfo&);
  Napi::Value GetOwner(const Napi::CallbackInfo&);
  void SetOwner(const Napi::CallbackInfo&, const Napi::Value&);
  void WriteObject(const Napi::CallbackInfo&);
  Napi::Value Write(const Napi::CallbackInfo&);
  Napi::Value Reference(const Napi::CallbackInfo&);
  void FlateCompressStream(const Napi::CallbackInfo&);
  void DelayedStreamLoad(const Napi::CallbackInfo&);
  Napi::Value AsType(const Napi::CallbackInfo&);
  Napi::Value GetInstance(const Napi::CallbackInfo&);

  const PoDoFo::PdfObject GetObject() { return obj; }

private:
  PoDoFo::PdfObject obj = *new PoDoFo::PdfObject();
};

class ObjWriteAsync : public Napi::AsyncWorker
{
public:
  ObjWriteAsync(Napi::Function& cb, Obj* obj, string dest)
    : AsyncWorker(cb)
    , obj(obj)
    , arg(std::move(dest))
  {}
  ~ObjWriteAsync() {}

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
