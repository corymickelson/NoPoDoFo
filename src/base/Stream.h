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

#ifndef NPDF_STREAM_H
#define NPDF_STREAM_H

#include <napi.h>
#include <podofo/podofo.h>
#include <string>

using namespace std;

namespace NoPoDoFo {
class Stream : public Napi::ObjectWrap<Stream>
{
public:
  explicit Stream(const Napi::CallbackInfo& callbackInfo);
  ~Stream();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value Write(const Napi::CallbackInfo&);
  Napi::Value GetBuffer(const Napi::CallbackInfo&);
  Napi::Value GetFilteredBuffer(const Napi::CallbackInfo&);
  PoDoFo::PdfStream* GetStream() { return stream; }

private:
  PoDoFo::PdfStream* stream;
};
}
#endif // NPDF_STREAM_H
