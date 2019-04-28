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

#ifndef NPDF_ENCODING_H
#define NPDF_ENCODING_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {
class Encoding : public Napi::ObjectWrap<Encoding>
{
public:
  explicit Encoding(const Napi::CallbackInfo& callbackInfo);
	explicit Encoding(const Encoding&) = delete;
	const Encoding&operator=(const Encoding&) = delete;
  ~Encoding();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env&, Napi::Object& target);
  JsValue AddToDictionary(const Napi::CallbackInfo&);
  JsValue ConvertToUnicode(const Napi::CallbackInfo&);
  JsValue ConvertToEncoding(const Napi::CallbackInfo&);

private:
  const PoDoFo::PdfEncoding* Self;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_ENCODING_H
