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

#ifndef NPDF_FORM_H
#define NPDF_FORM_H

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using std::cout;
using std::endl;
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Form : public Napi::ObjectWrap<Form>
{
public:
  explicit Form(const Napi::CallbackInfo&);
  explicit Form(const Form&) = delete;
  const Form& operator=(const Form&) = delete;
  ~Form();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetNeedAppearances(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetNeedAppearances(const Napi::CallbackInfo&);
  JsValue GetFormDictionary(const Napi::CallbackInfo&);
  JsValue SigFlags(const Napi::CallbackInfo&);
  void SetSigFlags(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetDefaultAppearance(const Napi::CallbackInfo&);
  void SetDefaultAppearance(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetResource(const Napi::CallbackInfo&);
  void SetResource(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetCalculationOrder(const Napi::CallbackInfo&);
  void SetCalculationOrder(const Napi::CallbackInfo&, const JsValue&);
  void RefreshAppearances(const Napi::CallbackInfo&);
  PoDoFo::PdfAcroForm* GetForm() const { return Doc.GetAcroForm(Create); }

  PoDoFo::PdfDictionary* GetDictionary() const
  {
    return &(Doc.GetAcroForm()->GetObject()->GetDictionary());
  }

  std::map<std::string, PoDoFo::PdfObject*> GetFieldAPKeys(PoDoFo::PdfField*);
private:
  bool Create = true;
  PoDoFo::PdfDocument& Doc;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
