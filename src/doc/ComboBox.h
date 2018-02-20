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



#ifndef NPDF_COMBOBOX_H
#define NPDF_COMBOBOX_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class ComboBox : public Napi::ObjectWrap<ComboBox> {
public:
    explicit ComboBox(const Napi::CallbackInfo &);
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env&, Napi::Object&);

private:
    std::unique_ptr<PoDoFo::PdfComboBox> self;
};
}



#endif //NPDF_COMBOBOX_H
