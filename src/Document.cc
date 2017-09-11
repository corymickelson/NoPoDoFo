//
// Created by red on 9/6/17.
//

#include "Document.h"
#include "closure.h"

//Napi::FunctionReference Document::constructor;

Document::Document(const CallbackInfo& callbackInfo)
  : ObjectWrap(callbackInfo)
{
  _document = new PoDoFo::PdfMemDocument();
}

Napi::Value
Document::Load(const CallbackInfo& info)
{
  string filePath = info[0].As<String>().Utf8Value();
  if (!filesystem::exists(filePath)) {
    stringstream ss;
    ss << "File: " << filePath << " not found. Try using absolute path instead "
       << endl;
    throw Napi::Error::New(info.Env(), ss.str());
  }
  originPdf = filePath;

  // async, if callback function provided
  /*if(info[1].IsFunction()) {
    bool status;
    auto *closure = static_cast<closure_t*>(malloc(sizeof(closure_t*)));
    status = InitClosure(closure, this);
    if(!status) {
      DestroyClosure(closure);
      free(closure);
      throw Napi::Error::New(info.Env(), "Failed to create closure. What did you do?");
    }
    this->Ref();
    closure->ptr = new Napi::FunctionReference();
    closure->ptr->Reset(info[1].As<Napi::Function>(),1);
    auto *req = new uv_work_t;
    req->data = closure;
    uv_queue_work(uv_default_loop(), req, LoadAsync,)
  }*/
  _document->Load(filePath.c_str());
  return Value();
}

static void
LoadAsync(const string &file, PoDoFo::PdfMemDocument *document, closure_t *closure)
{
  document->Load(file.c_str());
}

static void
LoadAsyncAfter()
{

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
  PoDoFo::PdfPage* page = _document->GetPage(n);
  auto pagePtr = Napi::External<PoDoFo::PdfPage>::New(info.Env(), page);
  auto docPtr =
    Napi::External<PoDoFo::PdfMemDocument>::New(info.Env(), _document);
  auto instance = Page::constructor.New({ pagePtr, docPtr });
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
Document::SetPassword(const CallbackInfo& info)
{
  if (info.Length() < 1) {
  }
  string password = info[0].As<String>().Utf8Value();
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
