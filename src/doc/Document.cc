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
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "Encrypt.h"
#include "Font.h"
#include "Form.h"
#include "Page.h"
#include "SignatureField.h"
#include <iostream>
#include <sstream>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::experimental::string_view;

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
      InstanceAccessor("language", nullptr, &Document::SetLanguage),
      InstanceAccessor("info", &Document::GetInfo, nullptr),
      InstanceMethod("setPassword", &Document::SetPassword),
      InstanceMethod("hasSignatures", &Document::HasSignature),
      InstanceMethod("getSignatures", &Document::GetSignatures),
      InstanceMethod("load", &Document::Load),
      InstanceMethod("getPageCount", &Document::GetPageCount),
      InstanceMethod("getPage", &Document::GetPage),
      InstanceMethod("splicePages", &Document::DeletePages),
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
      InstanceMethod("createFontSubset", &Document::CreateFontSubset),
      InstanceMethod("getOutlines", &Document::GetOutlines),
      InstanceMethod("getNames", &Document::GetNamesTree),
      InstanceMethod("createPage", &Document::CreatePage),
      InstanceMethod("createPages", &Document::CreatePages),
      InstanceMethod("createXObject", &Document::CreateXObject),
      InstanceMethod("getAttachment", &Document::GetAttachment),
      InstanceMethod("getFont", &Document::GetFont),
      InstanceMethod("listFonts", &Document::ListFonts),
      InstanceMethod("addNamedDestination", &Document::AddNamedDestination) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Document", ctor);
}
Document::Document(const CallbackInfo& info)
  : ObjectWrap(info)
  , BaseDocument(info, true)
{
  cout << "Memory Document" << endl;
}

void
Document::SetPassword(const CallbackInfo& info)
{
  auto value = info[0];
  if (value.IsEmpty() || !value.IsString()) {
    throw Napi::Error::New(info.Env(), "password must be of type string");
  }
  string password = value.As<String>().Utf8Value();
  try {
    GetDocument().SetPassword(password);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
    ErrorHandler(err, info);
  }
}
Napi::Value
Document::ListFonts(const CallbackInfo& info)
{
  fonts.clear();
  this->fonts = Document::GetFonts(GetDocument());
  auto list = Array::New(info.Env());
  uint32_t n = 0;
  for (auto item : fonts) {
    string itemId = item->GetIdentifier().GetName();
    string itemName = item->GetFontMetrics()->GetFontname();
    auto v = Object::New(info.Env());
    v.Set("id", String::New(info.Env(), itemId));
    v.Set("name", String::New(info.Env(), itemName));
    v.Set("file", String::New(info.Env(), item->GetFontMetrics()->GetFilename()));
    list.Set(n, v);
    n++;
  }
  return list;
}

PoDoFo::PdfFont*
Document::GetPdfFont(PdfMemDocument &doc, string_view id)
{
  vector<PdfFont*> fonts = Document::GetFonts(doc);
    for (auto item : fonts) {
    string itemId = item->GetIdentifier().GetName();
    string itemName = item->GetFontMetrics()->GetFontname();
    if (itemId == id || itemName == id) {
      if (item->IsSubsetting()) {
        cout << "WARNING: This is a font subset" << endl;
      }
      return item;
    }
  }
  return nullptr;
}
Value
Document::GetFont(const CallbackInfo& info)
{
  try {
    auto id = info[0].As<String>().Utf8Value();
    if (fonts.empty()) {
      this->fonts = Document::GetFonts(GetDocument());
    }
    for (auto item : fonts) {
      string itemId = item->GetIdentifier().GetName();
      string itemName = item->GetFontMetrics()->GetFontname();
      if (itemId == id || itemName == id) {
        //        EscapableHandleScope scope(info.Env());
        //        return scope.Escape(
        //          Font::constructor.New({ External<PdfFont>::New(info.Env(),
        //          item) }));
        if (item->IsSubsetting()) {
          cout << "WARNING: This is a font subset" << endl;
        }
        return Font::constructor.New(
          { External<PdfFont>::New(info.Env(), item) });
      }
    }
    return info.Env().Null();
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
  return info.Env().Undefined();
}
vector<PdfFont*>
Document::GetFonts(PdfMemDocument& doc)
{
  vector<PdfObject*> fontObjs;
  vector<PdfFont*> fonts;
  for (auto item : doc.GetObjects()) {
    if (item->IsDictionary()) {
      if (item->GetDictionary().HasKey(Name::TYPE) &&
          item->GetDictionary().GetKey(Name::TYPE)->IsName() &&
          item->GetDictionary().GetKey(Name::TYPE)->GetName().GetName() ==
            Name::FONT) {
        fontObjs.push_back(item);
      }
    }
    if (item->IsReference()) {
      auto ref = doc.GetObjects().GetObject(item->GetReference());
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
    auto font = doc.GetFont(o);
    if (!font) {
      continue;
    } else {
      fonts.push_back(font);
    }
  }
  return fonts;
}

void
Document::DeletePages(const CallbackInfo& info)
{
  int pageIndex = info[0].As<Number>();
  int count = info[1].As<Number>();
  if (GetDocument().GetPageCount() < pageIndex + count || pageIndex < 0) {
    RangeError::New(info.Env(), "Pages out of range")
      .ThrowAsJavaScriptException();
  }
  try {
    GetDocument().DeletePages(pageIndex, count);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
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
    GetDocument().SetEncrypted(*e);
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
  const PdfObject* trailerPdObject = GetDocument().GetTrailer();
  auto ptr = const_cast<PdfObject*>(trailerPdObject);
  auto initPtr = Napi::External<PdfObject>::New(info.Env(), ptr);
  auto instance = Obj::constructor.New({ initPtr });
  return instance;
}

Napi::Value
Document::GetCatalog(const CallbackInfo& info)
{
  PdfObject* catalog = GetDocument().GetCatalog();
  auto initPtr = Napi::External<PdfObject>::New(info.Env(), catalog);
  auto instance = Obj::constructor.New({ initPtr });
  return instance;
}

class DocumentWriteAsync : public AsyncWorker
{
public:
  DocumentWriteAsync(Napi::Function& cb, Document& doc, string arg)
    : Napi::AsyncWorker(cb, "document_write_async", doc.Value())
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
      doc.GetDocument().Write(&device);
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

class DocumentLoadBufferAsync : public AsyncWorker
{
public:
  DocumentLoadBufferAsync(Function& cb,
                          Document& doc,
                          PdfRefCountedInputDevice* input,
                          bool forUpdate,
                          string pwd)
    : AsyncWorker(cb, "document_load_buffer_async", doc.Value())
    , doc(doc)
    , data(input)
    , forUpdate(forUpdate)
    , pwd(std::move(pwd))
  {}
  ~DocumentLoadBufferAsync() { delete data; }

private:
  Document& doc;
  PdfRefCountedInputDevice* data;
  bool forUpdate;
  string pwd;

protected:
  void Execute() override
  {
    TryLoad(doc.GetDocument(),
            string(""),
            *data,
            pwd,
            forUpdate,
            DocumentInputDevice::Memory);
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), doc.Value() });
  }
};

class DocumentLoadAsync : public AsyncWorker
{
public:
  DocumentLoadAsync(Function& cb,
                    Document& doc,
                    string arg,
                    bool forUpdate,
                    string pwd)
    : AsyncWorker(cb, "document_load_async", doc.Value())
    , doc(doc)
    , arg(std::move(arg))
    , pwd(std::move(pwd))
    , forUpdate(forUpdate)
  {}

private:
  Document& doc;
  string arg;
  string pwd;
  bool forUpdate;

  // AsyncWorker interface
protected:
  void Execute() override
  {
    TryLoad(doc.GetDocument(),
            arg,
            nullptr,
            pwd,
            forUpdate,
            DocumentInputDevice::Disk);
  }
  void OnOK() override
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), doc.Value() });
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
  if (info.Length() < 2) {
    Error::New(info.Env(), "Requires a minimum of 2 argument")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  Function cb;
  bool forUpdate = false;
  AsyncWorker* worker;
  string pwd;

  if (info.Length() >= 2 && info[1].IsObject()) {
    auto opts = info[1].As<Object>();
    if (opts.Has("forUpdate")) {
      forUpdate = opts.Get("forUpdate").As<Boolean>();
    }
    if (opts.Has("password")) {
      pwd = opts.Get("password").As<String>().Utf8Value();
    }
  }
  if (!info[info.Length() - 1].IsFunction()) {
    Error::New(info.Env(), "Last argument must be a callback function")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  cb = info[info.Length() - 1].As<Function>();

  if (info[0].IsBuffer()) {
    auto device = new PdfRefCountedInputDevice(
      info[0].As<Buffer<char>>().Data(), info[0].As<Buffer<char>>().Length());
    worker = new DocumentLoadBufferAsync(cb, *this, device, forUpdate, pwd);
  } else if (info[0].IsString()) {
    worker = new DocumentLoadAsync(
      cb, *this, info[0].As<String>().Utf8Value(), forUpdate, pwd);
  } else {
    TypeError::New(info.Env(),
                   "1st argument must be the file path, or the file buffer")
      .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  loadForIncrementalUpdates = forUpdate;
  worker->Queue();

  return info.Env().Undefined();
}

class DocumentWriteBufferAsync : public AsyncWorker
{
public:
  DocumentWriteBufferAsync(Function& cb, Document& doc)
    : AsyncWorker(cb, "document_write_buffer_async", doc.Value())
    , doc(doc)
  {}

private:
  Document& doc;
  PdfRefCountedBuffer output;

protected:
  void Execute() override
  {
    PdfOutputDevice device(&output);
    doc.GetDocument().Write(&device);
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
    : AsyncWorker(callback, "gc_async")
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
Document::GetEncrypt(const Napi::CallbackInfo& info)
{
  auto enc = GetDocument().GetEncrypt();
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
  auto pagesDoc = &Document::Unwrap(info[0].As<Object>())->GetDocument();
  int start = info[1].As<Number>();
  int end = info[2].As<Number>();
  GetDocument().InsertPages(pagesDoc, start, end);
  return Number::New(info.Env(), GetDocument().GetPageCount());
}

Napi::Value
Document::HasSignature(const CallbackInfo& info)
{
  for (int i = 0; i < base->GetPageCount(); i++) {
    auto page = base->GetPage(i);
    for (int j = 0; j < page->GetNumFields(); j++) {
      auto field = page->GetField(j);
      if (field.GetType() == ePdfField_Signature) {
        return Boolean::New(info.Env(), true);
      }
    }
  }
  return Boolean::New(info.Env(), false);
}
Napi::Value
Document::GetSignatures(const CallbackInfo& info)
{
  auto js = Array::New(info.Env());
  uint32_t jsIndex = 0;
  for (int i = 0; i < base->GetPageCount(); i++) {
    auto page = base->GetPage(i);
    for (int j = 0; j < page->GetNumFields(); j++) {
      auto field = page->GetField(j);
      if (field.GetType() == ePdfField_Signature) {
        cout << "Found field " << field.GetFieldName().GetStringUtf8() << endl;
        js.Set(jsIndex,
               SignatureField::constructor.New({ External<PdfAnnotation>::New(
                 info.Env(), field.GetWidgetAnnotation()) }));
        jsIndex++;
      }
    }
  }

  return js;
}
}
