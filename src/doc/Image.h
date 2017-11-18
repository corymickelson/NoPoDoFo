//
// Created by red on 9/11/17.
//

#ifndef NPDF_IMAGE_H
#define NPDF_IMAGE_H

#include "Document.h"
#include <napi.h>
#include <podofo/podofo.h>
namespace NoPoDoFo {
class Image : public Napi::ObjectWrap<Image>
{
public:
  Image(const Napi::CallbackInfo&);
  ~Image()
  {
    delete _doc;
    delete img;
  }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetWidth(const Napi::CallbackInfo&);
  Napi::Value GetHeight(const Napi::CallbackInfo&);
  void LoadFromFile(const Napi::CallbackInfo&);
  void LoadFromBuffer(const Napi::CallbackInfo&);
  Napi::Value IsLoaded(const Napi::CallbackInfo&);
  void SetInterpolate(const Napi::CallbackInfo&);
  PoDoFo::PdfImage GetImage() { return *img; }

private:
  Document* _doc;
  PoDoFo::PdfImage* img;
  bool loaded = false;
};
}
#endif // NPDF_IMAGE_H
