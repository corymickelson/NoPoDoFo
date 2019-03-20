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

#ifndef NPDF_BASEDOCUMENT_H
#define NPDF_BASEDOCUMENT_H

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace NoPoDoFo {

class BaseDocument
{
public:
  explicit BaseDocument(const Napi::CallbackInfo&, bool);
  virtual ~BaseDocument();
  Napi::Value GetPageCount(const Napi::CallbackInfo&);
  virtual Napi::Value GetPage(const Napi::CallbackInfo&);
  Napi::Value GetObjects(const Napi::CallbackInfo&);
  Napi::Value GetObject(const Napi::CallbackInfo&);
  Napi::Value IsAllowed(const Napi::CallbackInfo&);
  Napi::Value CreateFont(const Napi::CallbackInfo&);
  Napi::Value CreateFontSubset(const Napi::CallbackInfo&);
  Napi::Value GetPageMode(const Napi::CallbackInfo&);
  Napi::Value GetForm(const Napi::CallbackInfo&);
  void SetPageMode(const Napi::CallbackInfo&, const Napi::Value&);
  void SetPageLayout(const Napi::CallbackInfo&, const Napi::Value&);
  void SetUseFullScreen(const Napi::CallbackInfo&);
  void SetHideToolbar(const Napi::CallbackInfo&);
  void SetHideMenubar(const Napi::CallbackInfo&);
  void SetHideWindowUI(const Napi::CallbackInfo&);
  void SetFitWindow(const Napi::CallbackInfo&);
  void SetCenterWindow(const Napi::CallbackInfo&);
  void SetDisplayDocTitle(const Napi::CallbackInfo&);
  void SetPrintingScale(const Napi::CallbackInfo&, const Napi::Value&);
  void SetLanguage(const Napi::CallbackInfo&, const Napi::Value&);
  void AttachFile(const Napi::CallbackInfo&);
  Napi::Value GetVersion(const Napi::CallbackInfo&);
  Napi::Value IsLinearized(const Napi::CallbackInfo&);
  Napi::Value GetWriteMode(const Napi::CallbackInfo&);
  virtual Napi::Value InsertExistingPage(const Napi::CallbackInfo&);
  Napi::Value GetInfo(const Napi::CallbackInfo&);
  Napi::Value GetOutlines(const Napi::CallbackInfo&);
  Napi::Value GetNamesTree(const Napi::CallbackInfo&);
  virtual Napi::Value CreatePage(const Napi::CallbackInfo&);
  Napi::Value CreatePages(const Napi::CallbackInfo&);
  Napi::Value InsertPage(const Napi::CallbackInfo&);
  virtual void Append(const Napi::CallbackInfo&);
  Napi::Value GetAttachment(const Napi::CallbackInfo&);
  void AddNamedDestination(const Napi::CallbackInfo&);
  Napi::Value CreateXObject(const Napi::CallbackInfo&);

  PoDoFo::PdfDocument* base;
  PoDoFo::PdfRefCountedBuffer* streamDocRefCountedBuffer = nullptr;
  PoDoFo::PdfOutputDevice* streamDocOutputDevice = nullptr;
  string output;

protected:
  PoDoFo::PdfFont* CreateFontObject(napi_env, Napi::Object, bool subset);
  std::string pwd;
  vector<PoDoFo::PdfObject*> copies;

private:
};
}
#endif
