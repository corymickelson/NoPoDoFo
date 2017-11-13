//
// Created by red on 11/11/17.
//

#ifndef NPDF_ENCODING_H
#define NPDF_ENCODING_H

#include <napi.h>
#include <podofo/podofo.h>

class Encoding: public Napi::ObjectWrap<Encoding> {
public:
  explicit Encoding(const Napi::CallbackInfo &callbackInfo);
  ~Encoding() {
    PoDoFo::podofo_free(encoding);
    encoding = nullptr;
  }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env &, Napi::Object &target);
  Napi::Value AddToDictionary(const Napi::CallbackInfo &);
  Napi::Value ConvertToUnicode(const Napi::CallbackInfo &);
  Napi::Value ConvertToEncoding(const Napi::CallbackInfo &);
  Napi::Value GetData(const Napi::CallbackInfo &);

private:
  PoDoFo::PdfEncoding *encoding;
};

#endif //NPDF_ENCODING_H
