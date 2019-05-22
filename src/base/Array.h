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

#ifndef NPDF_ARR_H
#define NPDF_ARR_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using std::vector;
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Array : public Napi::ObjectWrap<Array>
{
public:
  explicit Array(const Napi::CallbackInfo&);
  explicit Array(const Array&) = delete;
  const Array& operator=(const Array&) = delete;
  ~Array();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void Write(const Napi::CallbackInfo&);
  JsValue Length(const Napi::CallbackInfo&);
  JsValue ContainsString(const Napi::CallbackInfo&);
  JsValue GetStringIndex(const Napi::CallbackInfo&);
  JsValue IsDirty(const Napi::CallbackInfo&);
  JsValue At(const Napi::CallbackInfo&);
  void SetDirty(const Napi::CallbackInfo&, const Napi::Value&);
  JsValue GetImmutable(const Napi::CallbackInfo&);
  void SetImmutable(const Napi::CallbackInfo&, const Napi::Value&);
  void Push(const Napi::CallbackInfo&);
  JsValue Pop(const Napi::CallbackInfo&);
  JsValue Splice(const Napi::CallbackInfo&);
  void Clear(const Napi::CallbackInfo&);
  JsValue ToJsArray(const Napi::CallbackInfo&);
  PoDoFo::PdfArray& GetArray() const { return Init ? *Init : Self; }

private:
  vector<PoDoFo::PdfObject*> Children;
  Napi::Value GetObjAtIndex(const Napi::CallbackInfo&);
  PoDoFo::PdfArray& Self;
  PoDoFo::PdfArray* Init = nullptr;
  PoDoFo::PdfObject* Parent = nullptr;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
