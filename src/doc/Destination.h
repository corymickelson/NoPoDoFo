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

#ifndef NPDF_DESTINATION_H
#define NPDF_DESTINATION_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Destination : public Napi::ObjectWrap<Destination>
{
public:
  static Napi::FunctionReference Constructor;
  explicit Destination(const Napi::CallbackInfo& info);
	explicit Destination(const Destination& ) = delete;
	const Destination&operator=(const Destination&) = delete;
  ~Destination();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetType(const Napi::CallbackInfo&);
  JsValue GetZoom(const Napi::CallbackInfo&);
  JsValue GetRect(const Napi::CallbackInfo&);
  JsValue GetTop(const Napi::CallbackInfo&);
  JsValue GetLeft(const Napi::CallbackInfo&);
  JsValue GetDValue(const Napi::CallbackInfo&);
  JsValue GetObject(const Napi::CallbackInfo&);
  void AddToDictionary(const Napi::CallbackInfo&);
  PoDoFo::PdfDestination& GetDestination() const { return *Self; }
  PoDoFo::PdfDestination* Self;
private:
  std::shared_ptr<spdlog::logger> Log;
};

}
#endif // NPDF_DESTINATION_H
