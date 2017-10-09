//
// Created by red on 9/23/17
//

#include <napi.h>
#include <podofo/podofo.h>

using namespace std;

class Obj : public Napi::ObjectWrap<Obj>
{
public:
  Obj(const Napi::CallbackInfo&);
  ~Obj() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetStream(const Napi::CallbackInfo&);
  Napi::Value HasStream(const Napi::CallbackInfo&);
  Napi::Value GetObjectLength(const Napi::CallbackInfo&);
  Napi::Value GetDataType(const Napi::CallbackInfo&);
  void GetByteOffset(const Napi::CallbackInfo&);
  Napi::Value GetOwner(const Napi::CallbackInfo&);
  void SetOwner(const Napi::CallbackInfo&, const Napi::Value&);
  void WriteObject(const Napi::CallbackInfo&);
  void Write(const Napi::CallbackInfo&);
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
  ObjWriteAsync(Napi::Function& cb, Obj* obj, string outptut);
  ~ObjWriteAsync() {}

protected:
  void Execute();
  void OnOK();

private:
  Obj* obj;
  string output;
  const char* eMessage = nullptr;
};

class ObjOffsetAsync : public Napi::AsyncWorker
{};

class ObjAsTypeAsync : public Napi::AsyncWorker
{};
