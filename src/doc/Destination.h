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

#ifndef NPDF_DESTINATION_H
#define NPDF_DESTINATION_H

#include <podofo/podofo.h>
#include <napi.h>

namespace NoPoDoFo {

class Destination: public Napi::ObjectWrap<Destination> {
public:
  static Napi::FunctionReference constructor;
  explicit Destination(const Napi::CallbackInfo &info);
  ~Destination();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetType(const Napi::CallbackInfo&);
  Napi::Value GetZoom(const Napi::CallbackInfo&);
  Napi::Value GetRect(const Napi::CallbackInfo&);
  Napi::Value GetTop(const Napi::CallbackInfo&);
  Napi::Value GetLeft(const Napi::CallbackInfo&);
  Napi::Value GetDValue(const Napi::CallbackInfo&);
  Napi::Value GetObject(const Napi::CallbackInfo&);
  void AddToDictionary(const Napi::CallbackInfo&);
  PoDoFo::PdfDestination GetDestination() { return *destination; }
  PoDoFo::PdfDestination* destination;
};

}
#endif //NPDF_DESTINATION_H
