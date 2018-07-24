//
// Created by cory on 7/23/18.
//

#ifndef NOPODOFO_DATE_H
#define NOPODOFO_DATE_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {

class Date : public Napi::ObjectWrap<Date>
{
public:
  explicit Date(const Napi::CallbackInfo&);
  ~Date();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env&, Napi::Object&);
  Napi::Value ToString(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfDate* timestamp;
};
}
#endif // NOPODOFO_DATE_H
