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

#ifndef NPDF_RECT_H
#define NPDF_RECT_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Rect : public Napi::ObjectWrap<Rect>
{
public:
  explicit Rect(const Napi::CallbackInfo& callbackInfo);
  explicit Rect(const Rect&) = delete;
  const Rect& operator=(const Rect&) = delete;
  ~Rect();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void Intersect(const Napi::CallbackInfo&);
  JsValue GetWidth(const Napi::CallbackInfo&);
  void SetWidth(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetHeight(const Napi::CallbackInfo&);
  void SetHeight(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetBottom(const Napi::CallbackInfo&);
  void SetBottom(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetLeft(const Napi::CallbackInfo&);
  void SetLeft(const Napi::CallbackInfo&, const JsValue&);
  PoDoFo::PdfRect& GetRect() const { return *Self; }

private:
  PoDoFo::PdfRect* Self;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_RECT_H
