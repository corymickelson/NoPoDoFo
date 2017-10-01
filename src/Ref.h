//
// Created by red on 10/1/17
//

#include <napi.h>
#include <podofo/podofo.h>

using namespace PoDoFo;

class Ref : public Napi::ObjectWrap<Ref>
{
public:
  explicit Ref(const Napi::CallbackInfo&);
  ~Ref() {}
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    Napi::Function ctor = DefineClass(
      env,
      "Ref",
      { InstanceAccessor(
          "generation", &Ref::GetGenerationNumber, &Ref::SetGenerationNumber),
        InstanceAccessor(
          "object", &Ref::GetObjectNumber, &Ref::SetObjectNumber),
        InstanceMethod("toString", &Ref::ToString),
        InstanceMethod("write", &Ref::Write),
        InstanceMethod("isIndirect", &Ref::IsIndirect) });
    constructor = Napi::Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Ref", constructor);
  }

  void SetGenerationNumber(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetGenerationNumber(const Napi::CallbackInfo&);
  void SetObjectNumber(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetObjectNumber(const Napi::CallbackInfo&);

  Napi::Value ToString(const Napi::CallbackInfo&);
  void Write(const Napi::CallbackInfo&);
  Napi::Value IsIndirect(const Napi::CallbackInfo&);

private:
  PdfReference ref = *new PdfReference();
};
