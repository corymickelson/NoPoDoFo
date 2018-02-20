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

#ifndef NPDF_REF_H
#define NPDF_REF_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Ref : public Napi::ObjectWrap<Ref>
{
public:
  explicit Ref(const Napi::CallbackInfo&);
  ~Ref();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void SetGenerationNumber(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetGenerationNumber(const Napi::CallbackInfo&);
  void SetObjectNumber(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetObjectNumber(const Napi::CallbackInfo&);

  Napi::Value ToString(const Napi::CallbackInfo&);
  void Write(const Napi::CallbackInfo&);
  Napi::Value IsIndirect(const Napi::CallbackInfo&);

  Napi::Value GetObj(const Napi::CallbackInfo&);
  PoDoFo::PdfReference GetRef() { return *ref; }

private:
  PoDoFo::PdfReference* ref;
};
}
#endif
