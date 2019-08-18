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

#ifndef NPDF_OUTLINE_H
#define NPDF_OUTLINE_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using std::vector;
using JsValue = Napi::Value;

namespace NoPoDoFo {

/**
 * @note PdfOutlineItem Wrapper, PdfOutline::CreateRoot exposed through
 * BaseDocument.
 */
class Outline : public Napi::ObjectWrap<Outline>
{
public:
  explicit Outline(const Napi::CallbackInfo& info);
  explicit Outline(const Outline&) = delete;
  const Outline& operator=(const Outline&) = delete;
  ~Outline();
	static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue CreateChild(const Napi::CallbackInfo&);
  JsValue CreateNext(const Napi::CallbackInfo&);
  JsValue InsertChild(const Napi::CallbackInfo&);
  JsValue Prev(const Napi::CallbackInfo&);
  JsValue Next(const Napi::CallbackInfo&);
  JsValue First(const Napi::CallbackInfo&);
  JsValue Last(const Napi::CallbackInfo&);
  JsValue GetParent(const Napi::CallbackInfo&);
  JsValue Erase(const Napi::CallbackInfo&);
  JsValue GetDestination(const Napi::CallbackInfo&);
  void SetDestination(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetAction(const Napi::CallbackInfo&);
  void SetAction(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetTitle(const Napi::CallbackInfo&);
  void SetTitle(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetTextFormat(const Napi::CallbackInfo&);
  void SetTextFormat(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetTextColor(const Napi::CallbackInfo&);
  void SetTextColor(const Napi::CallbackInfo&, const JsValue&);
  PoDoFo::PdfOutlineItem& GetOutline() { return Self; }

private:
  PoDoFo::PdfOutlineItem& Self; // owned by the document
  std::shared_ptr<spdlog::logger> Log;
};
}
#endif // NPDF_OUTLINE_H
