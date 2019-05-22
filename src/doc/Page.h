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

#ifndef NPDF_PDFPAGE_HPP
#define NPDF_PDFPAGE_HPP

#define CONVERSION_CONSTANT 0.002834645669291339

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using std::cout;
using std::endl;
using JsValue = Napi::Value;

namespace NoPoDoFo {

class Page : public Napi::ObjectWrap<Page>
{
public:
  explicit Page(const Napi::CallbackInfo& callbackInfo);
  explicit Page(const Page&) = delete;
  const Page operator=(const Page&) = delete;
  ~Page();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetRotation(const Napi::CallbackInfo&);
  JsValue GetNumFields(const Napi::CallbackInfo&);
  void SetRotation(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetField(const Napi::CallbackInfo&);
  JsValue GetField(const Napi::Env&, int);
  JsValue GetFields(const Napi::CallbackInfo&);
  JsValue GetFieldIndex(const Napi::CallbackInfo&);
  void SetPageWidth(const Napi::CallbackInfo&, const JsValue&);
  void SetPageHeight(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetPageWidth(const Napi::CallbackInfo&);
  JsValue GetPageHeight(const Napi::CallbackInfo&);
  JsValue GetTrimBox(const Napi::CallbackInfo&);
  void SetTrimBox(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetPageNumber(const Napi::CallbackInfo&);
  JsValue GetContents(const Napi::CallbackInfo&);
  JsValue GetResources(const Napi::CallbackInfo&);
  JsValue GetMediaBox(const Napi::CallbackInfo&);
  JsValue GetBleedBox(const Napi::CallbackInfo&);
  JsValue GetArtBox(const Napi::CallbackInfo&);
  JsValue CreateAnnotation(const Napi::CallbackInfo&);
  JsValue GetAnnotation(const Napi::CallbackInfo&);
  JsValue CreateField(const Napi::CallbackInfo&);
  JsValue GetNumAnnots(const Napi::CallbackInfo&);
  void DeleteAnnotation(const Napi::CallbackInfo&);
  void DeleteField(const Napi::CallbackInfo&);
  static bool DeleteFormField(PoDoFo::PdfPage&,
                              PoDoFo::PdfObject&,
                              PoDoFo::PdfObject&);
#if NOPODOFO_SDK
  void FlattenFields(const Napi::CallbackInfo&);
#endif
  PoDoFo::PdfPage& Self;
  Napi::Object ExtractAndApplyRectValues(const Napi::CallbackInfo&,
                                         PoDoFo::PdfRect&);

private:
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_PDFPAGE_HPP
