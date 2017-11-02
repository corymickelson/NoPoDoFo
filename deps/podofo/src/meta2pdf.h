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

#ifndef META2PDF_H
#define META2PDF_H

#include <winsock2.h>
#include <windows.h>

#include "podofo/podofo.h"
#include "encodings.h"

double LITEPDF_MM_TO_POINTS(double mm);
double LITEPDF_POINTS_TO_MM(double pts);

typedef struct _DBLSIZE {
   double cx;
   double cy;
} DBLSIZE;

typedef void (__stdcall * Meta2PdfErrorCB)(unsigned int code, const char *msg, FAR void *user_data);
/**<
   Error callback, usually used to inform about parts unknown to Meta2Pdf
   drawn in the device context.
   @param code Error code.
   @param msg Error message.
   @param user_data User data provided in @ref CloseMeta2PdfDC.
*/

HDC CreateMeta2PdfDC(const DBLSIZE &page_mm);
/**<
   Creates a device context of a size page_size to which is drawn image
   with standard GDI functions. This device context should be closed
   with @ref CloseMeta2PdfDC.

   @param page_mm Page size in millimeter units.
   @return Newly created device context; NULL on any error.
*/

void CloseMeta2PdfDC(HDC hDC,
                     const DBLSIZE &page_mm,
                     const SIZE &page_px,
                     PoDoFo::PdfDocument *document,
                     PoDoFo::PdfPainter *painter,
                     litePDF::MEncodingsCache *encodingsCache,
                     unsigned int drawFlags,
                     Meta2PdfErrorCB on_error,
                     FAR void *on_error_user_data);
/**<
   Finishes drawing to a device context and converts it to a PDF.
   on_error callback is used to inform about conversion errors,
   like if the context has some unsupported drawing in itself.

   @param hDC Device context created with @ref CreateMeta2PdfDC.
   @param page_mm Page size in millimeters.
   @param page_px Page size in pixels.
   @param document PdfDocument to write data to.
   @param painter Painter to draw this page in.
   @param encodingsCache Cache of PDF encodings for builtin fonts
   @param drawFlags bit-OR of LITEPDF_DRAW_FLAG_... constants
   @param on_error Callback to use for conversion error messages.
   @param on_error_user_data User data passed to on_error callback.
   
   @note After this call the device context and all its memory is freed.
*/

void AbortMeta2PdfDC(HDC hDC);
/**<
   Aborts any changes made to the hDC, created by @ref CreateMeta2PdfDC and
   frees all associated memory with it, without saving the changes anywhere.

   @param hDC Device context created with @ref CreateMeta2PdfDC.
*/

void PlayMeta2Pdf(HENHMETAFILE emf,
                  const DBLSIZE &page_mm,
                  const SIZE &page_px,
                  PoDoFo::PdfDocument *document,
                  PoDoFo::PdfPainter *painter,
                  litePDF::MEncodingsCache *encodingsCache,
                  unsigned int drawFlags,
                  Meta2PdfErrorCB on_error,
                  FAR void *on_error_user_data);
/**<
   Gets saved EnhancedMetaFile and converts it ta PDF.
   on_error callback is used to inform about conversion errors,
   like if the context has some unsupported drawing in itself.

   @param emf Enhanced Meta File to play.
   @param page_mm Page size in millimeters.
   @param page_px Page size in pixels.
   @param document PdfDocument to write data to.
   @param painter Painter to draw this page in.
   @param encodingsCache Cache of PDF encodings for builtin fonts
   @param drawFlags bit-OR of LITEPDF_DRAW_FLAG_... constants
   @param on_error Callback to use for conversion error messages.
   @param on_error_user_data User data passed to on_error callback.
*/

#endif /* META2PDF_H */
