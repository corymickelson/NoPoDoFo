/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2019
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

#include "base/Color.h"
#include <podofo/podofo.h>

#if defined(_WIN32) || defined(_WIN64)
/* This is intended as a drop-in replacement for unistd.h on Windows.
 * Please add functionality as neeeded.
 * https://stackoverflow.com/a/826027/1202830
 */

#include <direct.h> /* for _getcwd() and _chdir() */
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
#elif defined(__APPLE__)
#include <unistd.h>
#define string_view std::string
#elif defined(__linux__)
#include <experimental/string_view>
#include <unistd.h>
using std::experimental::string_view;
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <experimental/filesystem>
#include <spdlog/spdlog.h>
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

#define TRY_LOAD(doc, file, buffer, pwd, forUpdate, typeE)                     \
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

#define NPDF_COLOR_ACCESSOR(color, types, cb)                                  \
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
template<typename... Ts>
inline void
Logger(const std::string& logger,
       spdlog::level::level_enum level,
       const char* format,
       Ts&&... ts)
{
  if (spdlog::get(logger) != nullptr) {
    spdlog::get(logger)->log(level, format, ts...);
  }
}
template<typename... Ts>
inline void
Logger(std::shared_ptr<spdlog::logger> logger,
       spdlog::level::level_enum level,
       const char* format,
       Ts&&... ts)
{
  if (logger != nullptr) {
    logger->log(level, format, ts...);
  }
}
template<typename... Ts>
inline void
Logger(std::shared_ptr<spdlog::logger> logger, const char* format, Ts&&... ts)
{
  if (logger != nullptr) {
    logger->log(logger->level(), format, ts...);
  }
}
}

const char RECT_OP[] = "re";
const char CONCAT_MATRIX_OP[] = "cm";
const char CURVE_TO_OP[] = "c";
const char END_PATH_NO_FILL_OR_STROKE_OP[] = "n";
const char FILL_OP[] = "f";
const char FILL_EVEN_ODD_OP[] = "f*";
const char INVOKE_X_OBJECT_OP[] = "Do";
const char LINE_TO_OP[] = "l";
const char BEGIN_MARKED_CONTENT_OP[] = "BMC";
const char END_MARKED_CONTENT_OP[] = "EMC";
const char TEXT_POS_OP[] = "Td";
const char MOVE_TO_OP[] = "m";
const char CHAR_SPACING_OP[] = "Tc";
const char CMYK_OP[] = "k";
const char CMYK_STROKE_OP[] = "K";
const char DASH_OP[] = "d";
const char GREY_OP[] = "g";
const char GREY_STROKE_OP[] = "G";
const char LINE_CAP_OP[] = "J";
const char LINE_JOIN_OP[] = "j";
const char LINE_WIDTH_OP[] = "w";
const char NON_ZERO_WINDING_CLIP_OP[] = "W";
const char RGB_OP[] = "rg";
const char RGB_STROKE_OP[] = "RG";
const char FONT_AND_SIZE_OP[] = "Tf";
const char SHOW_TEXT_OP[] = "Tj";
const char RESTORE_OP[] = "Q";
const char SAVE_OP[] = "q";
const char STROKE_OP[] = "S";
const char BEGIN_TEXT_OP[] = "BT";
const char END_TEXT_OP[] = "ET";

#endif // NOPODOFO_DEFINES_H
