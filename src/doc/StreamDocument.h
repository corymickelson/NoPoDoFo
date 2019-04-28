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

#ifndef NPDF_STREAMDOCUMENT_H
#define NPDF_STREAMDOCUMENT_H

#include "BaseDocument.h"

#include <napi.h>
#include <podofo/podofo.h>
using JsValue = Napi::Value;

namespace NoPoDoFo {
class StreamDocument final
  : public Napi::ObjectWrap<StreamDocument>
    , public BaseDocument
{
public:
  static Napi::FunctionReference Constructor;
  explicit StreamDocument(const Napi::CallbackInfo&);
  explicit StreamDocument(const StreamDocument&) = delete;
  const StreamDocument& operator=(const StreamDocument&) = delete;
  ~StreamDocument();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue Close(const Napi::CallbackInfo&);
  void Append(const Napi::CallbackInfo&) override;
  JsValue InsertExistingPage(const Napi::CallbackInfo&) override;

  PoDoFo::PdfStreamedDocument& GetStreamedDocument() const
  {
    return *dynamic_cast<PoDoFo::PdfStreamedDocument*>(BaseDocument::Base);
  }
};
};
#endif
