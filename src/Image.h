//
// Created by red on 9/11/17.
//

#ifndef NPDF_IMAGE_H
#define NPDF_IMAGE_H

#include "Document.h"
#include <boost/filesystem.hpp>
#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace boost;
using namespace PoDoFo;

struct NPdfImage
{
  string originFile;
  PdfImage source;
};

class Image : public ObjectWrap<Image>
{
public:
  Image(const CallbackInfo&);
  ~Image()
  {
    delete _doc;
    free(img);
  }
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor =
      DefineClass(env,
                  "Image",
                  { InstanceMethod("getWidth", &Image::GetWidth),
                    InstanceMethod("getHeight", &Image::GetHeight),
                    InstanceMethod("setFile", &Image::SetFile),
                    InstanceMethod("setData", &Image::SetData) });
    target.Set("Image", ctor);
  }

  Napi::Value GetWidth(const CallbackInfo&);
  Napi::Value GetHeight(const CallbackInfo&);
  void SetFile(const CallbackInfo&);
  void SetData(const CallbackInfo&);
  PdfImage GetImage() { return *img; }

private:
  Document* _doc;
  PdfImage* img;
  bool loaded = false;
};

#endif // NPDF_IMAGE_H
