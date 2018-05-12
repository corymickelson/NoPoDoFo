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

#ifndef NPDF_FORM_H
#define NPDF_FORM_H

#include "Document.h"

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Form : public Napi::ObjectWrap<Form>
{
public:
  explicit Form(const Napi::CallbackInfo&);
  ~Form();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetNeedAppearances(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetNeedAppearances(const Napi::CallbackInfo&);
  Napi::Value GetFormDictionary(const Napi::CallbackInfo&);
  Napi::Value SigFlags(const Napi::CallbackInfo&);
  void SetSigFlags(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetDefaultAppearance(const Napi::CallbackInfo&);
  void SetDefaultAppearance(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetDR(const Napi::CallbackInfo&);
  void SetDR(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetCalculationOrder(const Napi::CallbackInfo&);
  void SetCalculationOrder(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetFont(const Napi::CallbackInfo&);

  PoDoFo::PdfAcroForm* GetForm() { return form; }

protected:
  void FlattenFields(PoDoFo::PdfArray&, vector<PoDoFo::PdfField*>*);
  PoDoFo::PdfDictionary* DR();

private:
  bool create = true;
  Document* doc;
  PoDoFo::PdfAcroForm* form;
};
}
#endif
