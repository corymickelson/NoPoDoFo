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

#ifndef NPDF_ACTION_H
#define NPDF_ACTION_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Action : public Napi::ObjectWrap<Action>
{
public:
  explicit Action(const Napi::CallbackInfo& info);
  explicit Action(const Action&) = delete;
  const Action& operator=(const Action&) = delete;
  ~Action();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetUri(const Napi::CallbackInfo&);
  JsValue GetScript(const Napi::CallbackInfo&);
  JsValue GetType(const Napi::CallbackInfo&);
  void SetUri(const Napi::CallbackInfo&, const JsValue&);
  void SetScript(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetObject(const Napi::CallbackInfo&);
  void AddToDictionary(const Napi::CallbackInfo&);
  PoDoFo::PdfAction& GetAction() const { return *Self; }

private:
  PoDoFo::PdfAction* Self;
  std::shared_ptr<spdlog::logger> Log;
};
}
#endif // NPDF_ACTION_H
