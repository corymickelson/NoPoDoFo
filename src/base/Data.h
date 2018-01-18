#ifndef NPDF_BASEDATA_H
#define NPDF_BASEDATA_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Data : public Napi::ObjectWrap<Data>
{
public:
  explicit Data(const Napi::CallbackInfo&);
  ~Data();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void Write(const Napi::CallbackInfo&);
  Napi::Value Value(const Napi::CallbackInfo&);

  PoDoFo::PdfData* GetData() { return self; }

private:
  PoDoFo::PdfData* self;
};
}
#endif
