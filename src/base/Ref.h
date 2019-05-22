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

#ifndef NOPODOFO_REF_H
#define NOPODOFO_REF_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using namespace Napi;

using JsValue = Napi::Value;
using PoDoFo::PdfReference;

namespace NoPoDoFo {
class Ref : public Napi::ObjectWrap<Ref>
{
public:
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  explicit Ref(const CallbackInfo& info);
  explicit Ref(const Ref&) = delete;
  const Ref& operator=(const Ref&) = delete;
  ~Ref();
  JsValue GetObjectNumber(const CallbackInfo&);
  JsValue GetGenerationNumber(const CallbackInfo&);

  PdfReference* Self;
private:
  std::shared_ptr<spdlog::logger> DbgLog;
};

}

#endif // NOPODOFO_REF_H
