//
// Created by red on 9/11/17.
//

#include "Image.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

using namespace Napi;
using namespace PoDoFo;

FunctionReference Image::constructor;

Image::Image(const CallbackInfo& info)
  : ObjectWrap(info)
{

#ifdef PODOFO_HAVE_JPEG_LIB
  try {
    if (info.Length() < 1) {
      throw Napi::Error::New(info.Env(), "Image requires the document.");
    }
    auto docObj = info[0].As<Object>();
    _doc = Document::Unwrap(docObj);
    PdfMemDocument* doc = _doc->GetDocument();
    img = new PdfImage(doc);
    if (info.Length() == 2 && info[1].IsString()) {
      string imgFile = info[1].As<String>().Utf8Value();
      img->LoadFromFile(imgFile.c_str());
      loaded = true;
    }
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
#else
  throw Napi::Error::New(info.Env(),
                         "NPdf PoDoFo requires libjpeg for images.");
#endif
}
void
Image::Initialize(Napi::Env& env, Napi::Object& target)
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
void
Image::LoadFromFile(const CallbackInfo& info)
{

#ifdef PODOFO_HAVE_JPEG_LIB
  try {
    if (info[0].IsString()) {
      string file = info[0].As<String>().Utf8Value();
      img->LoadFromFile(file.c_str());
      loaded = true;
    } else {
      throw Napi::Error::New(
        info.Env(), "LoadFromFile takes a single argument of type string.");
    }
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo fail code: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
#else
  throw Napi::Error::New(info.Env(),
                         "NPdf PoDoFo requires libjpeg for images.");
#endif
}

void
Image::LoadFromBuffer(const CallbackInfo& info)
#ifdef PODOFO_HAVE_JPEG_LIB
{
  try {
    if (info.Length() < 1 || !info[0].IsBuffer()) {
      throw Napi::Error::New(
        info.Env(), "LoadFromBuffer requires a single argument of type Buffer");
    }
    string jsValue = info[0].As<String>().Utf8Value();
    auto* value = new unsigned char[jsValue.length()];
    strcpy(reinterpret_cast<char*>(value), jsValue.c_str());
    img->LoadFromData(value, static_cast<long>(jsValue.length()));
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo error. Code= " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  } catch (Error& err) {
    stringstream msg;
    msg << "JS error: " << err.Message() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
#else
  throw Napi::Error::New(info.Env(),
                         "NPdf PoDoFo requires libjpeg for images.");
#endif
}

Napi::Value
Image::GetHeight(const CallbackInfo& info)
{
  try {
    if (!loaded) {
      throw Napi::Error::New(info.Env(),
                             "Can not call getWidth before setting file/data.");
    }
    return Napi::Number::New(info.Env(), img->GetHeight());
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo failure: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

Napi::Value
Image::GetWidth(const CallbackInfo& info)
{
  try {
    if (!loaded) {
      throw Napi::Error::New(info.Env(),
                             "Can not call getWidth before setting file/data.");
    }
    return Napi::Number::New(info.Env(), img->GetWidth());
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo failure: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

Napi::Value
Image::IsLoaded(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), loaded);
}

void
Image::SetInterpolate(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_boolean });
  img->SetInterpolate(info[0].As<Boolean>());
}
