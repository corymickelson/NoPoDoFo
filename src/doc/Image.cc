/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Image.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Document.h"
#include "StreamDocument.h"

using namespace Napi;
using namespace PoDoFo;

using std::make_unique;
using std::string;

namespace NoPoDoFo {

FunctionReference Image::constructor; // NOLINT

Image::Image(const CallbackInfo& info)
  : ObjectWrap(info)
{

#if defined(PODOFO_HAVE_JPEG_LIB) && defined(PODOFO_HAVE_PNG_LIB) &&           \
  defined(PODOFO_HAVE_TIFF_LIB)
  if (info.Length() < 2 || !info[0].IsObject() ||
      (!info[1].IsString() || !info[1].IsBuffer())) {
    Error::New(info.Env(),
               "Image requires the document and image source [file or buffer].")
      .ThrowAsJavaScriptException();
    return;
  }
  auto iObj = info[0].As<Object>();
  if (iObj.InstanceOf(Document::constructor.Value())) {
    doc = Document::Unwrap(iObj)->GetBaseDocument();
  } else if (iObj.InstanceOf(StreamDocument::constructor.Value())) {
    doc = StreamDocument::Unwrap(iObj)->GetBaseDocument();
  }
  int format = 0;
  if (info.Length() >= 3 && info[2].IsString()) {
    format = info[2].As<Number>();
  }
  img = make_unique<PdfImage>(doc.get());
  string file;
  unsigned char* buffer;
  size_t bufLen = 0;
  if (info[1].IsString()) {
    file = info[1].As<String>().Utf8Value();
    //    img->LoadFromFile(file.c_str());
  } else if (info[1].IsBuffer()) {
    buffer = info[1].As<Buffer<unsigned char>>().Data();
    size_t len = info[1].As<Buffer<unsigned char>>().Length();
    //    img->LoadFromData(file, len);
  }
  // data = 0, png =1, tiff =2, jpeg=3
  switch (format) {
    case 0:
      if (bufLen > 0)
        img->LoadFromData(buffer, bufLen);
      else
        img->LoadFromFile(file.c_str());
      break;
    case 1:
      if (bufLen > 0)
        img->LoadFromPngData(buffer, bufLen);
      else
        img->LoadFromPng(file.c_str());
      break;
    case 2:
      if (bufLen > 0)
        img->LoadFromTiffData(buffer, bufLen);
      else
        img->LoadFromTiff(file.c_str());
      break;
    case 3:
      if (bufLen > 0)
        img->LoadFromJpegData(buffer, bufLen);
      else
        img->LoadFromJpeg(file.c_str());
      break;
    default:
      break;
  }

#else
  throw Napi::Error::New(
    info.Env(), "Please rebuild PoDoFo with libpng libjpeg and libtiff.");
#endif
}
void
Image::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Image",
    {
      InstanceAccessor("width", &Image::GetWidth, nullptr),
      InstanceAccessor("height", &Image::GetHeight, nullptr),
      //                  InstanceMethod("loadFromFile", &Image::LoadFromFile),
      //                  InstanceMethod("setData", &Image::LoadFromBuffer),
      InstanceMethod("setInterpolate", &Image::SetInterpolate),
    });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("Image", ctor);
}
//void
//Image::LoadFromFile(const CallbackInfo& info)
//{
//
//#ifdef PODOFO_HAVE_JPEG_LIB
//  try {
//    if (info[0].IsString()) {
//      string file = info[0].As<String>().Utf8Value();
//      img->LoadFromFile(file.c_str());
//    } else {
//      throw Napi::Error::New(
//        info.Env(), "LoadFromFile takes a single argument of type string.");
//    }
//  } catch (PdfError& err) {
//    stringstream msg;
//    msg << "PoDoFo fail code: " << err.GetError() << endl;
//    throw Napi::Error::New(info.Env(), msg.str());
//  }
//#else
//  throw Napi::Error::New(info.Env(),
//                         "NPdf PoDoFo requires libjpeg for images.");
//#endif
//}
//
//void
//Image::LoadFromBuffer(const CallbackInfo& info)
//{
//#ifdef PODOFO_HAVE_JPEG_LIB
//  try {
//    if (info.Length() < 1 || !info[0].IsBuffer()) {
//      throw Napi::Error::New(
//        info.Env(), "LoadFromBuffer requires a single argument of type Buffer");
//    }
//    string jsValue = info[0].As<String>().Utf8Value();
//    auto* value = new unsigned char[jsValue.length()];
//    strcpy(reinterpret_cast<char*>(value), jsValue.c_str());
//    img->LoadFromData(value, static_cast<long>(jsValue.length()));
//  } catch (PdfError& err) {
//    stringstream msg;
//    msg << "PoDoFo error. Code= " << err.GetError() << endl;
//    throw Napi::Error::New(info.Env(), msg.str());
//  } catch (Error& err) {
//    stringstream msg;
//    msg << "JS error: " << err.Message() << endl;
//    throw Napi::Error::New(info.Env(), msg.str());
//  }
//#else
//  throw Napi::Error::New(info.Env(),
//                         "NPdf PoDoFo requires libjpeg for images.");
//#endif
//}

Napi::Value
Image::GetHeight(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), img->GetHeight());
}

Napi::Value
Image::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), img->GetWidth());
}

void
Image::SetInterpolate(const CallbackInfo& info)
{
  img->SetInterpolate(info[0].As<Boolean>());
}
}
