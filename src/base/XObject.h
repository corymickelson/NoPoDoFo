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

#ifndef NPDF_XOBJECT_H
#define NPDF_XOBJECT_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class XObject : public Napi::ObjectWrap<XObject>
{
public:
  explicit XObject(const Napi::CallbackInfo& info);
  ~XObject();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetContents(const Napi::CallbackInfo&);
  Napi::Value GetContentsForAppending(const Napi::CallbackInfo&);
  Napi::Value GetResources(const Napi::CallbackInfo&);
  Napi::Value GetPageSize(const Napi::CallbackInfo&);
  Napi::Value Reference(const Napi::CallbackInfo&);
  PoDoFo::PdfXObject& GetXObject() { return *xobj; }

private:
  PoDoFo::PdfXObject* xobj;
};
}
#endif // NPDF_XOBJECT_H
