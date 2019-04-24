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

#ifndef NOPODOFO_DATE_H
#define NOPODOFO_DATE_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Date : public Napi::ObjectWrap<Date>
{
public:
  explicit Date(const Napi::CallbackInfo&);
  explicit Date(const Date&) = delete;
  const Date& operator=(const Date&) = delete;
  ~Date();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env&, Napi::Object&);
  JsValue ToString(const Napi::CallbackInfo&);
  JsValue IsValid(const Napi::CallbackInfo&);

  PoDoFo::PdfDate GetDate() const { return *Ts; }

private:
  PoDoFo::PdfDate* Ts;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NOPODOFO_DATE_H
