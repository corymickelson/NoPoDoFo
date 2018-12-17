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
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "../base/Ref.h"
#include "../base/XObject.h"
#include "../doc/Rect.h"
#include "Document.h"
#include "Encrypt.h"
#include "FileSpec.h"
#include "Font.h"
#include "Form.h"
#include "Outline.h"
#include "Page.h"
#include "StreamDocument.h"

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using tl::nullopt;

namespace NoPoDoFo {

/**
 * @note JS Derived class instantiate new BaseDocument<T>(string: filePath,
 * opts: {version, writeMode, encrypt})
 *
 * @brief BaseDocument::BaseDocument
 * @param info
 */
BaseDocument::BaseDocument(const Napi::CallbackInfo& info, bool inMem)
{
  if (inMem) {
    base = new PdfMemDocument();
  } else {
    EPdfVersion version = ePdfVersion_1_7;
    EPdfWriteMode writeMode = ePdfWriteMode_Default;
    PdfEncrypt* encrypt = nullptr;

    if (info.Length() >= 2 && info[1].IsObject()) {
      auto nObj = info[1].As<Object>();
      if (nObj.Has("version")) {
        version = static_cast<EPdfVersion>(
          nObj.Get("version").As<Number>().Uint32Value());
      }
      if (nObj.Has("writeMode")) {
        writeMode = static_cast<EPdfWriteMode>(
          nObj.Get("writeMode").As<Number>().Uint32Value());
      }
      if (nObj.Has("encrypt")) {
        auto nEncObj = Encrypt::Unwrap(nObj.Get("encrypt").As<Object>());
        encrypt = const_cast<PdfEncrypt*>(nEncObj->encrypt);
      }
    }
    if (info.Length() > 0 && info[0].IsString()) {
      output = info[0].As<String>().Utf8Value();
      base =
        new PdfStreamedDocument(output.c_str(), version, encrypt, writeMode);
    } else {
      streamDocRefCountedBuffer = new PdfRefCountedBuffer(2048);
      streamDocOutputDevice = new PdfOutputDevice(streamDocRefCountedBuffer);
      base = new PdfStreamedDocument(
        streamDocOutputDevice, version, encrypt, writeMode);
    }
  }
}

BaseDocument::~BaseDocument()
{
  cout << "Cleanup" << endl;
  delete base;
  delete streamDocOutputDevice;
  delete streamDocRefCountedBuffer;
}

Napi::Value
BaseDocument::GetPageCount(const CallbackInfo& info)
{
  int pages = base->GetPageCount();
  return Number::New(info.Env(), pages);
}
Napi::Value
BaseDocument::GetPage(const CallbackInfo& info)
{
  int n = info[0].As<Number>();
  if (n < 0 || n > base->GetPageCount()) {
    RangeError::New(info.Env(), "Page index out of range")
      .ThrowAsJavaScriptException();
    return {};
  }
  return Page::constructor.New(
    { External<PdfPage>::New(info.Env(), base->GetPage(n)) });
}

void
BaseDocument::SetHideMenubar(const Napi::CallbackInfo&)
{
  base->SetHideMenubar();
}

Napi::Value
BaseDocument::GetPageMode(const CallbackInfo& info)
{
  return Number::New(info.Env(), static_cast<int>(base->GetPageMode()));
}

void
BaseDocument::SetPageMode(const CallbackInfo&, const Napi::Value& value)
{
  int flag = value.As<Number>();
  auto mode = static_cast<EPdfPageMode>(flag);
  base->SetPageMode(mode);
}

void
BaseDocument::SetPageLayout(const CallbackInfo&, const Napi::Value& value)
{
  int flag = value.As<Number>();
  auto mode = static_cast<EPdfPageLayout>(flag);
  base->SetPageLayout(mode);
}

void
BaseDocument::SetUseFullScreen(const CallbackInfo&)
{
  base->SetUseFullScreen();
}

void
BaseDocument::SetHideToolbar(const CallbackInfo&)
{
  base->SetHideMenubar();
}

void
BaseDocument::SetHideWindowUI(const CallbackInfo&)
{
  base->SetHideWindowUI();
}

void
BaseDocument::SetFitWindow(const CallbackInfo&)
{
  base->SetFitWindow();
}

void
BaseDocument::SetCenterWindow(const CallbackInfo&)
{
  base->SetCenterWindow();
}

void
BaseDocument::SetDisplayDocTitle(const CallbackInfo&)
{
  base->SetDisplayDocTitle();
}

void
BaseDocument::SetPrintingScale(const CallbackInfo&, const Napi::Value& value)
{
  PdfName scale(value.As<String>().Utf8Value());
  base->SetPrintScaling(scale);
}

void
BaseDocument::SetLanguage(const CallbackInfo&, const Napi::Value& value)
{
  base->SetLanguage(value.As<String>().Utf8Value());
}

void
BaseDocument::AttachFile(const CallbackInfo& info)
{
  string value = info[0].As<String>().Utf8Value();
  if (!FileAccess(value)) {
    Error::New(info.Env(), "File: " + value + " not found")
      .ThrowAsJavaScriptException();
    return;
  }
  bool embed = true;
  if (!output.empty() || (streamDocOutputDevice && streamDocRefCountedBuffer)) {
    embed = false;
  }
  PdfFileSpec attachment(value.c_str(), embed, base);
  base->AttachFile(attachment);
}

Napi::Value
BaseDocument::GetVersion(const CallbackInfo& info)
{
  EPdfVersion versionE = base->GetPdfVersion();
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
  return Boolean::New(info.Env(), base->IsLinearized());
}

Napi::Value
BaseDocument::GetWriteMode(const CallbackInfo& info)
{
  string writeMode;
  switch (base->GetWriteMode()) {
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
    for (auto item : *base->GetObjects()) {
      if (item->IsReference()) {
        item = item->GetOwner()->GetObject(item->GetReference());
      }
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
  PdfReference* ref = nullptr;
  if (info[0].IsArray()) {
    unsigned long g = info[0]
                        .As<Array>()
                        .Get(static_cast<uint32_t>(0))
                        .As<Number>()
                        .Uint32Value();
    pdf_objnum o =
      info[0].As<Array>().Get(static_cast<uint32_t>(1)).As<Number>();
    auto gen = static_cast<pdf_gennum>(g);
    PdfReference r(o, gen);
    ref = &r;
  } else if (info[0].IsObject() &&
             info[0].As<Object>().InstanceOf(Ref::constructor.Value())) {
    ref = Ref::Unwrap(info[0].As<Object>())->self;
  }
  if (!ref) {
    Error::New(info.Env(), "NoPoDoFo is unable to resolve null reference")
      .ThrowAsJavaScriptException();
    return {};
  }
  PdfObject* target = base->GetObjects()->GetObject(*ref);
  if (!target) {
    stringstream oss;
    oss << "NoPoDoFo is unable to resolve reference " << ref->ObjectNumber()
        << " : " << ref->GenerationNumber();
    Error::New(info.Env(), oss.str()).ThrowAsJavaScriptException();
    return {};
  }
  return Obj::constructor.New({ External<PdfObject>::New(info.Env(), target) });
}

Napi::Value
BaseDocument::IsAllowed(const CallbackInfo& info)
{
  string allowed = info[0].As<String>().Utf8Value();
  bool is;
  if (allowed == "Print") {
    is = base->IsPrintAllowed();
  } else if (allowed == "Edit") {
    is = base->IsEditAllowed();
  } else if (allowed == "Copy") {
    is = base->IsCopyAllowed();
  } else if (allowed == "EditNotes") {
    is = base->IsEditNotesAllowed();
  } else if (allowed == "FillAndSign") {
    is = base->IsFillAndSignAllowed();
  } else if (allowed == "Accessible") {
    is = base->IsAccessibilityAllowed();
  } else if (allowed == "DocAssembly") {
    is = base->IsDocAssemblyAllowed();
  } else if (allowed == "HighPrint") {
    is = base->IsHighPrintAllowed();
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
  if (info.Length() < 1 || !info[0].IsObject()) {
    Error::New(info.Env(), "Invalid arguement, expected an object")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  PdfFont* font = CreateFontObject(info.Env(), info[0].As<Object>(), false);
  return Font::constructor.New({ External<PdfFont>::New(info.Env(), font) });
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
  if (atN < 0) {
    cout << "Appending page to the beginning of this document" << endl;
  }
  if (base->GetPageCount() + 1 < atN) {
    RangeError::New(info.Env(), "at index out of range")
      .ThrowAsJavaScriptException();
    return {};
  }
  if (memPageN < 0 || memPageN > memDoc->base->GetPageCount() - 1) {
    RangeError::New(info.Env(), "parameter document page range exception")
      .ThrowAsJavaScriptException();
    return {};
  }
  base->InsertExistingPageAt(memDoc->GetDocument(), memPageN, atN);
  return Number::New(info.Env(), base->GetPageCount());
}
Napi::Value
BaseDocument::GetInfo(const CallbackInfo& info)
{
  PdfInfo* i = base->GetInfo();
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
  vector<int> opts = AssertCallbackInfo(
    info,
    { { 0, { nullopt, option(napi_boolean), option(napi_string) } },
      { 1, { nullopt, option(napi_string) } } });

  PdfOutlineItem* outlines = nullptr;
  // Create with default options
  if (opts[0] == 0) {
    outlines = base->GetOutlines();
  } // Create with create option provided
  if (opts[0] == 1) {
    bool create = info[0].As<Boolean>();
    outlines = base->GetOutlines(create);
  }

  if (!outlines)
    return info.Env().Null();

  string root;
  if (opts[0] == 2) {
    root = info[0].As<String>().Utf8Value();
  }
  if (opts[1] == 1) {
    root = info[1].As<String>().Utf8Value();
  }
  if (!root.empty()) {
    dynamic_cast<PdfOutlines*>(outlines)->CreateRoot(PdfString(root));
  }

  return Outline::constructor.New(
    { External<PdfOutlineItem>::New(info.Env(), outlines),
      External<BaseDocument>::New(info.Env(), this) });
}
Napi::Value
BaseDocument::GetNamesTree(const CallbackInfo& info)
{
  auto names = base->GetNamesTree(info[0].As<Boolean>());
  if (!names)
    return info.Env().Null();
  return Obj::constructor.New(
    { External<PdfObject>::New(info.Env(), names->GetObject()) });
}
Napi::Value
BaseDocument::CreatePage(const CallbackInfo& info)
{
  auto r = Rect::Unwrap(info[0].As<Object>())->GetRect();
  auto page = base->CreatePage(r);
  return Page::constructor.New({ External<PdfPage>::New(info.Env(), page) });
}
Napi::Value
BaseDocument::CreatePages(const Napi::CallbackInfo& info)
{
  auto coll = info[0].As<Array>();
  vector<PdfRect> rects;
  for (uint32_t i = 0; i < coll.Length(); i++) {
    rects.emplace_back(
      Rect::Unwrap(coll.Get(static_cast<uint32_t>(i)).As<Object>())->GetRect());
  }
  base->CreatePages(rects);
  return Number::New(info.Env(), base->GetPageCount());
}
Napi::Value
BaseDocument::InsertPage(const Napi::CallbackInfo& info)
{
  auto rect = Rect::Unwrap(info[0].As<Object>())->GetRect();
  int index = info[1].As<Number>();
  base->InsertPage(rect, index);
  return Page::constructor.New(
    { External<PdfPage>::New(info.Env(), base->GetPage(index)) });
}

void
BaseDocument::Append(const Napi::CallbackInfo& info)
{
  if (info.Length() == 1 && info[0].IsArray()) {
    auto docs = info[0].As<Array>();
    for (unsigned int i = 0; i < docs.Length(); i++) {
      auto arg = docs.Get(i);
      if (arg.IsObject() &&
          arg.As<Object>().InstanceOf(Document::constructor.Value())) {
        auto mergedDoc = Document::Unwrap(arg.As<Object>());
        base->Append(mergedDoc->GetDocument());
      } else {
        TypeError::New(info.Env(),
                       "Only Document's can be appended, StreamDocument not "
                       "supported in append operation")
          .ThrowAsJavaScriptException();
        return;
      }
    }
  } else if (info.Length() == 1 && info[0].IsObject() &&
             info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
    auto mergedDoc = Document::Unwrap(info[0].As<Object>());
    base->Append(mergedDoc->GetDocument());
  }
}

Napi::Value
BaseDocument::GetAttachment(const CallbackInfo& info)
{
  string name = info[0].As<String>();
  if (base->GetAttachment(name)) {
    return FileSpec::constructor.New({ External<PdfObject>::New(
      info.Env(), base->GetAttachment(name)->GetObject()) });
  }
  PdfObject* embeddedFiles =
    base->GetNamesTree(false)->GetObject()->MustGetIndirectKey(
      Name::EMBEDDED_FILES);
  for (auto& i : embeddedFiles->MustGetIndirectKey(Name::KIDS)->GetArray()) {
    PdfObject* kid = nullptr;
    PdfObject* names = nullptr;
    PdfObject* filespec = nullptr;
    if (i.IsReference()) {
      kid = base->GetObjects()->GetObject(i.GetReference());
    } else if (i.IsDictionary()) {
      kid = &i;
    } else
      break;

    if (kid->GetDictionary().HasKey(Name::NAMES) ||
        !kid->MustGetIndirectKey(Name::NAMES)->IsArray()) {
      names = kid->MustGetIndirectKey(Name::NAMES);
    } else
      break;

    for (auto&& ii : names->GetArray()) {
      PdfObject* item;
      if (ii.IsReference()) {
        item = base->GetObjects()->GetObject(ii.GetReference());
      } else
        item = &ii;
      if (item->IsDictionary() && item->GetDictionary().HasKey(Name::TYPE) &&
          item->GetDictionary().GetKey(Name::TYPE)->GetName() ==
            Name::FILESPEC) {
        filespec = item;
        break;
      }
    }
    if (filespec && filespec->GetDictionary().HasKey(Name::UF)) {
      PdfObject* uf = filespec->MustGetIndirectKey(Name::UF);
      if (uf->IsString() && uf->GetString().GetStringUtf8() == name) {
        return FileSpec::constructor.New(
          { External<PdfObject>::New(info.Env(), filespec) });
      }
    }
  }

  return info.Env().Null();
}
void
BaseDocument::AddNamedDestination(const Napi::CallbackInfo& info)
{
  PdfPage page = Page::Unwrap(info[0].As<Object>())->page;
  EPdfDestinationFit fit =
    static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value());
  string name = info[2].As<String>().Utf8Value();
  auto destination = new PdfDestination(&page, fit);
  base->AddNamedDestination(*destination, name);
}

Napi::Value
BaseDocument::CreateXObject(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsObject() ||
      !info[0].As<Object>().InstanceOf(Rect::constructor.Value())) {
    TypeError::New(
      info.Env(),
      "CreateXObject requires an NoPoDoFo::Rect instance as the first argument")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  return XObject::constructor.New(
    { info[0].As<Object>(), External<PdfDocument>::New(info.Env(), base) });
}
Napi::Value
BaseDocument::GetForm(const CallbackInfo& info)
{
  Napi::Object instance =
    Form::constructor.New({ External<BaseDocument>::New(info.Env(), this),
                            Boolean::New(info.Env(), true) });
  return instance;
}
Napi::Value
BaseDocument::CreateFontSubset(const Napi::CallbackInfo& info)
{
  if (info.Length() < 1 || !info[0].IsObject()) {
    Error::New(info.Env(), "Invalid argument, expected an object")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  PdfFont* font = CreateFontObject(info.Env(), info[0].As<Object>(), true);
  return Font::constructor.New({ External<PdfFont>::New(info.Env(), font) });
}
PdfFont*
BaseDocument::CreateFontObject(napi_env env, Napi::Object opts, bool subset)
{
  HandleScope scope(env);
  if (!opts.Has("fontName")) {
    TypeError::New(env, "Requires fontName").ThrowAsJavaScriptException();
  }
  auto fontName = opts.Get("fontName").As<String>().Utf8Value();
  bool bold = opts.Has("bold") ? opts.Get("bold").As<Boolean>() : false;
  bool italic = opts.Has("italic") ? opts.Get("italic").As<Boolean>() : false;
  bool embed = opts.Has("embed") ? opts.Get("embed").As<Boolean>() : false;
  const PdfEncoding* encoding = nullptr;
  string filename =
    opts.Has("fileName") ? opts.Get("fileName").As<String>().Utf8Value() : "";
#ifndef PODOFO_HAVE_FONTCONFIG
  std::cout << "This build does not include fontconfig. To load a font you "
               "must provide the full path to the font file"
            << std::endl;
  if (filename == "") {
    Error::New(env,
               "This build requires any font creation to pass the full path to "
               "the font file")
      .ThrowAsJavaScriptException();
  }
#endif
  int n = opts.Has("encoding") ? opts.Get("encoding").As<Number>() : 1;
  switch (n) {
    case 1:
      //      encoding = new PdfWinAnsiEncoding();
      encoding = PdfEncodingFactory::GlobalWinAnsiEncodingInstance();
      break;
    case 2:
      //      encoding = new PdfStandardEncoding();
      encoding = PdfEncodingFactory::GlobalStandardEncodingInstance();
      break;
    case 3:
      //      encoding = new PdfDocEncoding();
      encoding = PdfEncodingFactory::GlobalPdfDocEncodingInstance();
      break;
    case 4:
      //      encoding = new PdfMacRomanEncoding();
      encoding = PdfEncodingFactory::GlobalMacRomanEncodingInstance();
      break;
    case 5:
      //      encoding = new PdfMacExpertEncoding();
      encoding = PdfEncodingFactory::GlobalMacExpertEncodingInstance();
      break;
    case 6:
      //      encoding = new PdfSymbolEncoding();
      encoding = PdfEncodingFactory::GlobalSymbolEncodingInstance();
      break;
    case 7:
      //      encoding = new PdfZapfDingbatsEncoding();
      encoding = PdfEncodingFactory::GlobalZapfDingbatsEncodingInstance();
      break;
    case 8:
      //      encoding = new PdfWin1250Encoding();
      encoding = PdfEncodingFactory::GlobalWin1250EncodingInstance();
      break;
    case 9:
      //      encoding = new PdfIso88592Encoding();
      encoding = PdfEncodingFactory::GlobalIso88592EncodingInstance();
      break;
    default:
      //      encoding = new PdfIdentityEncoding(0, 0xffff, true);
      encoding = PdfEncodingFactory::GlobalIdentityEncodingInstance();
  }
  try {
    PdfFont* font;
    if (subset) {
      font =
        base->CreateFontSubset(fontName.c_str(),
                               bold,
                               italic,
                               false,
                               encoding,
                               filename.empty() ? nullptr : filename.c_str());
    } else {
      font = base->CreateFont(fontName.c_str(),
                              bold,
                              italic,
                              false,
                              encoding,
                              PdfFontCache::eFontCreationFlags_AutoSelectBase14,
                              embed,
                              filename.empty() ? nullptr : filename.c_str());
    }
    return font;
  } catch (PdfError& err) {
    Error::New(env, err.what()).ThrowAsJavaScriptException();
    return {};
  }
}
}
