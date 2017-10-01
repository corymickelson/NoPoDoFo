//
// Created by red on 9/11/17.
//

#ifndef NPDF_IMAGE_H
#define NPDF_IMAGE_H

#include <boost/filesystem.hpp>
#include <napi.h>
#include <podofo/podofo.h>
#include "Document.h"

using namespace Napi;
using namespace boost;
using namespace PoDoFo;


class Image : public ObjectWrap<Image>
{
public:
  Image(const CallbackInfo&);
  ~Image()
  {
    delete _doc;
    delete img;
  }
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "Image",
                  { InstanceMethod("getWidth", &Image::GetWidth),
                    InstanceMethod("getHeight", &Image::GetHeight),
                    InstanceMethod("loadFromFile", &Image::LoadFromFile),
                    InstanceAccessor("isLoaded", &Image::IsLoaded, nullptr),
                    InstanceMethod("setInterpolate", &Image::SetInterpolate),
                    InstanceMethod("setData", &Image::LoadFromBuffer) });
    target.Set("Image", ctor);
  }

  Napi::Value GetWidth(const CallbackInfo&);
  Napi::Value GetHeight(const CallbackInfo&);
  void LoadFromFile(const CallbackInfo &);
  void LoadFromBuffer(const CallbackInfo &);
  Napi::Value IsLoaded(const CallbackInfo&);
  void SetInterpolate(const CallbackInfo &);
  PdfImage GetImage() { return *img; }

private:
  Document* _doc;
  PdfImage* img;
  bool loaded = false;
};

#endif // NPDF_IMAGE_H
