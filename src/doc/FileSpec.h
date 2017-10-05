//
// Created by red on 9/11/17.
//

#ifndef NPDF_FILESPEC_H
#define NPDF_FILESPEC_H

#include "Document.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace std;

class FileSpec : public Napi::ObjectWrap<FileSpec>
{
public:
  FileSpec(const Napi::CallbackInfo&);
  ~FileSpec() { delete spec; }
  static void Initialize(Napi::Env& env, Napi::Object& target);

private:
  PoDoFo::PdfFileSpec* spec;
};
#endif // NPDF_FILESPEC_H
