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

#ifndef EXTGSTATE_H
#define EXTGSTATE_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {
class ExtGState : public Napi::ObjectWrap<ExtGState>
{
public:
  explicit ExtGState(const Napi::CallbackInfo& info);
  explicit ExtGState(const ExtGState&) = delete;
  const ExtGState& operator=(const ExtGState&) = delete;
  ~ExtGState();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetFillOpacity(const Napi::CallbackInfo& info);
  void SetStrokeOpacity(const Napi::CallbackInfo& info);
  void SetBlendMode(const Napi::CallbackInfo& info);
  void SetOverprint(const Napi::CallbackInfo& info);
  void SetFillOverprint(const Napi::CallbackInfo& info);
  void SetStrokeOverprint(const Napi::CallbackInfo& info);
  void SetNonZeroOverprint(const Napi::CallbackInfo& info);
  void SetRenderingIntent(const Napi::CallbackInfo& info);
  void SetFrequency(const Napi::CallbackInfo& info);

  PoDoFo::PdfExtGState* GetExtGState() const { return Self.get(); }

private:
  std::unique_ptr<PoDoFo::PdfExtGState> Self;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
