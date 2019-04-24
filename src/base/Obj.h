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

#ifndef NPDF_OBJ_H
#define NPDF_OBJ_H

#include <napi.h>
#include <podofo/podofo.h>
#include <iostream>
#include <spdlog/logger.h>

using std::cout;
using std::endl;
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Obj : public Napi::ObjectWrap<Obj>
{
public:
  explicit Obj(const Napi::CallbackInfo&);
  explicit Obj(const Obj&) = delete;
  const Obj& operator=(const Obj&) = delete;
  ~Obj();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetStream(const Napi::CallbackInfo&);
  JsValue HasStream(const Napi::CallbackInfo&);
  JsValue GetObjectLength(const Napi::CallbackInfo&);
  JsValue GetDataType(const Napi::CallbackInfo&);
  JsValue GetByteOffset(const Napi::CallbackInfo&);
  JsValue Write(const Napi::CallbackInfo&);
  JsValue Reference(const Napi::CallbackInfo&);
  void FlateCompressStream(const Napi::CallbackInfo&);
  void DelayedStreamLoad(const Napi::CallbackInfo&);
  JsValue GetNumber(const Napi::CallbackInfo&);
  JsValue GetReal(const Napi::CallbackInfo&);
  JsValue GetString(const Napi::CallbackInfo&);
  JsValue GetName(const Napi::CallbackInfo&);
  JsValue GetArray(const Napi::CallbackInfo&);
  JsValue GetBool(const Napi::CallbackInfo&);
  JsValue GetDictionary(const Napi::CallbackInfo&);
  JsValue GetRawData(const Napi::CallbackInfo&);
  JsValue GetImmutable(const Napi::CallbackInfo&);
  void SetImmutable(const Napi::CallbackInfo&, const JsValue&);
  void Clear(const Napi::CallbackInfo&);
  JsValue MustGetIndirect(const Napi::CallbackInfo&);
  PoDoFo::PdfObject& GetObject() const { return Init == nullptr ? NObj : *Init; }

private:
  PoDoFo::PdfObject& NObj;
  PoDoFo::PdfObject* Init = nullptr;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
