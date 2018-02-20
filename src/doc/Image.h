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
  ~Image();

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
