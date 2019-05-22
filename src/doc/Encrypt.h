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

#ifndef NPDF_ENCRYPT_H
#define NPDF_ENCRYPT_H

#include <napi.h>
#include <podofo/podofo.h>

#include <iostream>
#include <spdlog/logger.h>
using std::cout;
using std::endl;
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Encrypt : public Napi::ObjectWrap<Encrypt>
{
public:
	explicit Encrypt(const Napi::CallbackInfo&);
  explicit Encrypt(const Encrypt&) = delete;
  const Encrypt&operator=(const Encrypt&) = delete;
  ~Encrypt();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  static JsValue CreateEncrypt(const Napi::CallbackInfo&);
  JsValue IsAllowed(const Napi::CallbackInfo&);
  JsValue GetOwnerValue(const Napi::CallbackInfo&);
  JsValue GetUserValue(const Napi::CallbackInfo&);
  JsValue GetProtectionsValue(const Napi::CallbackInfo&);
  JsValue GetEncryptionKey(const Napi::CallbackInfo&);
  JsValue GetKeyLength(const Napi::CallbackInfo&);

  const PoDoFo::PdfEncrypt* Self;
private:
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
