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

#ifndef NPDF_LISTBOX_H
#define NPDF_LISTBOX_H

#include "Field.h"
#include "ListField.h"
#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {

class ListBox final
  : public Napi::ObjectWrap<ListBox>
  , public Field
  , public ListField
{
public:
  explicit ListBox(const Napi::CallbackInfo&);
  ~ListBox();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env&, Napi::Object&);

  PoDoFo::PdfListBox GetField() { return PoDoFo::PdfListBox(field); }

private:
  PoDoFo::PdfField& field;
  std::shared_ptr<spdlog::logger> dbglog;
};
}

#endif // NPDF_LISTBOX_H
