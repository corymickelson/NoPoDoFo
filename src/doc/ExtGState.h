#ifndef EXTGSTATE_H
#define EXTGSTATE_H

#include "Document.h"

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class ExtGState : public Napi::ObjectWrap<ExtGState>
{
public:
  explicit ExtGState(const Napi::CallbackInfo& info);
  ~ExtGState();

  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetFillOpacity(const Napi::CallbackInfo& info);
  void SetStrokeOpacity(const Napi::CallbackInfo& info);
  void SetBlendMode(const Napi::CallbackInfo& info);
  void SetOverprint(const Napi::CallbackInfo& info);
  void SetFillOverprint(const Napi::CallbackInfo& info);
  void SetStrokeOverprint(const Napi::CallbackInfo& info);
  void SetNonZeroOverprint(const Napi::CallbackInfo& info);
  void SetRenderingIntent(const Napi::CallbackInfo& info);
  void SetFrequency(const Napi::CallbackInfo& info);

  PoDoFo::PdfExtGState* GetExtGState() { return self; }

private:
  PoDoFo::PdfExtGState* self;
  //  Document* doc;
};
}
#endif
