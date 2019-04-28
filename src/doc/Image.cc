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
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "Document.h"
#include "StreamDocument.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::make_unique;
using std::string;

namespace NoPoDoFo {

FunctionReference Image::Constructor; // NOLINT

Image::Image(const CallbackInfo& info)
  : ObjectWrap(info)
{
  DbgLog = spdlog::get("DbgLog");
#if defined(PODOFO_HAVE_JPEG_LIB) && defined(PODOFO_HAVE_PNG_LIB) &&           \
  defined(PODOFO_HAVE_TIFF_LIB)
  if (info.Length() < 2 || !info[0].IsObject() ||
      (!info[1].IsString() && !info[1].IsBuffer())) {
    Error::New(info.Env(),
               "Image requires the document and image source [file or buffer].")
      .ThrowAsJavaScriptException();
    return;
  }
  auto iObj = info[0].As<Object>();
  if (iObj.InstanceOf(Document::Constructor.Value())) {
    Doc = Document::Unwrap(iObj)->Base;
  } else if (iObj.InstanceOf(StreamDocument::Constructor.Value())) {
    Doc = StreamDocument::Unwrap(iObj)->Base;
  }
  int format = 0;
  if (info.Length() >= 3 && info[2].IsString()) {
    format = info[2].As<Number>();
  }
  Self = make_unique<PdfImage>(Doc);
  string file;
  unsigned char* buffer;
  size_t bufLen = 0;
  if (info[1].IsString()) {
    file = info[1].As<String>().Utf8Value();
    if (!FileAccess(file)) {
      Error::New(info.Env(), "File not found").ThrowAsJavaScriptException();
      return;
    }
  } else if (info[1].IsBuffer()) {
    buffer = info[1].As<Buffer<unsigned char>>().Data();
    bufLen = info[1].As<Buffer<unsigned char>>().Length();
  }
  // data = 0, png =1, tiff =2, jpeg=3
  switch (format) {
    case 0:
      if (bufLen > 0)
        Self->LoadFromData(buffer, bufLen);
      else
        Self->LoadFromFile(file.c_str());
      break;
    case 1:
      if (bufLen > 0)
        Self->LoadFromPngData(buffer, bufLen);
      else
        Self->LoadFromPng(file.c_str());
      break;
    case 2:
      if (bufLen > 0)
        Self->LoadFromTiffData(buffer, bufLen);
      else
        Self->LoadFromTiff(file.c_str());
      break;
    case 3:
      if (bufLen > 0)
        Self->LoadFromJpegData(buffer, bufLen);
      else
        Self->LoadFromJpeg(file.c_str());
      break;
    default:
      break;
  }

#else
  throw Napi::Error::New(
    info.Env(), "Please rebuild PoDoFo with libpng libjpeg and libtiff.");
#endif
}

Image::~Image()
{
  DbgLog->debug("Image Cleanup");
  HandleScope scope(Env());
  Doc = nullptr;
}
void
Image::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor =
    DefineClass(env,
                "Image",
                {
                  InstanceAccessor("width", &Image::GetWidth, nullptr),
                  InstanceAccessor("height", &Image::GetHeight, nullptr),
                  InstanceMethod("setInterpolate", &Image::SetInterpolate),
                });
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();

  target.Set("Image", ctor);
}

Napi::Value
Image::GetHeight(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), Self->GetHeight());
}

Napi::Value
Image::GetWidth(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), Self->GetWidth());
}

void
Image::SetInterpolate(const CallbackInfo& info)
{
  Self->SetInterpolate(info[0].As<Boolean>());
}
}
