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

#ifndef NPDF_PUSHBUTTON_H
#define NPDF_PUSHBUTTON_H

#include "Button.h"
#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>
using JsValue = Value;

namespace NoPoDoFo {

class PushButton final
  : public ObjectWrap<PushButton>
    , public Field
    , public Button
{
public:
  static FunctionReference Constructor;
  explicit PushButton(const CallbackInfo& callbackInfo);
  explicit PushButton(const PushButton&) = delete;
  const PushButton& operator=(const PushButton&) = delete;
  static void Initialize(Napi::Env& env, Object& target);
  JsValue GetRolloverCaption(const CallbackInfo&);
  JsValue GetAlternateCaption(const CallbackInfo&);
  void SetRolloverCaption(const CallbackInfo&, const JsValue&);
  void SetAlternateCaption(const CallbackInfo&, const JsValue&);
  PdfPushButton GetPushButton() { return PdfPushButton(Self); }
  PdfField& Self;
private:
  std::shared_ptr<spdlog::logger> Log;
};
}
#endif // NPDF_PUSHBUTTON_H
