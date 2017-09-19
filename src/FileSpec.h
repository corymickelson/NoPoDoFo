//
// Created by red on 9/11/17.
//

#ifndef NPDF_FILESPEC_H
#define NPDF_FILESPEC_H

#include "Document.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace std;
using namespace Napi;
using namespace PoDoFo;

class FileSpec : public ObjectWrap<FileSpec>
{
public:
  FileSpec(const CallbackInfo&);
  ~FileSpec() { delete spec; }
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(env, "FileSpec", {});
    target.Set("FileSpec", ctor);
  }

private:
  PdfFileSpec* spec;
};
#endif // NPDF_FILESPEC_H
