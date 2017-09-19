//
// Created by red on 9/10/17.
//

#ifndef NPDF_ERRORHANDLER_H
#define NPDF_ERRORHANDLER_H

#include <podofo/podofo.h>
#include <string>

using namespace std;

class ErrorHandler
{
public:
  ErrorHandler();
  ~ErrorHandler();
  static string WriteMsg(PoDoFo::PdfError&);

private:
  static stringstream ParseMsgFromPdfError(PoDoFo::PdfError&);
};
#endif // NPDF_ERRORHANDLER_H
