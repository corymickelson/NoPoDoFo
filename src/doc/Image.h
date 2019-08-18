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

#ifndef NPDF_IMAGE_H
#define NPDF_IMAGE_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Image : public Napi::ObjectWrap<Image>
{
public:
  explicit Image(const Napi::CallbackInfo&);
  explicit Image(const Image&) = delete;
  const Image& operator=(const Image&) = delete;
  ~Image();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetWidth(const Napi::CallbackInfo&);
  JsValue GetHeight(const Napi::CallbackInfo&);
  void SetInterpolate(const Napi::CallbackInfo&);
  void SetImageColorSpace(const Napi::CallbackInfo& info);
  void SetImageICCProfile(const Napi::CallbackInfo& info);
  void SetImageSoftMask(const Napi::CallbackInfo& info);
  void SetImageChromaKeyMask(const Napi::CallbackInfo&);
  PoDoFo::PdfImage GetImage() const { return *Self; }

private:
  std::unique_ptr<PoDoFo::PdfImage> Self;
  PoDoFo::PdfDocument* Doc;
  std::shared_ptr<spdlog::logger> Log;
};
}
#endif // NPDF_IMAGE_H
