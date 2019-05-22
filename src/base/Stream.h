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

#ifndef NPDF_STREAM_H
#define NPDF_STREAM_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {
class Stream : public Napi::ObjectWrap<Stream>
{
public:
  explicit Stream(const Napi::CallbackInfo& callbackInfo);
  explicit Stream(const Stream&) = delete;
  const Stream& operator=(const Stream&) = delete;
  ~Stream();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue Write(const Napi::CallbackInfo&);
  void Set(const Napi::CallbackInfo&);
  void BeginAppend(const Napi::CallbackInfo&);
  void Append(const Napi::CallbackInfo&);
  void EndAppend(const Napi::CallbackInfo&);
  JsValue IsAppending(const Napi::CallbackInfo&);
  JsValue Length(const Napi::CallbackInfo&);
  JsValue GetCopy(const Napi::CallbackInfo&);
  PoDoFo::PdfStream& GetStream() const { return  Self; }

private:
  PoDoFo::PdfStream& Self;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_STREAM_H
