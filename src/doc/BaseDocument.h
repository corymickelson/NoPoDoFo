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

#ifndef NPDF_BASEDOCUMENT_H
#define NPDF_BASEDOCUMENT_H

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using JsValue = Napi::Value;

namespace NoPoDoFo {

class BaseDocument
{
public:
  explicit BaseDocument(const Napi::CallbackInfo&, bool);
  explicit BaseDocument(const BaseDocument&) = delete;
  const BaseDocument& operator=(const BaseDocument&) = delete;
  virtual ~BaseDocument();
  JsValue GetPageCount(const Napi::CallbackInfo&);
  virtual JsValue GetPage(const Napi::CallbackInfo&);
  JsValue GetObjects(const Napi::CallbackInfo&);
  JsValue GetObject(const Napi::CallbackInfo&);
  JsValue IsAllowed(const Napi::CallbackInfo&);
  JsValue CreateFont(const Napi::CallbackInfo&);
  JsValue CreateFontSubset(const Napi::CallbackInfo&);
  JsValue GetPageMode(const Napi::CallbackInfo&);
  JsValue GetForm(const Napi::CallbackInfo&);
  void SetPageMode(const Napi::CallbackInfo&, const JsValue&);
  void SetPageLayout(const Napi::CallbackInfo&, const JsValue&);
  void SetUseFullScreen(const Napi::CallbackInfo&);
  void SetHideToolbar(const Napi::CallbackInfo&);
  void SetHideMenubar(const Napi::CallbackInfo&);
  void SetHideWindowUI(const Napi::CallbackInfo&);
  void SetFitWindow(const Napi::CallbackInfo&);
  void SetCenterWindow(const Napi::CallbackInfo&);
  void SetDisplayDocTitle(const Napi::CallbackInfo&);
  void SetPrintingScale(const Napi::CallbackInfo&, const JsValue&);
  void SetLanguage(const Napi::CallbackInfo&, const JsValue&);
  void AttachFile(const Napi::CallbackInfo&);
  JsValue GetVersion(const Napi::CallbackInfo&);
  JsValue IsLinearized(const Napi::CallbackInfo&);
  JsValue GetWriteMode(const Napi::CallbackInfo&);
  virtual JsValue InsertExistingPage(const Napi::CallbackInfo&);
  JsValue GetInfo(const Napi::CallbackInfo&);
  JsValue GetOutlines(const Napi::CallbackInfo&);
  JsValue GetNamesTree(const Napi::CallbackInfo&);
  virtual JsValue CreatePage(const Napi::CallbackInfo&);
  JsValue CreatePages(const Napi::CallbackInfo&);
  JsValue InsertPage(const Napi::CallbackInfo&);
  virtual void Append(const Napi::CallbackInfo&);
  JsValue GetAttachment(const Napi::CallbackInfo&);
  void AddNamedDestination(const Napi::CallbackInfo&);
  JsValue CreateXObject(const Napi::CallbackInfo&);

  PoDoFo::PdfDocument* Base;
  PoDoFo::PdfRefCountedBuffer* StreamDocRefCountedBuffer = nullptr;
  PoDoFo::PdfOutputDevice* StreamDocOutputDevice = nullptr;
  string Output;

protected:
  PoDoFo::PdfFont* CreateFontObject(napi_env, Napi::Object, bool subset);
  std::string Pwd;
  vector<PoDoFo::PdfObject*> Copies;

	std::shared_ptr<spdlog::logger> Log;
};
}
#endif
