/*
 * Copyright (C) 2017 zyx [@:] zyx gmx [dot] us
 *
 * The litePDF library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, the version 3
 * of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENCODINGS_H
#define ENCODINGS_H

#include <winsock2.h>
#include <windows.h>
#include <map>

#include "podofo/podofo.h"

using namespace PoDoFo;

// Silence some annoying warnings from Visual Studio
#ifdef _MSC_VER
#if _MSC_VER <= 1200 // Visual Studio 6
#pragma warning(disable: 4786)
#endif // _MSC_VER <= 1200
#endif // _MSC_VER

namespace litePDF {

class MEncodingsCache
{
private:
   std::map<BYTE, std::pair<bool /* can delete */, PdfEncoding *> > encodings;
   PdfDocument *document;
public:
   MEncodingsCache(PdfDocument *pDocument);
   virtual ~MEncodingsCache();

   const PdfEncoding *getEncodingForCharSet(BYTE lfCharSet, PdfDocument *overrideDocument = NULL);
   /* returns encoding for given charset (LOGFONT::lfCharSet), NULL if not available */

   void populateBase14FontWidths(PdfFont *base14Font);
   /* sets Widths of characters into Dictionary of base14Font */

   void put_encodingReplacement(BYTE lfCharSet, const PdfEncoding *encoding);
   /* stores encoding as a replacement for lfCharSet; the 'encoding' should stay
      valid for a whole life-time of the EncodingsCache */

   void clear(void);
   /* clears whole cache */

   const wchar_t *maybeFixSymbolCharsetString(PdfFont *font, const wchar_t *origString, DWORD nChars, wchar_t **newString);
   /* changes origString to point use symbol charset letters, if the font is symbol;
      it allocates *newString for it, which will contain nChars too. Returns either origString
      or *newString. Free *newString with free().
   */
};

}; // namespace litePDF

using namespace litePDF;

#endif // ENCODINGS_H
