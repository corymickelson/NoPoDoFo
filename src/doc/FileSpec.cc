//
// Created by red on 9/11/17.
//

//( const char* pszFilename, bool bEmbedd, PdfDocument* pParent, bool
// bStripPath)

#include "FileSpec.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
FunctionReference FileSpec::constructor;

void
FileSpec::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(env, "FileSpec", {});
  target.Set("FileSpec", ctor);
}
FileSpec::FileSpec(const CallbackInfo& info)
  : ObjectWrap<FileSpec>(info)
{
  if (info.Length() != 2) {
    throw Napi::TypeError::New(
      info.Env(), "FileSpec requires two arguments of type: string, Document");
  }
  string file = info[0].As<String>().Utf8Value();
  auto docObj = info[1].As<Object>();
  Document* doc = Document::Unwrap(docObj);
  spec = new PdfFileSpec(file.c_str(), true, doc->GetDocument(), true);
}
FileSpec::~FileSpec()
{
  if (spec != nullptr) {
    HandleScope scope(Env());
    delete spec;
  }
}
}
