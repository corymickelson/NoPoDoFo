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

#ifndef NPDF_VECTOR_H
#define NPDF_VECTOR_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo{
class Vector : public Napi::ObjectWrap<Vector> {
public:
  explicit Vector(const Napi::CallbackInfo &info);
  ~Vector();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env &env, Napi::Object &target);
  Napi::Value GetParentDocument(const Napi::CallbackInfo &info);
  void SetParentDocument(const Napi::CallbackInfo &info, const Napi::Value &value);
  void SetAutoDelete(const Napi::CallbackInfo &info);
  void SetCanReuseObjectNumber(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value GetCanReuseObjectNumber(const Napi::CallbackInfo &info);
  void Clear(const Napi::CallbackInfo &info);
  Napi::Value GetSize(const Napi::CallbackInfo &info);
  Napi::Value GetObjectCount(const Napi::CallbackInfo &info);
  Napi::Value GetObject(const Napi::CallbackInfo &info);
  Napi::Value GetIndex(const Napi::CallbackInfo &info);
  Napi::Value CreateObject(const Napi::CallbackInfo &info);
  Napi::Value RemoveObject(const Napi::CallbackInfo &info);
  Napi::Value CollectGarbage(const Napi::CallbackInfo &info);

private:
  PoDoFo::PdfDocument *document;
  PoDoFo::PdfVecObjects *vector;
};
}
#endif //NPDF_VECTOR_H
