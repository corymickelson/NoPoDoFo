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

#ifndef NPDF_XOBJECT_H
#define NPDF_XOBJECT_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {
class XObject : public Napi::ObjectWrap<XObject>
{
public:
  explicit XObject(const Napi::CallbackInfo& info);
  explicit XObject(const XObject&) = delete;
  const XObject& operator=(const XObject&) = delete;
  ~XObject();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetContents(const Napi::CallbackInfo&);
  JsValue GetContentsForAppending(const Napi::CallbackInfo&);
  JsValue GetResources(const Napi::CallbackInfo&);
  JsValue GetPageSize(const Napi::CallbackInfo&);
  JsValue Reference(const Napi::CallbackInfo&);
  PoDoFo::PdfXObject& GetXObject() const { return *Self; }

private:
  PoDoFo::PdfXObject* Self;
  std::shared_ptr<spdlog::logger> Log;
};
}
#endif // NPDF_XOBJECT_H
