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

#ifndef NPDF_COLOR_H
#define NPDF_COLOR_H

#include "spdlog/logger.h"
#include <napi.h>
#include <podofo/podofo.h>

using Napi::CallbackInfo;
using JsValue = Napi::Value;
using PoDoFo::PdfColor;

namespace NoPoDoFo {
class Color : public Napi::ObjectWrap<Color>
{
public:
  static Napi::FunctionReference Constructor;

  static void Initialize(Napi::Env& env, Napi::Object& target);

  explicit Color(const CallbackInfo&);
  explicit Color(const Color&) = delete;
  const Color& operator=(const Color&) = delete;
  ~Color();

  JsValue GetGrey(const CallbackInfo&);

  JsValue GetCyan(const CallbackInfo&);
  JsValue GetMagenta(const CallbackInfo&);
  JsValue GetYellow(const CallbackInfo&);
  JsValue GetBlack(const CallbackInfo&);

  JsValue GetBlue(const CallbackInfo&);
  JsValue GetRed(const CallbackInfo&);
  JsValue GetGreen(const CallbackInfo&);

  JsValue IsRGB(const CallbackInfo&);
  JsValue IsCMYK(const CallbackInfo&);
  JsValue IsGreyScale(const CallbackInfo&);
  JsValue GetName(const CallbackInfo&);
  JsValue GetDensity(const CallbackInfo&);
  JsValue ConvertToGreyScale(const CallbackInfo&);
  JsValue ConvertToRGB(const CallbackInfo&);
  JsValue ConvertToCMYK(const CallbackInfo&);
  JsValue GetColorStreamString(const CallbackInfo&);
  PdfColor* Self;

private:
  std::shared_ptr<spdlog::logger> DbgLog;
};
}

#endif
