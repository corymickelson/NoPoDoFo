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

#include "BaseDocument.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Obj.h"
#include "../base/Ref.h"
#include "Font.h"
#include "Page.h"

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;

namespace NoPoDoFo {

BaseDocument::BaseDocument() {}

BaseDocument::~BaseDocument()
{
  delete document;
}

Napi::Value
BaseDocument::GetPageCount(const CallbackInfo& info)
{
  int pages = document->GetPageCount();
  return Number::New(info.Env(), pages);
}
Napi::Value
BaseDocument::GetPage(const CallbackInfo& info)
{
  int n = info[0].As<Number>();
  PdfPage* page = document->GetPage(n);
  auto pagePtr = External<PdfPage>::New(info.Env(), page);
  auto instance = Page::constructor.New({ pagePtr });
  return instance;
}

void
BaseDocument::SetHideMenubar(const Napi::CallbackInfo&)
{
  document->SetHideMenubar();
}

Napi::Value
BaseDocument::GetPageMode(const CallbackInfo& info)
{
  string mode;
  switch (document->GetPageMode()) {
    case ePdfPageModeDontCare:
      mode = "DontCare";
      break;
    case ePdfPageModeFullScreen:
      mode = "FullScreen";
      break;
    case ePdfPageModeUseAttachments:
      mode = "UseAttachments";
      break;
    case ePdfPageModeUseBookmarks:
      mode = "UseBookmarks";
      break;
    case ePdfPageModeUseNone:
      mode = "UseNone";
      break;
    case ePdfPageModeUseOC:
      mode = "UseOC";
      break;
    case ePdfPageModeUseThumbs:
      mode = "UseThumbs";
      break;
  }
  return String::New(info.Env(), mode);
}

void
BaseDocument::SetPageMode(const CallbackInfo&, const Napi::Value& value)
{
  int flag = value.As<Number>();
  auto mode = static_cast<EPdfPageMode>(flag);
  document->SetPageMode(mode);
}

void
BaseDocument::SetPageLayout(const CallbackInfo&, const Napi::Value& value)
{
  int flag = value.As<Number>();
  auto mode = static_cast<EPdfPageLayout>(flag);
  document->SetPageLayout(mode);
}

void
BaseDocument::SetUseFullScreen(const CallbackInfo&)
{
  document->SetUseFullScreen();
}

void
BaseDocument::SetHideToolbar(const CallbackInfo&)
{
  document->SetHideMenubar();
}

void
BaseDocument::SetHideWindowUI(const CallbackInfo&)
{
  document->SetHideWindowUI();
}

void
BaseDocument::SetFitWindow(const CallbackInfo&)
{
  document->SetFitWindow();
}

void
BaseDocument::SetCenterWindow(const CallbackInfo&)
{
  document->SetCenterWindow();
}

void
BaseDocument::SetDisplayDocTitle(const CallbackInfo&)
{
  document->SetDisplayDocTitle();
}

void
BaseDocument::SetPrintingScale(const CallbackInfo&, const Napi::Value& value)
{
  PdfName scale(value.As<String>().Utf8Value());
  document->SetPrintScaling(scale);
}

void
BaseDocument::SetBaseURI(const CallbackInfo&, const Napi::Value& value)
{
  document->SetBaseURI(value.As<String>().Utf8Value());
}

void
BaseDocument::SetLanguage(const CallbackInfo&, const Napi::Value& value)
{
  document->SetLanguage(value.As<String>().Utf8Value());
}

void
BaseDocument::AttachFile(const CallbackInfo& info)
{
  string filepath = info[0].As<String>().Utf8Value();
  PdfFileSpec attachment(filepath.c_str(), true, document);
  document->AttachFile(attachment);
}

Napi::Value
BaseDocument::GetVersion(const CallbackInfo& info)
{
  EPdfVersion versionE = document->GetPdfVersion();
  double v = 0.0;
  switch (versionE) {
    case ePdfVersion_1_1:
      v = 1.1;
      break;
    case ePdfVersion_1_3:
      v = 1.3;
      break;
    case ePdfVersion_1_0:
      v = 1.0;
      break;
    case ePdfVersion_1_2:
      v = 1.2;
      break;
    case ePdfVersion_1_4:
      v = 1.4;
      break;
    case ePdfVersion_1_5:
      v = 1.5;
      break;
    case ePdfVersion_1_6:
      v = 1.6;
      break;
    case ePdfVersion_1_7:
      v = 1.7;
      break;
  }
  if (v == 0.0) {
    throw Error::New(info.Env(),
                     "Failed to parse document. Pdf version unknown.");
  }
  return Number::New(info.Env(), v);
}

Napi::Value
BaseDocument::IsLinearized(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), document->IsLinearized());
}

Napi::Value
BaseDocument::GetWriteMode(const CallbackInfo& info)
{
  string writeMode;
  switch (document->GetWriteMode()) {
    case ePdfWriteMode_Clean: {
      writeMode = "Clean";
      break;
    }
    case ePdfWriteMode_Compact: {
      writeMode = "Compact";
      break;
    }
  }
  return Napi::String::New(info.Env(), writeMode);
}

Napi::Value
BaseDocument::GetObjects(const CallbackInfo& info)
{
  try {
    auto js = Array::New(info.Env());
    uint32_t count = 0;
    for (auto item : *document->GetObjects()) {
      auto instance = External<PdfObject>::New(info.Env(), item);
      js[count] = Obj::constructor.New({ instance });
      ++count;
    }
    return js;
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

Napi::Value
BaseDocument::GetObject(const CallbackInfo& info)
{
  auto ref = Ref::Unwrap(info[0].As<Object>());
  PdfObject* target = document->GetObjects()->GetObject(ref->GetRef());
  return Obj::constructor.New({ External<PdfObject>::New(info.Env(), target) });
}

Napi::Value
BaseDocument::IsAllowed(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string allowed = info[0].As<String>().Utf8Value();
  bool is;
  if (allowed == "Print") {
    is = document->IsPrintAllowed();
  } else if (allowed == "Edit") {
    is = document->IsEditAllowed();
  } else if (allowed == "Copy") {
    is = document->IsCopyAllowed();
  } else if (allowed == "EditNotes") {
    is = document->IsEditNotesAllowed();
  } else if (allowed == "FillAndSign") {
    is = document->IsFillAndSignAllowed();
  } else if (allowed == "Accessible") {
    is = document->IsAccessibilityAllowed();
  } else if (allowed == "DocAssembly") {
    is = document->IsDocAssemblyAllowed();
  } else if (allowed == "HighPrint") {
    is = document->IsHighPrintAllowed();
  } else {
    throw Napi::Error::New(
      info.Env(),
      "Unknown argument. Please see definitions file for isAllowed args");
  }
  return Napi::Boolean::New(info.Env(), is);
}

Napi::Value
BaseDocument::CreateFont(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  auto fontName = info[0].As<String>().Utf8Value();
  bool bold = false;
  bool italic = false;
  const PdfEncoding* encoding = nullptr;
  bool embed = false;
  const char* filename = nullptr;
  if (info.Length() >= 2 && info[1].IsBoolean())
    bold = info[1].As<Boolean>();
  if (info.Length() >= 3 && info[2].IsBoolean())
    italic = info[2].As<Boolean>();
  if (info.Length() >= 4 && info[3].IsNumber()) {
    int n = info[3].As<Number>();
    switch (n) {
      case 1:
        encoding = new PdfWinAnsiEncoding();
        break;
      case 2:
        encoding = new PdfStandardEncoding();
        break;
      case 3:
        encoding = new PdfDocEncoding();
        break;
      case 4:
        encoding = new PdfMacRomanEncoding();
        break;
      case 5:
        encoding = new PdfMacExpertEncoding();
        break;
      case 6:
        encoding = new PdfSymbolEncoding();
        break;
      case 7:
        encoding = new PdfZapfDingbatsEncoding();
        break;
      case 8:
        encoding = new PdfWin1250Encoding();
        break;
      case 9:
        encoding = new PdfIso88592Encoding();
        break;
      default:
        encoding = new PdfIdentityEncoding(0, 0xffff, true);
    }
  }
  if (info.Length() >= 5 && info[4].IsBoolean())
    embed = info[4].As<Boolean>();
  if (info.Length() >= 6 && info[5].IsString())
    filename = info[5].As<String>().Utf8Value().c_str();
  try {
    PdfFont* font =
      document->CreateFont(fontName.c_str(),
                           bold,
                           italic,
                           false,
                           encoding,
                           PdfFontCache::eFontCreationFlags_AutoSelectBase14,
                           embed,
                           filename);
    return Font::constructor.New({ External<PdfFont>::New(info.Env(), font) });
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
}
