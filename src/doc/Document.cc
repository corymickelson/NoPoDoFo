//
// Created by red on 9/6/17.
//

#include "Document.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Obj.h"
#include "Encrypt.h"
#include "Page.h"

using namespace Napi;
using namespace std;
using namespace PoDoFo;

void
Document::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Document",
    { InstanceAccessor("password", nullptr, &Document::SetPassword),
      InstanceAccessor("encrypt", &Document::GetEncrypt, &Document::SetEncrypt),
      InstanceMethod("load", &Document::Load),
      InstanceMethod("getPageCount", &Document::GetPageCount),
      InstanceMethod("getPage", &Document::GetPage),
      InstanceMethod("mergeDocument", &Document::MergeDocument),
      InstanceMethod("deletePage", &Document::DeletePage),
      InstanceMethod("getVersion", &Document::GetVersion),
      InstanceMethod("isLinearized", &Document::IsLinearized),
      InstanceMethod("getWriteMode", &Document::GetWriteMode),
      InstanceMethod("createEncrypt", &Document::CreateEncrypt),
      InstanceMethod("write", &Document::Write),
      InstanceMethod("writeBuffer", &Document::WriteBuffer),
      InstanceMethod("getObjects", &Document::GetObjects),
      InstanceMethod("getTrailer", &Document::GetTrailer) });

  target.Set("Document", ctor);
}
Document::Document(const CallbackInfo& info)
  : ObjectWrap(info)
{
  document = new PdfMemDocument();
}

Napi::Value
Document::LoadBuffer(const CallbackInfo& info)
{
  auto resolver = Promise::Deferred::New(info.Env());
  if (!info[0].IsBuffer()) {
    throw Error::New(info.Env(), "Buffer required");
  }
  auto buffer = info[0].As<Buffer<char>>();
  document->LoadFromBuffer(buffer.Data(), buffer.Length());
  resolver.Resolve(info.Env().Undefined());
  return resolver.Promise();
}

Napi::Value
Document::GetPageCount(const CallbackInfo& info)
{
  int pages = document->GetPageCount();
  return Napi::Number::New(info.Env(), pages);
}

Napi::Value
Document::GetPage(const CallbackInfo& info)
{
  try {
    if (info.Length() != 1 || !info[0].IsNumber()) {
      throw Napi::Error::New(info.Env(),
                             "getPage takes an argument of 1, of type number.");
    }
    int n = info[0].As<Number>();
    int pl = document->GetPageCount();
    if (n > pl) {
      throw Napi::RangeError::New(info.Env(), "Index out of page count range");
    }
    PdfPage* page = document->GetPage(n);
    auto pagePtr = Napi::External<PdfPage>::New(info.Env(), page);
    auto docPtr = Napi::External<PdfMemDocument>::New(info.Env(), document);
    auto instance = Page::constructor.New({ pagePtr, docPtr });
    return instance;
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
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

Napi::Value
Document::GetVersion(const CallbackInfo& info)
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
    throw Napi::Error::New(info.Env(),
                           "Failed to parse document. Pdf version unknown.");
  }
  return Napi::Number::New(info.Env(), v);
}

Napi::Value
Document::IsLinearized(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), document->IsLinearized());
}

void
Document::DeletePage(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int pageIndex = info[0].As<Number>();
  if (pageIndex < 0 || pageIndex > document->GetPageCount()) {
    throw Napi::Error::New(info.Env(), "Page index out of range");
  }
  try {
    document->GetPagesTree()->DeletePage(pageIndex);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

void
Document::MergeDocument(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string docPath = info[0].As<String>().Utf8Value();
  PdfMemDocument mergedDoc;
  try {
    mergedDoc.Load(docPath.c_str());
  } catch (PdfError& err) {
    if (err.GetError() == ePdfError_InvalidPassword && info.Length() != 2) {
      throw Napi::Error::New(info.Env(),
                             "Password required to modify this document. Call "
                             "MergeDocument(filePath, password)");
    } else if (err.GetError() == ePdfError_InvalidPassword &&
               info.Length() == 2 && info[1].IsString()) {
      string password = info[1].As<String>().Utf8Value();
      mergedDoc.SetPassword(password);
    }
  }

  document->Append(mergedDoc);
}

Napi::Value
Document::GetWriteMode(const CallbackInfo& info)
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

void
Document::SetEncrypt(const CallbackInfo& info, const Napi::Value& value)
{
  try {
    AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
    if (!value.IsObject()) {
      throw Napi::Error::New(info.Env(),
                             "Set encrypt requires a single argument of"
                             " type Object<{userPassword:string,"
                             " ownerPassword:string, protection:Array<string>,"
                             " algorithm: string, keyLength: int");
    }
    auto encryption = value.As<Object>();
    string ownerPwd;
    string userPwd;
    int permParameter = 0;
    int algoParameter = 0;
    int key = 0;
    ParseJsEncryptObj(
      info, encryption, ownerPwd, userPwd, permParameter, algoParameter, key);
    document->SetEncrypted(
      userPwd,
      ownerPwd,
      permParameter,
      static_cast<PdfEncrypt::EPdfEncryptAlgorithm>(algoParameter),
      static_cast<PdfEncrypt::EPdfKeyLength>(key));
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PdfMemDocument::SetEncrypt failed with error: " << err.GetError()
        << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

Napi::Value
Document::GetEncrypt(const CallbackInfo& info)
{
  const PdfEncrypt* enc = document->GetEncrypt();
  auto ptr = const_cast<PdfEncrypt*>(enc);
  auto encryptPtr = Napi::External<PdfEncrypt>::New(info.Env(), ptr);
  auto instance = Encrypt::constructor.New({ encryptPtr });
  return instance;
}

Napi::Value
Document::CreateEncrypt(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto initObj = info[0].As<Object>();
  string uPwd, oPwd;
  int perm, algo, key;
  ParseJsEncryptObj(info, initObj, oPwd, uPwd, perm, algo, key);
  static PdfEncrypt* enc = PdfEncrypt::CreatePdfEncrypt(
    uPwd,
    oPwd,
    perm,
    static_cast<PdfEncrypt::EPdfEncryptAlgorithm>(algo),
    static_cast<PdfEncrypt::EPdfKeyLength>(key));
  auto ptr = Napi::External<PdfEncrypt>::New(info.Env(), enc);
  auto instance = Encrypt::constructor.New({ ptr });
  return instance;
}

Napi::Value
Document::GetObjects(const CallbackInfo& info)
{
  auto js = Napi::Array::New(info.Env());
  auto it = document->GetObjects().begin();
  int count = 0;
  while (it != document->GetObjects().end()) {
    auto instance = External<PdfObject>::New(info.Env(), (*it));
    js.Set(Number::New(info.Env(), count), Obj::constructor.New({ instance }));
    ++it;
    ++count;
  }
  return js;
}

Napi::Value
Document::GetTrailer(const CallbackInfo& info)
{
  const PdfObject* trailerPdObject = document->GetTrailer();
  auto* ptr = const_cast<PdfObject*>(trailerPdObject);
  auto initPtr = Napi::External<PdfObject>::New(info.Env(), ptr);
  auto instance = Obj::constructor.New({ initPtr });
  return instance;
}

Value
Document::IsAllowed(const CallbackInfo& info)
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
  return Boolean::New(info.Env(), is);
}

void
Document::ParseJsEncryptObj(const CallbackInfo& info,
                            Object& encryption,
                            string& ownerPwd,
                            string& userPwd,
                            int& permParameter,
                            int& algoParameter,
                            int& key)
{
  try {
    if (encryption.Has("ownerPassword")) {
      ownerPwd = encryption.Get("ownerPassword").As<String>().Utf8Value();
    }
    if (encryption.Has("userPassword")) {
      userPwd = encryption.Get("userPassword").As<String>().Utf8Value();
    }
    if (encryption.Has("protection")) {
      if (encryption.IsArray()) {
        auto permissions = encryption.Get("protection").As<Array>();
        if (!permissions.IsEmpty()) {
          vector<int> p;
          for (uint32_t i = 0; i < permissions.Length(); ++i) {
            if (permissions.Get(i).IsString()) {
              string permission = permissions.Get(i).As<String>().Utf8Value();
              if (permission == "Copy")
                p.push_back(0x00000010);
              else if (permission == "Print")
                p.push_back(0x00000004);
              else if (permission == "Edit")
                p.push_back(0x00000008);
              else if (permission == "EditNotes")
                p.push_back(0x00000020);
              else if (permission == "FillAndSign")
                p.push_back(0x00000100);
              else if (permission == "Accessible")
                p.push_back(0x00000200);
              else if (permission == "DocAssembly")
                p.push_back(0x00000400);
              else if (permission == "HighPrint")
                p.push_back(0x00000800);
              else {
                stringstream msg;
                msg << "Unknown permission parameter: " << permission
                    << ". Permission must be one or more of: "
                    << "[Copy, Print, Edit, EditNotes, FillAndSign, "
                       "Accessible, DocAssembly, HighPrint]"
                    << endl;
                throw Napi::Error::New(info.Env(), msg.str());
              }
            }
          }
          for (auto pi : p) {
            permParameter |= pi;
          }
        }
      }
    }
    if (encryption.Has("algorithm")) {
      // rc4v1 =1 rc4v2 = 2 aesv2 = 4 aesv3 = 8
      Napi::Value algoProp = encryption.Get("algorithm");
      string algo;
      if (algoProp.IsString()) {
        algo = algoProp.As<String>().Utf8Value();
        if (algo == "rc4v1")
          algoParameter = 1;
        else if (algo == "rc4v2")
          algoParameter = 2;
        else if (algo == "aesv2")
          algoParameter = 4;
        else if (algo == "aesv3")
          algoParameter = 8;
        else {
          stringstream msg;
          msg << "Unknown permission parameter: " << algo
              << ". Permission must be one or more of: [rc4v1, rc4v2, aesv2, "
                 "aesv3]"
              << endl;
          throw Napi::Error::New(info.Env(), msg.str());
        }
      }
    }
    if (encryption.Has("keyLength")) {
      // 40 56 80 96 128 256
      int keyValues[6] = { 40, 56, 80, 96, 128, 256 };
      Napi::Value keyProp = encryption.Get("keyLength");
      if (keyProp.IsNumber()) {
        key = keyProp.As<Number>();
        for (int i = 0; i < 6; ++i) {
          if (keyValues[i] == key)
            break;
          if (keyValues[i] != key && i == 6) {
            stringstream msg;
            msg << "Unknown permission parameter: " << key
                << ". Permission must be one or more of: [40, 56, 80, 96, 128, "
                   "256]"
                << endl;
            throw Napi::Error::New(info.Env(), msg.str());
          }
        }
      }
    }
  } catch (PdfError& err) {
    stringstream msg;
    msg << "Parse Encrypt Object failed with error: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

class DocumentWriteAsync : public Napi::AsyncWorker
{
public:
  DocumentWriteAsync(Napi::Function& cb, Document* doc, string arg)
    : Napi::AsyncWorker(cb)
    , doc(doc)
    , arg(std::move(arg))
  {}
  ~DocumentWriteAsync() {}

private:
  Document* doc;
  string arg = "";

  // AsyncWorker interface
protected:
  void Execute()
  {
    try {
      PdfOutputDevice device(arg.c_str());
      doc->GetDocument()->Write(&device);
    } catch (PdfError& err) {
      SetError(String::New(Env(), ErrorHandler::WriteMsg(err)));
    } catch (Napi::Error& err) {
      SetError(String::New(Env(), ErrorHandler::WriteMsg(err)));
    }
  }
  void OnOK()
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), arg) });
  }
};

Napi::Value
Document::Write(const CallbackInfo& info)
{
  try {
    if (info.Length() == 2 && info[0].IsString() && info[1].IsFunction()) {
      string arg = info[0].As<String>();
      auto cb = info[1].As<Function>();
      DocumentWriteAsync* worker = new DocumentWriteAsync(cb, this, arg);
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

class DocumentLoadAsync : public Napi::AsyncWorker
{
public:
  DocumentLoadAsync(Napi::Function& cb, Document* doc, string arg)
    : AsyncWorker(cb)
    , doc(doc)
    , arg(std::move(arg))
  {}
  ~DocumentLoadAsync() {}

  void ForUpdate(bool v) { update = v; }

private:
  Document* doc;
  string arg;
  bool update = false;

  // AsyncWorker interface
protected:
  void Execute()
  {
    try {
      if (arg.empty()) {
        throw Napi::Error::New(Env(), "null destination path");
      }
      doc->GetDocument()->Load(arg.c_str());
    } catch (PdfError& err) {
      SetError(ErrorHandler::WriteMsg(err));
    } catch (Napi::Error& err) {
      SetError(ErrorHandler::WriteMsg(err));
    }
  }
  void OnOK()
  {
    HandleScope scope(Env());
    Callback().Call({ Env().Null(), String::New(Env(), arg) });
  }
};

Napi::Value
Document::Load(const CallbackInfo& info)
{
  try {
    AssertFunctionArgs(
      info, 2, { napi_valuetype::napi_string, napi_valuetype::napi_function });
    string filePath = info[0].As<String>().Utf8Value();
    auto cb = info[1].As<Function>();
    bool forUpdate = false;
    if (info.Length() == 3) {
      forUpdate = info[2].As<Boolean>();
      if (forUpdate)
        loadForIncrementalUpdates = true;
    }
    originPdf = filePath;
    DocumentLoadAsync* worker = new DocumentLoadAsync(cb, this, filePath);
    worker->ForUpdate(forUpdate);
    worker->Queue();
  } catch (PdfError& e) {
    if (e.GetError() == ePdfError_InvalidPassword) {
      throw Napi::Error::New(info.Env(),
                             "Password required to modify this document");
    } else {
      ErrorHandler err;
      string msg = err.WriteMsg(e);
      throw Napi::Error::New(info.Env(), msg);
    }
  }
  return info.Env().Undefined();
}

class DocumentWriteBufferAsync : public AsyncWorker
{
public:
  DocumentWriteBufferAsync(Function& cb, Document* doc)
    : AsyncWorker(cb)
    , doc(doc)
  {}
  ~DocumentWriteBufferAsync() {}

private:
  Document* doc;
  string value = "";
  size_t size = 0;

protected:
  void Execute()
  {
    PdfRefCountedBuffer docBuffer;
    PdfOutputDevice device(&docBuffer);
    doc->GetDocument()->Write(&device);
    value = string(docBuffer.GetBuffer());
    size = docBuffer.GetSize();
  }
  void OnOK()
  {
    HandleScope scope(Env());
    if (value.empty() || size == 0) {
      SetError("Error, failed to write to buffer");
    }
    Callback().Call(
      { Env().Null(), Buffer<char>::Copy(Env(), value.c_str(), size) });
  }
};

Napi::Value
Document::WriteBuffer(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_function });
  auto cb = info[0].As<Function>();
  DocumentWriteBufferAsync* worker = new DocumentWriteBufferAsync(cb, this);
  worker->Queue();
  return info.Env().Undefined();
}
