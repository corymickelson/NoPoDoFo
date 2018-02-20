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

#ifndef NPDF_ARR_H
#define NPDF_ARR_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Array : public Napi::ObjectWrap<Array>
{
public:
  explicit Array(const Napi::CallbackInfo&);
  ~Array();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void Write(const Napi::CallbackInfo&);
  Napi::Value ToArray(const Napi::CallbackInfo&);
  Napi::Value Length(const Napi::CallbackInfo&);
  Napi::Value ContainsString(const Napi::CallbackInfo&);
  Napi::Value GetStringIndex(const Napi::CallbackInfo&);
  Napi::Value IsDirty(const Napi::CallbackInfo&);
  Napi::Value At(const Napi::CallbackInfo&);
  void SetDirty(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetImmutable(const Napi::CallbackInfo&);
  void SetImmutable(const Napi::CallbackInfo&, const Napi::Value&);
  void Push(const Napi::CallbackInfo&);
  Napi::Value Pop(const Napi::CallbackInfo&);
  void Clear(const Napi::CallbackInfo&);
  Napi::Value Eq(const Napi::CallbackInfo&);
  PoDoFo::PdfArray* GetArray() { return array; }

private:
  PoDoFo::PdfArray* array;
  Napi::Value GetObjAtIndex(const Napi::CallbackInfo&);
};
}
#endif
