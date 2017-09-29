//
// Created by red on 9/10/17.
//

#ifndef NPDF_ERRORHANDLER_H
#define NPDF_ERRORHANDLER_H

#include <napi.h>
#include <podofo/podofo.h>
#include <string>

using namespace std;
using namespace Napi;
using namespace PoDoFo;

class ErrorHandler
{
public:
  ErrorHandler();
  ErrorHandler(PoDoFo::PdfError&, const CallbackInfo&);
  ~ErrorHandler();
  static string WriteMsg(PoDoFo::PdfError&);

private:
  static string ParseMsgFromPdfError(PoDoFo::PdfError&);
};
#endif // NPDF_ERRORHANDLER_H
