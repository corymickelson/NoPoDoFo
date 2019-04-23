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

#include <napi.h>
#include <podofo/podofo.h>
#include "spdlog/logger.h"

using Napi::CallbackInfo;
using value = Napi::Value;
using PoDoFo::PdfColor;

namespace NoPoDoFo {
class Color : public Napi::ObjectWrap<Color>
{
public:
  static Napi::FunctionReference constructor;

  static void Initialize(Napi::Env& env, Napi::Object& target);

  explicit Color(const CallbackInfo&);
  ~Color();

  value GetGrey(const CallbackInfo&);

  value GetCyan(const CallbackInfo&);
  value GetMagenta(const CallbackInfo&);
  value GetYellow(const CallbackInfo&);
  value GetBlack(const CallbackInfo&);

  value GetBlue(const CallbackInfo&);
  value GetRed(const CallbackInfo&);
  value GetGreen(const CallbackInfo&);

  value IsRGB(const CallbackInfo&);
  value IsCMYK(const CallbackInfo&);
  value IsGreyScale(const CallbackInfo&);
  value GetName(const CallbackInfo&);
  value GetDensity(const CallbackInfo&);
  value ConvertToGreyScale(const CallbackInfo&);
  value ConvertToRGB(const CallbackInfo&);
  value ConvertToCMYK(const CallbackInfo&);
  value GetColorStreamString(const CallbackInfo &);
  PdfColor* color;

private:
  std::shared_ptr<spdlog::logger> dbglog;
};
}

#endif
