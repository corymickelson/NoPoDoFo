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

#ifndef NOPODOFO_DEFINES_H
#define NOPODOFO_DEFINES_H

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define DEFER(fn) ScopeGuard CONCAT(__defer__, __LINE__) = [&]() { fn; }

#include "base/Color.h"
#include <napi.h>
#include <podofo/podofo.h>

#if defined(_WIN32) || defined(_WIN64)
/* This is intended as a drop-in replacement for unistd.h on Windows.
 * Please add functionality as neeeded.
 * https://stackoverflow.com/a/826027/1202830
 */

#include <direct.h> /* for _getcwd() and _chdir() */
#include <io.h>
#include <process.h> /* for getpid() and the exec..() family */
#include <stdlib.h>
#include <string>

#define srandom srand
#define random rand

/* Values for the second argument to access.
   These may be OR'd together.  */
#define R_OK 4 /* Test for read permission.  */
#define W_OK 2 /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK 0 /* Test for existence.  */

#define access _access
#define string_view std::string
#else
#include <unistd.h>
#include <experimental/string_view>
using std::experimental::string_view;
#endif

#if defined _WIN32 || defined _WIN64 || __linux__
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

enum DocumentInputDevice
{
  Disk,
  Memory
};
int
FileAccess(std::string& file);

#define TryLoad(doc, file, buffer, pwd, forUpdate, typeE)                      \
  {                                                                            \
    try {                                                                      \
      switch (typeE) {                                                         \
        case DocumentInputDevice::Disk:                                        \
          (doc).Load((file).c_str(), forUpdate);                               \
          break;                                                               \
        case DocumentInputDevice::Memory:                                      \
          (doc).LoadFromDevice(buffer, forUpdate);                             \
          break;                                                               \
      }                                                                        \
    } catch (PdfError & e) {                                                   \
      if (e.GetError() == ePdfError_InvalidPassword) {                         \
        if ((pwd).empty()) {                                                   \
          SetError("Password required to modify this document");               \
        } else {                                                               \
          try {                                                                \
            (doc).SetPassword(pwd);                                            \
          } catch (...) {                                                      \
            SetError("Password Invalid");                                      \
          }                                                                    \
        }                                                                      \
      } else {                                                                 \
        SetError(ErrorHandler::WriteMsg(e));                                   \
      }                                                                        \
    }                                                                          \
  }
enum NPDFColorFormat
{
  GreyScale = 0,
  RGB,
  CMYK
};

#define NPDFColorAccessor(color, types, cb)                                    \
  /* Color is the PdfColor object, types an array of NPDFColorFormat's in*/    \
  /* order of precedence, and the function to call as the cb*/                 \
  {                                                                            \
    if ((*color).IsRGB() &&                                                    \
        find((types).begin(), (types).end(), NPDFColorFormat::RGB) !=          \
          (types).end()) {                                                     \
      cb((*color).GetRed(), (*color).GetGreen(), (*color).GetBlue());          \
      return;                                                                  \
    }                                                                          \
    if ((*color).IsCMYK() &&                                                   \
        find((types).begin(), (types).end(), NPDFColorFormat::CMYK) !=         \
          (types).end()) {                                                     \
      cb((*color).GetCyan(),                                                   \
         (*color).GetMagenta(),                                                \
         (*color).GetYellow(),                                                 \
         (*color).GetBlack());                                                 \
      return;                                                                  \
    }                                                                          \
    if ((*color).IsGrayScale() &&                                              \
        find((types).begin(), (types).end(), NPDFColorFormat::GreyScale) !=    \
          (types).end()) {                                                     \
      cb((*color).GetGrayScale());                                             \
      return;                                                                  \
    }                                                                          \
                                                                               \
    switch (types[0]) {                                                        \
      case NPDFColorFormat::GreyScale: {                                       \
        auto gs = (*color).ConvertToGrayScale();                               \
        cb(gs.GetGrayScale());                                                 \
        break;                                                                 \
      }                                                                        \
      case NPDFColorFormat::RGB: {                                             \
        auto rgb = (*color).ConvertToRGB();                                    \
        cb(rgb.GetRed(), rgb.GetGreen(), rgb.GetBlue());                       \
        break;                                                                 \
      }                                                                        \
      case NPDFColorFormat::CMYK: {                                            \
        auto cmyk = (*color).ConvertToCMYK();                                  \
        cb(cmyk.GetCyan(),                                                     \
           cmyk.GetMagenta(),                                                  \
           cmyk.GetYellow(),                                                   \
           cmyk.GetBlack());                                                   \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }
}

const char RectOp[] = "re";
const char ConcatMatrixOp[] = "cm";
const char CurveToOp[] = "c";
const char EndPathNoFillOrStrokeOp[] = "n";
const char FillOp[] = "f";
const char FillEvenOddOp[] = "f*";
const char InvokeXObjectOp[] = "Do";
const char LineToOp[] = "l";
const char BeginMarkedContentOp[] = "BMC";
const char EndMarkedContentOp[] = "EMC";
const char TextPosOp[] = "Td";
const char MoveToOp[] = "m";
const char CharSpacingOp[] = "Tc";
const char CMYKOp[] = "k";
const char CMYKStrokeOp[] = "K";
const char DashOp[] = "d";
const char GreyOp[] = "g";
const char GreyStrokeOp[] = "G";
const char LineCapOp[] = "J";
const char LineJoinOp[] = "j";
const char LineWidthOp[] = "w";
const char NonZeroWindingClipOp[] = "W";
const char RGBOp[] = "rg";
const char RGBStrokeOp[] = "RG";
const char FontAndSizeOp[] = "Tf";
const char ShowTextOp[] = "Tj";
const char RestoreOp[] = "Q";
const char SaveOp[] = "q";
const char StrokeOp[] = "S";
const char BeginTextOp[] = "BT";
const char EndTextOp[] = "ET";

#endif // NOPODOFO_DEFINES_H
