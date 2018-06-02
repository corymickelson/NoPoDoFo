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

#ifndef NPDF_ENCRYPT_H
#define NPDF_ENCRYPT_H

#include <napi.h>
#include <podofo/podofo.h>

#include <iostream>
using std::cout;
using std::endl;

namespace NoPoDoFo {
class Encrypt : public Napi::ObjectWrap<Encrypt>
{
public:
  explicit Encrypt(const Napi::CallbackInfo&);
  ~Encrypt() { cout << "Destructing Encrypt" << endl; }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  static Napi::Value CreateEncrypt(const Napi::CallbackInfo&);
  Napi::Value IsAllowed(const Napi::CallbackInfo&);
  Napi::Value GetOwnerValue(const Napi::CallbackInfo&);
  Napi::Value GetUserValue(const Napi::CallbackInfo&);
  Napi::Value GetProtectionsValue(const Napi::CallbackInfo&);
  Napi::Value GetEncryptionKey(const Napi::CallbackInfo&);
  Napi::Value GetKeyLength(const Napi::CallbackInfo&);

  const PoDoFo::PdfEncrypt* encrypt;
};
}
#endif
