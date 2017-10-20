//
// Created by red on 9/10/17.
//

#ifndef NPDF_ERRORHANDLER_H
#define NPDF_ERRORHANDLER_H

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>
#include <sstream>
#include <string>

using namespace std;

class ErrorHandler
{
public:
  ErrorHandler();
  ErrorHandler(PoDoFo::PdfError&, const Napi::CallbackInfo&);
  ErrorHandler(Napi::Error&, const Napi::CallbackInfo&);
  ~ErrorHandler();
  static string WriteMsg(PoDoFo::PdfError&);
  static string WriteMsg(Napi::Error&);

private:
  static string ParseMsgFromPdfError(PoDoFo::PdfError&);
};
#endif // NPDF_ERRORHANDLER_H
