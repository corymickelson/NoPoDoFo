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

#ifndef NPDF_BUTTON_H
#define NPDF_BUTTON_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Button
{
public:
  explicit Button(PoDoFo::PdfField&);
  explicit Button(const Button&) = delete;
  const Button& operator=(const Button&) = delete;
  ~Button();
  JsValue GetCaption(const Napi::CallbackInfo&);
  void SetCaption(const Napi::CallbackInfo&, const JsValue&);
  PoDoFo::PdfObject* GetStateAppearanceStream();
  void WriteAppearanceStream(const PoDoFo::PdfXObject& canvas);
  PoDoFo::PdfButton* Btn;
  PoDoFo::PdfField& Field;

protected:
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_BUTTON_H
