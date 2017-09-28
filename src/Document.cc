//
// Created by red on 9/6/17.
//

#include "Document.h"

Document::Document(const CallbackInfo& info)
  : ObjectWrap(info)
{
  _document = new PoDoFo::PdfMemDocument();
}

Napi::Value
Document::Load(const CallbackInfo& info)
{
  string filePath = info[0].As<String>().Utf8Value();
  bool forUpdate = false;
  if (info.Length() == 2) {
    forUpdate = info[1].As<Boolean>();
  }
  if (!filesystem::exists(filePath)) {
    stringstream ss;
    ss << "File: " << filePath << " not found. Try using absolute path instead "
       << endl;
    throw Napi::Error::New(info.Env(), ss.str());
  }
  originPdf = filePath;
  try {
    _document->Load(filePath.c_str(), forUpdate);
  } catch (PoDoFo::PdfError& e) {
    if (e.GetError() == PoDoFo::ePdfError_InvalidPassword) {
      throw Napi::Error::New(info.Env(),
                             "Password required to modify this document");
    } else {
      ErrorHandler err;
      string msg = err.WriteMsg(e);
      throw Napi::Error::New(info.Env(), msg);
    }
  }
  return Value();
}

Napi::Value
Document::GetPageCount(const CallbackInfo& info)
{
  int pages = _document->GetPageCount();
  return Napi::Number::New(info.Env(), pages);
}

Napi::Value
Document::GetPage(const CallbackInfo& info)
{
  if (info.Length() != 1 || !info[0].IsNumber()) {
    throw Napi::Error::New(info.Env(),
                           "getPage takes an argument of 1, of type number.");
  }
  int n = info[0].As<Number>();
  int pl = _document->GetPageCount();
  if (n > pl) {
    throw Napi::RangeError::New(info.Env(), "Index out of page count range");
  }
  PoDoFo::PdfPage* page = _document->GetPage(n);
  auto pagePtr = Napi::External<PoDoFo::PdfPage>::New(info.Env(), page);
  auto docPtr =
    Napi::External<PoDoFo::PdfMemDocument>::New(info.Env(), _document);
  auto instance = Page::constructor.New(
    { pagePtr, docPtr, Napi::Number::New(info.Env(), n) });
  return instance;
}

Napi::Value
Document::Write(const CallbackInfo& info)
{
  if (info.Length() == 1 && info[0].IsString()) {
    string destinationFile = info[0].As<String>().Utf8Value();
    PoDoFo::PdfOutputDevice device(destinationFile.c_str());
    _document->Write(&device);
    return Napi::String::New(info.Env(), destinationFile);
  } else if (info.Length() == 0) {
    throw Napi::Error::New(info.Env(),
                           "Writing to node buffer not currently supported :(");
  } else {
    throw Napi::Error::New(
      info.Env(),
      "Invalid argument. Write(arg:string) will write the the file path"
      "defined by arg. Write() will return a Buffer containing the "
      "document.");
  }
}

void
Document::SetPassword(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty()) {
    throw Napi::Error::New(info.Env(), "Can not set empty password");
  }
  string password = value.As<String>().Utf8Value();
  //  string password = info[0].As<String>().Utf8Value();
  _document->SetPassword(password);
}

Napi::Value
Document::GetVersion(const CallbackInfo& info)
{
  PoDoFo::EPdfVersion versionE = _document->GetPdfVersion();
  double v = 0.0;
  switch (versionE) {
    case PoDoFo::ePdfVersion_1_1:
      v = 1.1;
      break;
    case PoDoFo::ePdfVersion_1_3:
      v = 1.3;
      break;
    case PoDoFo::ePdfVersion_1_0:
      v = 1.0;
      break;
    case PoDoFo::ePdfVersion_1_2:
      v = 1.2;
      break;
    case PoDoFo::ePdfVersion_1_4:
      v = 1.4;
      break;
    case PoDoFo::ePdfVersion_1_5:
      v = 1.5;
      break;
    case PoDoFo::ePdfVersion_1_6:
      v = 1.6;
      break;
    case PoDoFo::ePdfVersion_1_7:
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
  return Napi::Boolean::New(info.Env(), _document->IsLinearized());
}

void
Document::DeletePage(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_number });
  int pageIndex = info[0].As<Number>();
  _document->GetPagesTree()->DeletePage(pageIndex);
}

void
Document::MergeDocument(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  string docPath = info[0].As<String>().Utf8Value();
  if (filesystem::exists(docPath) == false) {
    stringstream msg;
    msg << "File: " << docPath << " not found" << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
  PoDoFo::PdfMemDocument merged(docPath.c_str());
  _document->Append(merged);
}

Napi::Value
Document::GetWriteMode(const CallbackInfo& info)
{
  string writeMode;
  switch (_document->GetWriteMode()) {
    case PoDoFo::ePdfWriteMode_Clean: {
      writeMode = "Clean";
      break;
    }
    case PoDoFo::ePdfWriteMode_Compact: {
      writeMode = "Compact";
      break;
    }
  }
  return Napi::String::New(info.Env(), writeMode);
}

void
Document::SetEncrypted(const CallbackInfo& info)
{
  try {
    AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
    if (!info[0].IsObject()) {
      throw Napi::Error::New(info.Env(),
                             "Set encrypt requires a single argument of"
                             " type Object<{userPassword:string,"
                             " ownerPassword:string, protection:Array<string>,"
                             " algorithm: string, keyLength: int");
    }
    auto encryption = info[0].As<Object>();
    string ownerPwd;
    string userPwd;
    int permParameter = 0;
    int algoParameter = 0;
    int key = 0;
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
    _document->SetEncrypted(
      userPwd,
      ownerPwd,
      permParameter,
      static_cast<PoDoFo::PdfEncrypt::EPdfEncryptAlgorithm>(algoParameter),
      static_cast<PoDoFo::PdfEncrypt::EPdfKeyLength>(key));
  } catch (PoDoFo::PdfError& err) {
    stringstream msg;
    msg << "PoDoFo::PdfMemDocument::SetEncrypt failed with error: "
        << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}
