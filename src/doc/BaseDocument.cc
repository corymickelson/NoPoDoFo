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
#include "../doc/Rect.h"
#include "FileSpec.h"
#include "Font.h"
#include "Page.h"

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::make_shared;
using std::string;

namespace NoPoDoFo {

BaseDocument::BaseDocument(const Napi::CallbackInfo& info)
{
  if (info.Length() == 2 && info[1].IsBoolean() && info[0].IsString()) {
    create = info[1].As<Boolean>();
    output = info[0].As<String>().Utf8Value();
  }
  if (create) {
    document = make_shared<PdfStreamedDocument>(output.c_str());
    cout << "Creating new StreamedDocument at " << output << endl;
  } else {
    document = make_shared<PdfMemDocument>();
    cout << "Creating new MemDocument" << endl;
  }
}

BaseDocument::~BaseDocument()
{
  std::cout << "Destructing BaseDocument" << std::endl;
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
  PdfFileSpec attachment(filepath.c_str(), true, document.get());
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
/**
 * @note Javascript args (doc:Document, pageN:number, atN:number)
 * @param info
 * @return
 */
Napi::Value
BaseDocument::InsertExistingPage(const CallbackInfo& info)
{
  auto memDoc = Document::Unwrap(info[0].As<Object>());
  int memPageN = info[1].As<Number>();
  int atN = info[2].As<Number>();
  if (document->GetPageCount() < atN) {
    throw RangeError();
  }
  document->InsertExistingPageAt(memDoc->GetMemDocument().get(), memPageN, atN);
  return Number::New(info.Env(), document->GetPageCount());
}
Napi::Value
BaseDocument::GetInfo(const CallbackInfo& info)
{
  PdfInfo* i = document->GetInfo();
  auto infoObj = Object::New(info.Env());
  infoObj.Set("author", i->GetAuthor().GetStringUtf8());
  infoObj.Set("createdAt", i->GetCreationDate().GetTime());
  infoObj.Set("creator", i->GetCreator().GetStringUtf8());
  infoObj.Set("keywords", i->GetKeywords().GetStringUtf8());
  infoObj.Set("producer", i->GetProducer().GetStringUtf8());
  infoObj.Set("subject", i->GetSubject().GetStringUtf8());
  infoObj.Set("title", i->GetTitle().GetStringUtf8());
  return infoObj;
}
/**
 *
 * @param info
 * @return Obj - The outline object dictionary
 */
Napi::Value
BaseDocument::GetOutlines(const CallbackInfo& info)
{
  auto outline = document->GetOutlines(info[0].As<Boolean>())->GetObject();
  return Obj::constructor.New(
    { External<PdfObject>::New(info.Env(), outline) });
}
Napi::Value
BaseDocument::GetNamesTree(const CallbackInfo& info)
{
  return Obj::constructor.New({ External<PdfObject>::New(
    info.Env(), document->GetNamesTree(info[0].As<Boolean>())->GetObject()) });
}
Napi::Value
BaseDocument::CreatePage(const CallbackInfo& info)
{
  auto r = Rect::Unwrap(info[0].As<Object>())->GetRect();
  return Page::constructor.New(
    { External<PdfPage>::New(info.Env(), document->CreatePage(*r)) });
}
Napi::Value
BaseDocument::CreatePages(const Napi::CallbackInfo& info)
{
  auto coll = info[0].As<Array>();
  vector<PdfRect> rects;
  for (int n = coll.Length(); n >= 0; n++) {
    PdfRect r = *Rect::Unwrap(coll.Get(n).As<Object>())->GetRect();
    rects.push_back(r);
  }
  document->CreatePages(rects);
  return Number::New(info.Env(), document->GetPageCount());
}
Napi::Value
BaseDocument::InsertPage(const Napi::CallbackInfo& info)
{
  auto rect = Rect::Unwrap(info[0].As<Object>())->GetRect();
  int index = info[1].As<Number>();
  document->InsertPage(*rect, index);
  return Page::constructor.New(
    { External<PdfPage>::New(info.Env(), document->GetPage(index)) });
}
Napi::Value
BaseDocument::Append(const Napi::CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string docPath = info[0].As<String>().Utf8Value();
  PdfMemDocument mergedDoc;
  try {
    mergedDoc.Load(docPath.c_str());
  } catch (PdfError& err) {
    if (err.GetError() == ePdfError_InvalidPassword && info.Length() != 2) {
      throw Error::New(info.Env(),
                       "Password required to modify this document. Call "
                       "MergeDocument(filePath, password)");
    } else if (err.GetError() == ePdfError_InvalidPassword &&
               info.Length() == 2 && info[1].IsString()) {
      string password = info[1].As<String>().Utf8Value();
      mergedDoc.SetPassword(password);
    }
  }
  try {
    document->Append(mergedDoc);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}
/**
 * @note Javascript args (page::Page, Destination: NPDFDestinationFit,
 * attachmentName:string)
 * @param info
 * @return
 */
Napi::Value
BaseDocument::GetAttachment(const CallbackInfo& info)
{
  return FileSpec::constructor.New({ External<PdfFileSpec>::New(
    info.Env(), document->GetAttachment(info[0].As<String>().Utf8Value())) });
}
void
BaseDocument::AddNamedDestination(const Napi::CallbackInfo& info)
{
  PdfPage* page = Page::Unwrap(info[0].As<Object>())->GetPage();
  EPdfDestinationFit fit =
    static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value());
  string name = info[2].As<String>().Utf8Value();
  PdfDestination destination(page, fit);
  document->AddNamedDestination(destination, name);
}
}
