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

#include "Document.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "Encrypt.h"
#include "Font.h"
#include "Form.h"
#include "Page.h"
#include <iostream>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::string;

namespace NoPoDoFo {

FunctionReference Document::constructor; // NOLINT

void
Document::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Document",
    { StaticMethod("gc", &Document::GC),
      InstanceAccessor("password", nullptr, &Document::SetPassword),
      InstanceAccessor("encrypt", &Document::GetEncrypt, &Document::SetEncrypt),
      InstanceAccessor("form", &Document::GetForm, nullptr),
      InstanceAccessor("body", &Document::GetObjects, nullptr),
      InstanceAccessor("trailer", &Document::GetTrailer, nullptr),
      InstanceAccessor("catalog", &Document::GetCatalog, nullptr),
      InstanceAccessor("version", &Document::GetVersion, nullptr),
      InstanceAccessor(
        "pageMode", &Document::GetPageMode, &Document::SetPageMode),
      InstanceAccessor("pageLayout", nullptr, &Document::SetPageLayout),
      InstanceAccessor("printingScale", nullptr, &Document::SetPrintingScale),
      InstanceAccessor("baseURI", nullptr, &Document::SetBaseURI),
      InstanceAccessor("language", nullptr, &Document::SetLanguage),
      InstanceAccessor("info", &Document::GetInfo, nullptr),

      InstanceMethod("load", &Document::Load),
      InstanceMethod("getPageCount", &Document::GetPageCount),
      InstanceMethod("getPage", &Document::GetPage),
      InstanceMethod("splicePages", &Document::DeletePages),
      InstanceMethod("getFont", &Document::GetFont),
      InstanceMethod("hideToolbar", &Document::SetHideToolbar),
      InstanceMethod("hideMenubar", &Document::SetHideMenubar),
      InstanceMethod("hideWindowUI", &Document::SetHideWindowUI),
      InstanceMethod("fitWindow", &Document::SetFitWindow),
      InstanceMethod("centerWindow", &Document::SetCenterWindow),
      InstanceMethod("displayDocTitle", &Document::SetDisplayDocTitle),
      InstanceMethod("useFullScreen", &Document::SetUseFullScreen),
      InstanceMethod("attachFile", &Document::AttachFile),
      InstanceMethod("insertExistingPage", &Document::InsertExistingPage),
      InstanceMethod("insertPage", &Document::InsertPage),
      InstanceMethod("insertPages", &Document::InsertPages),
      InstanceMethod("append", &Document::Append),
      InstanceMethod("isLinearized", &Document::IsLinearized),
      InstanceMethod("getWriteMode", &Document::GetWriteMode),
      InstanceMethod("write", &Document::Write),
      InstanceMethod("getObject", &Document::GetObject),
      InstanceMethod("isAllowed", &Document::IsAllowed),
      InstanceMethod("createFont", &Document::CreateFont),
      InstanceMethod("getOutlines", &Document::GetOutlines),
      InstanceMethod("getNames", &Document::GetNamesTree),
      InstanceMethod("createPage", &Document::CreatePage),
      InstanceMethod("createPages", &Document::CreatePages),
      InstanceMethod("getAttachment", &Document::GetAttachment),
      InstanceMethod("addNamedDestination", &Document::AddNamedDestination),
      InstanceMethod("__ptrCount", &Document::GetSharedPtrCount) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Document", ctor);
}
Document::Document(const CallbackInfo& info)
  : ObjectWrap(info)
  , BaseDocument(info)
{
  document =
    std::static_pointer_cast<PdfMemDocument>(BaseDocument::GetBaseDocument());
}

/**
 * @note PdfFont resource is managed by the PdfDocument
 * @param info
 * @return
 */
Value
Document::GetFont(const CallbackInfo& info)
{
  try {
    auto id = info[0].As<String>().Utf8Value();
    vector<PdfObject*> fontObjs;
    vector<PdfFont*> fonts;
    for (auto item : document->GetObjects()) {
      if (item->IsDictionary()) {
        if (item->GetDictionary().HasKey(Name::TYPE) &&
            item->GetDictionary().GetKey(Name::TYPE)->IsName() &&
            item->GetDictionary().GetKey(Name::TYPE)->GetName().GetName() ==
              Name::FONT) {
          fontObjs.push_back(item);
        }
      }
      if (item->IsReference()) {
        auto ref = document->GetObjects().GetObject(item->GetReference());
        if (ref->IsDictionary()) {
          if (ref->GetDictionary().HasKey(Name::TYPE) &&
              ref->GetDictionary().GetKey(Name::TYPE)->IsName() &&
              ref->GetDictionary().GetKey(Name::TYPE)->GetName().GetName() ==
                Name::FONT) {
            fontObjs.push_back(ref);
          }
        }
      }
    }
    for (auto o : fontObjs) {
      auto font = document->GetFont(o);
      if (!font) {
        continue;
      } else {
        fonts.push_back(font);
      }
    }
    for (auto item : fonts) {
      string itemId = item->GetIdentifier().GetName();
      string itemName = item->GetFontMetrics()->GetFontname();
      cout << "Font Identifier: " << itemId << endl;
      cout << "Font Name: " << itemName << endl;
      if (itemId == id || itemName == id) {
        EscapableHandleScope scope(info.Env());
        return scope.Escape(
          Font::constructor.New({ External<PdfFont>::New(info.Env(), item) }));
      }
    }
    return info.Env().Null();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}

void
Document::SetPassword(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty() || !value.IsString()) {
    throw Napi::Error::New(info.Env(), "password must be of type string");
  }
  string password = value.As<String>().Utf8Value();
  try {
    document->SetPassword(password);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
    ErrorHandler(err, info);
  }
}

void
Document::DeletePages(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { { napi_number, napi_number } }, nullptr);
  int pageIndex = info[0].As<Number>();
  int count = info[1].As<Number>();
  if (document->GetPageCount() < pageIndex + count) {
    RangeError::New(info.Env(), "Pages out of range")
      .ThrowAsJavaScriptException();
  }
  try {
    document->DeletePages(pageIndex, count);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Document::SetEncrypt(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsExternal()) {
    TypeError::New(
      info.Env(),
      "Requires an External<PdfEncrypt> please see the docs for more info.")
      .ThrowAsJavaScriptException();
    return;
  }
  try {
    const PdfEncrypt* e = value.As<External<PdfEncrypt>>().Data();
    document->SetEncrypted(*e);
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PdfMemDocument::SetEncrypt failed with error: " << err.GetError()
        << endl;
    throw Error::New(info.Env(), msg.str());
  }
}
Napi::Value
Document::GetTrailer(const CallbackInfo& info)
{
  const PdfObject* trailerPdObject = document->GetTrailer();
  auto ptr = const_cast<PdfObject*>(trailerPdObject);
  auto initPtr = Napi::External<PdfObject>::New(
    info.Env(), ptr);
  auto instance = Obj::constructor.New({ initPtr });
  return instance;
}

Napi::Value
Document::GetCatalog(const CallbackInfo& info)
{
  const PdfObject* catalog = document->GetCatalog();
  auto ptr = const_cast<PdfObject*>(catalog);
  auto initPtr = Napi::External<PdfObject>::New(
    info.Env(), ptr);
  auto instance = Obj::constructor.New({ initPtr });
  return instance;
}

class DocumentWriteAsync : public AsyncWorker
{
public:
  DocumentWriteAsync(Napi::Function& cb, Document& doc, string arg)
    : Napi::AsyncWorker(cb)
    , doc(doc)
    , arg(std::move(arg))
  {}

private:
  Document& doc;
  string arg = "";

protected:
  void Execute() override
  {
    try {
      PdfOutputDevice device(arg.c_str());
      doc.GetMemDocument()->Write(&device);
    } catch (PdfError& err) {
      SetError(String::New(Env(), ErrorHandler::WriteMsg(err)));
    } catch (Napi::Error& err) {
      SetError(String::New(Env(), ErrorHandler::WriteMsg(err)));
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), arg) });
  }
};

class DocumentLoadAsync : public AsyncWorker
{
public:
  DocumentLoadAsync(Function& cb,
                    Document& doc,
                    string arg,
                    PdfRefCountedInputDevice* refBuffer)
    : AsyncWorker(cb)
    , doc(doc)
    , arg(std::move(arg))
    , refBuffer(refBuffer)
  {}

  void ForUpdate(bool v) { update = v; }
  void SetPassword(string v) { pwd = std::move(v); }
  void SetUseBuffer(bool v) { useBuffer = v; }

private:
  Document& doc;
  PdfRefCountedInputDevice* refBuffer;
  string arg;
  string pwd;
  bool update = false;
  bool useBuffer = false;

  // AsyncWorker interface
protected:
  void Execute() override
  {
    try {
      if (!useBuffer)
        doc.GetMemDocument()->Load(arg.c_str(), update);
      else {
#if PODOFO_VERSION_MINOR >= 9 && PODOFO_VERSION_PATCH >= 6
        doc.GetMemDocument()->LoadFromDevice(*refBuffer);
#else
        Error::New(Env(), "This podofo build does not support loading a buffered document").ThrowAsJavascriptException();
        return;
#endif
      }
    } catch (PdfError& e) {
      if (e.GetError() == ePdfError_InvalidPassword) {
        cout << "password missing" << endl;
        if (pwd.empty())
          SetError("Password required to modify this document");
        else {
          try {
            doc.GetMemDocument()->SetPassword(pwd);
            cout << "password set" << endl;
          } catch (PdfError& err) {
            cout << "Invalid password" << endl;
            stringstream msg;
            msg << "Invalid password.\n" << err.what() << endl;
            SetError(msg.str());
          }
        }
      } else {
        SetError(ErrorHandler::WriteMsg(e));
      }
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), arg) });
  }
};

/**
 * @details Javascript parameters: (file: string|Buffer, opts: {update: boolean
 * = false, isBuffer, pwd?: string}, cb:Function)
 * @param info
 * @return
 */
Napi::Value
Document::Load(const CallbackInfo& info)
{
  Function cb;
  bool forUpdate = false, useBuffer = false;
  string source, pwd;
  PdfRefCountedInputDevice* inputDevice = nullptr;
  if (info.Length() >= 2 && info[1].IsFunction()) {
    cb = info[1].As<Function>();
  } else if (info.Length() >= 2 && info[1].IsObject()) {
    auto opts = info[1].As<Object>();
    if (opts.Has("forUpdate")) {
      forUpdate = opts.Get("forUpdate").As<Boolean>();
    }
    if (opts.Has("isBuffer")) {
      useBuffer = opts.Get("isBuffer").As<Boolean>();
    }
    if (opts.Has("pwd")) {
      pwd = opts.Get("pwd").As<String>().Utf8Value();
    }
  } else {
    TypeError::New(
      info.Env(),
      "Expected an options object or callback function be received neither")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  if (info.Length() == 3 && info[2].IsFunction()) {
    cb = info[2].As<Function>();
  }
  if (useBuffer) {
    auto buffer = info[0].As<Buffer<char>>();
    inputDevice = new PdfRefCountedInputDevice(buffer.Data(), buffer.Length());
  } else {
    source = info[0].As<String>().Utf8Value();
  }

  loadForIncrementalUpdates = forUpdate;
  DocumentLoadAsync* worker =
    new DocumentLoadAsync(cb, *this, source, inputDevice);

  worker->SetPassword(pwd);
  worker->ForUpdate(forUpdate);
  worker->SetUseBuffer(useBuffer);
  worker->Queue();
  return info.Env().Undefined();
}

class DocumentWriteBufferAsync : public AsyncWorker
{
public:
  DocumentWriteBufferAsync(Function& cb, Document& doc)
    : AsyncWorker(cb)
    , doc(doc)
  {}

private:
  Document& doc;
  PdfRefCountedBuffer output;

protected:
  void Execute() override
  {
    PdfOutputDevice device(&output);
    doc.GetMemDocument()->Write(&device);
  }
  void OnOK() override
  {
    auto env = Env();
    HandleScope scope(env);
    if (output.GetSize() == 0) {
      SetError("Error, failed to write to buffer");
    }
    Callback().Call({ Env().Null(),
                      Buffer<char>::Copy(
                        scope.Env(), output.GetBuffer(), output.GetSize()) });
  }
};

Napi::Value
Document::Write(const CallbackInfo& info)
{
  try {
    if (info[0].IsFunction()) {
      auto cb = info[0].As<Function>();
      auto* worker = new DocumentWriteBufferAsync(cb, *this);
      worker->Queue();
      return info.Env().Undefined();
    } else if (info.Length() == 2 && info[0].IsString() &&
               info[1].IsFunction()) {
      string arg = info[0].As<String>();
      auto cb = info[1].As<Function>();
      DocumentWriteAsync* worker = new DocumentWriteAsync(cb, *this, arg);
      worker->Queue();
    } else {
      throw Error::New(
        info.Env(),
        String::New(info.Env(), "Requires at least a callback argument"));
    }
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Napi::Error& err) {
    ErrorHandler(err, info);
  }
  return Env().Undefined();
}

class GCAsync : public AsyncWorker
{
public:
  GCAsync(const Function& callback, string doc, string pwd, string output)
    : AsyncWorker(callback)
    , doc(std::move(doc))
    , pwd(std::move(pwd))
    , output(std::move(output))
    , size(0)
  {}

protected:
  void Execute() override
  {
    PdfVecObjects vecObjects;
    PdfParser parser(&vecObjects);
    vecObjects.SetAutoDelete(true);
    parser.ParseFile(doc.c_str(), false);
    PdfWriter writer(&parser);
    writer.SetPdfVersion(parser.GetPdfVersion());
    if (parser.GetEncrypted()) {
      writer.SetEncrypted(*(parser.GetEncrypt()));
    }
    if (output.empty()) {
      PdfRefCountedBuffer r;
      PdfOutputDevice device(&r);
      writer.Write(&device);
      value = string(r.GetBuffer());
      size = r.GetSize();
    } else {
      writer.SetWriteMode(ePdfWriteMode_Compact);
      writer.Write(output.c_str());
      size = 0;
      value = output;
    }
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    if (size > 0) {
      auto buffer =
        Buffer<char>::Copy(Env(), value.c_str(), static_cast<size_t>(size));
      Callback().Call({ Env().Null(), buffer });
    }
    Callback().Call({ Env().Null(), String::New(Env(), value) });
  }

private:
  string doc;
  string pwd;
  string output;
  string value;
  long size;
};

Napi::Value
Document::GC(const Napi::CallbackInfo& info)
{
  string source = info[0].As<String>().Utf8Value(), pwd, output;
  pwd = info[1].IsString() ? info[1].As<String>().Utf8Value() : "";
  output = info[2].IsString() ? info[2].As<String>().Utf8Value() : "";
  auto cb = info[3].As<Function>();
  auto worker = new GCAsync(cb, source, pwd, output);
  worker->Queue();
  return info.Env().Undefined();
}

Napi::Value
Document::CreatePage(const Napi::CallbackInfo& info)
{
  return BaseDocument::CreatePage(info);
}
Napi::Value
Document::GetSharedPtrCount(const Napi::CallbackInfo& info)
{
  return Number::New(info.Env(), BaseDocument::GetBaseDocument().use_count());
}
Napi::Value
Document::GetEncrypt(const Napi::CallbackInfo& info)
{
  auto enc = document->GetEncrypt();
  if (!enc) {
    return info.Env().Null();
  }
  return Encrypt::constructor.New(
    { External<PdfEncrypt>::New(info.Env(), const_cast<PdfEncrypt*>(enc)) });
}
Napi::Value
Document::InsertPages(const Napi::CallbackInfo& info)
{
  if (info.Length() < 3) {
    Error::New(info.Env(), "Expected args: IDocument, number, number")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  if (!info[0].IsObject() ||
      !info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
    TypeError::New(info.Env(), "Requires an instance of Document")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  if (!info[1].IsNumber() || !info[2].IsNumber()) {
    TypeError::New(info.Env(), "Requires a start and end range")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  auto pagesDoc = Document::Unwrap(info[0].As<Object>())->GetMemDocument();
  int start = info[1].As<Number>();
  int end = info[2].As<Number>();
  document->InsertPages(pagesDoc.get(), start, end);
  return Number::New(info.Env(), document->GetPageCount());
}
}
