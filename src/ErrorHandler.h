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
  ~ErrorHandler() {}
  static string WriteMsg(PoDoFo::PdfError&);
  static string WriteMsg(Napi::Error&);

private:
  static string ParseMsgFromPdfError(PoDoFo::PdfError&);
};
#endif // NPDF_ERRORHANDLER_H
