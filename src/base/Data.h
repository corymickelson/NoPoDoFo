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

#ifndef NPDF_BASEDATA_H
#define NPDF_BASEDATA_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Data : public Napi::ObjectWrap<Data>
{
public:
  explicit Data(const Napi::CallbackInfo&);
  ~Data();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void Write(const Napi::CallbackInfo&);
  Napi::Value Value(const Napi::CallbackInfo&);

  PoDoFo::PdfData* GetData() { return self; }

private:
  PoDoFo::PdfData* self;
};
}
#endif
