/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2019
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
#include "../ValidateArguments.h"
#include "Document.h"
#include "StreamDocument.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::make_unique;
using std::string;

namespace NoPoDoFo {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
FunctionReference Image::Constructor; // NOLINT
#pragma clang diagnostic pop

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
  unsigned char* buffer = nullptr;
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
  if (DbgLog != nullptr)
    DbgLog->debug("Image Cleanup");
  HandleScope scope(Env());
  Doc = nullptr;
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
      InstanceMethod("setChromaKeyMask", &Image::SetImageChromaKeyMask),
      InstanceMethod("setSoftMask", &Image::SetImageSoftMask),
      InstanceMethod("setColorSpace", &Image::SetImageColorSpace),
      InstanceMethod("setICCProfile", &Image::SetImageICCProfile),
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
void
Image::SetImageColorSpace(const Napi::CallbackInfo& info)
{
  if (!info[0].IsNumber()) {
    Error::New(info.Env(), "image color space expects NPDFColorSpace")
      .ThrowAsJavaScriptException();
  }
  auto colorSpace =
    static_cast<EPdfColorSpace>(info[0].As<Number>().Int64Value());
  Self->SetImageColorSpace(colorSpace);
}
void
Image::SetImageICCProfile(const Napi::CallbackInfo& info)
{
  auto opts =
    AssertCallbackInfo(info,
                       { { 0, { option(napi_object) } },
                         { 1, { option(napi_number) } },
                         { 2, { tl::nullopt, option(napi_number) } } });
  auto buf = info[0].As<Buffer<char>>();
  PdfMemoryInputStream input(buf.Data(), buf.Length());
  long colorComponent = info[1].As<Number>().Int64Value();
  EPdfColorSpace alt = ePdfColorSpace_DeviceRGB;
  if (opts[2] == 1) {
    alt = static_cast<EPdfColorSpace>(info[2].As<Number>().Int64Value());
  }
  Self->SetImageICCProfile(&input, colorComponent, alt);
}
void
Image::SetImageSoftMask(const Napi::CallbackInfo& info)
{
  if (info[0].As<Object>().InstanceOf(Image::Constructor.Value())) {
    auto img = Image::Unwrap(info[0].As<Object>())->GetImage();
    Self->SetImageSoftmask(&img);
  } else {
    Error::New(info.Env(), "soft mask expects an image as the argument value")
      .ThrowAsJavaScriptException();
  }
  Self->SetImageSoftmask(nullptr);
}
void
Image::SetImageChromaKeyMask(const Napi::CallbackInfo& info)
{
  AssertCallbackInfo(info,
                     {
                       { 0, { option(napi_number) } },
                       { 1, { option(napi_number) } },
                       { 2, { option(napi_number) } },
                       { 3, { option(napi_number) } },
                     });

  pdf_int64 r = 0, g = 0, b = 0, threshold = 0;
  r = info[0].As<Number>().Int64Value();
  g = info[1].As<Number>().Int64Value();
  b = info[2].As<Number>().Int64Value();
  threshold = info[3].As<Number>().Int64Value();
  Self->SetImageChromaKeyMask(r, g, b, threshold);
}
}
