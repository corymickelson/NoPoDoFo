//
// Created by red on 10/2/17
//

#include <napi.h>
#include <podofo/podofo.h>

class Arr : public Napi::ObjectWrap<Arr>
{
public:
  explicit Arr(const Napi::CallbackInfo&);
  ~Arr() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void Write(const Napi::CallbackInfo&);
  Napi::Value ContainsString(const Napi::CallbackInfo&);
  Napi::Value GetStringIndex(const Napi::CallbackInfo&);
  Napi::Value IsDirty(const Napi::CallbackInfo&);
  void SetDirty(const Napi::CallbackInfo&, const Napi::Value&);

private:
  PoDoFo::PdfArray arr = *new PoDoFo::PdfArray();
};
