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

#ifndef NPDF_DICTIONARY_H
#define NPDF_DICTIONARY_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;
using std::vector;

namespace NoPoDoFo {
class Dictionary : public Napi::ObjectWrap<Dictionary>
{
public:
  explicit Dictionary(const Napi::CallbackInfo&);
  explicit Dictionary(const Dictionary&) = delete;
  const Dictionary& operator=(const Dictionary&) = delete;

  ~Dictionary();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void AddKey(const Napi::CallbackInfo&);
  JsValue GetKey(const Napi::CallbackInfo&);
  JsValue GetKeyType(const Napi::CallbackInfo&);
  JsValue GetKeys(const Napi::CallbackInfo&);
  JsValue RemoveKey(const Napi::CallbackInfo&);
  JsValue HasKey(const Napi::CallbackInfo&);
  JsValue Clear(const Napi::CallbackInfo&);
  void SetImmutable(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetImmutable(const Napi::CallbackInfo&);
  void SetDirty(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetDirty(const Napi::CallbackInfo&);
  JsValue GetKeyAs(const Napi::CallbackInfo&);
  JsValue Write(const Napi::CallbackInfo&);
  void WriteSync(const Napi::CallbackInfo&);
  JsValue Eq(const Napi::CallbackInfo&);
  PoDoFo::PdfDictionary& GetDictionary() const { return Init ? *Init : Self; }

private:
  vector<PoDoFo::PdfObject*> Children;
  vector<PoDoFo::PdfArray*> ChildArrays;
  PoDoFo::PdfDictionary& Self;
  PoDoFo::PdfDictionary* Init = nullptr;
  PoDoFo::PdfObject* Parent = nullptr;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
