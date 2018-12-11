//
// Created by developer on 12/9/18.
//

#ifndef NOPODOFO_REF_H
#define NOPODOFO_REF_H

#include <podofo/podofo.h>
#include <napi.h>

using namespace Napi;

using value = Napi::Value;
using PoDoFo::PdfReference;

namespace NoPoDoFo {
class Ref: public Napi::ObjectWrap<Ref> {
public:
  PdfReference* self;
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env &env, Napi::Object& target);
  explicit Ref(const CallbackInfo &info);
  ~Ref();
  value GetObjectNumber(const CallbackInfo&);
  value GetGenerationNumber(const CallbackInfo&);
};

}


#endif //NOPODOFO_REF_H
