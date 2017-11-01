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

#ifdef _MSC_VER
#if _MSC_VER <= 1200 // Visual Studio 6
#pragma warning(disable: 4786)
#endif // _MSC_VER <= 1200
/* #ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG */
#endif // _MSC_VER

#include <winsock2.h>
#include <windows.h>

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <map>
#include <vector>

#include "meta2pdf.h"
#include "litePDF.h"
#include "jpeglib.h"

using namespace std;
using namespace PoDoFo;

#define STRUCT_OFFSET(_str, _bytes) ((void *) (((BYTE *) (_str)) + (_bytes)))

#ifndef BI_JPEG
#define BI_JPEG 4L
#endif

#ifndef BI_PNG
#define BI_PNG 5L
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Visual C++ 6 compatibility
#if defined(_MSC_VER) && _MSC_VER <= 1200
#define EXTLOGPEN32 EXTLOGPEN
#endif

#define double_equal(a, b) (((a) - 1e-6) <= (b) && ((a) + 1e-6) >= (b))

double LITEPDF_MM_TO_POINTS(double mm)
{
   double pts = (((mm) / 25.4) * 72.0);
   if (pts < 0) {
      pts -= 1e-6;
   } else {
      pts += 1e-6;
   }
   return pts;
}

double LITEPDF_POINTS_TO_MM(double pts)
{
   double mm = (((pts) / 72.0) * 25.4);
   if (mm < 0) {
      mm -= 1e-6;
   } else {
      mm += 1e-6;
   }
   return mm;
}

static double sqr(double x)
{
   return x * x;
}

HDC CreateMeta2PdfDC(const DBLSIZE &page_mm)
{
   // function expects values in 0.01 millimeter units
   RECT page_rect;
   page_rect.left = 0;
   page_rect.top = 0;
   page_rect.right = page_mm.cx * 100;
   page_rect.bottom = page_mm.cy * 100;

   return CreateEnhMetaFile(NULL, NULL, &page_rect, NULL);
}
//---------------------------------------------------------------------------

typedef enum {
   OBJECT_DATA_TYPE_UNKNOWN,
   OBJECT_DATA_TYPE_PEN,
   OBJECT_DATA_TYPE_EXTPEN,
   OBJECT_DATA_TYPE_BRUSH,
   OBJECT_DATA_TYPE_FONT,
   OBJECT_DATA_TYPE_PALETTE
} MObjectDataType;

class MObjectData
{
 public:
   MObjectDataType type;
   union {
      LOGPEN pen;
      const EXTLOGPEN32 *extPen;
      const LOGBRUSH32 *brush;
      LOGFONT font;
   } v;
   void *bytes;
   std::string brushName;

   MObjectData()
   {
      type = OBJECT_DATA_TYPE_UNKNOWN;
      bytes = NULL;
      brushName = "";
   }

   ~MObjectData()
   {
      if (bytes) {
         free (bytes);
         bytes = NULL;
      }
   }
};

typedef struct _DWorldMatrix
{
   double eM11, eM12;
   double eM21, eM22;
   double eDx, eDy;
} DWorldMatrix;

struct dcState
{
   int iBkMode;
   int iMapMode;
   int iArcDirection;
   int iStretchMode;
   int iPolyFillMode;
   UINT textAlign;
   COLORREF textColor;
   COLORREF bkColor;
   bool bkColorSet;
   bool penSet;
   bool extPenSet;
   bool brushSet;
   bool fontSet;
   LOGPEN setPen;
   const EXTLOGPEN32 *setExtPen;
   const LOGBRUSH32 *setBrush; // can be NULL for bitmap-like brushes, then use setBrushObjectIndex
   DWORD setBrushObjectIndex; // index to processData::objects for setBrush
   std::string *setExtPenBrushName; //filled once the brush is added to resources
   std::string *setBrushName; //filled once the brush is added to resources
   LOGFONT setFont;
   unsigned int fontAngle; //in degrees
   double fontAscent; //in PDF units
   double fontDescent; //in PDF units
   POINT brushOrg;

   bool isClipping;
   HRGN clipRgn; // current clipping region, or whole page
   UINT metaRgnCount; // how many times the SetMetaRgn was called

   //world transformations, in HDC units
   bool worldMatrixValid; // whether it's used at all
   bool worldMatrixApplied;
   DWorldMatrix worldMatrix;
};

struct processData
{
   PoDoFo::PdfDocument *document;
   PoDoFo::PdfPainter *painter;
   litePDF::MEncodingsCache *encodingsCache;

   bool isWindows6x;

   HDC replayDC; // used to extract paths, clips and so on

   double current_posX, current_posY; //MoveTo/LineTo/..., where the current cursor is, in pdf units
   SIZE pageSize_lu; //page size of HDC, in logical units
   SIZE pdfPageSize_mm; //size of PDF page, in mm
   unsigned int drawFlags;

   struct dcState currentDCState;

   // only for isotropic/anisotropic map modes
   POINT windowOrg;
   SIZE windowExt;
   POINT viewportOrg;
   SIZE viewportExt;

   bool buildingPath;

   Meta2PdfErrorCB on_error;
   void *on_error_user_data;

   map<DWORD, MObjectData *> objects;
   vector<struct dcState> savedStates;
   map<DWORD, LOGBRUSH32 *> stdBrushes;
};
//---------------------------------------------------------------------------

static void freeProcessDataMembers(struct processData &pd)
{
   map<DWORD, LOGBRUSH32 *>::iterator bit, bend = pd.stdBrushes.end();
   for (bit = pd.stdBrushes.begin(); bit != bend; bit++) {
      LOGBRUSH32 *lb = bit->second;
      if (lb) {
         free(lb);
         bit->second = NULL;
      }
   }
   pd.stdBrushes.clear();

   map<DWORD, MObjectData *>::iterator oit, oend = pd.objects.end();
   for (oit = pd.objects.begin(); oit != oend; oit++) {
      MObjectData *obj = oit->second;

      oit->second = NULL;
      if (obj) {
         delete obj;
      }
   }
   pd.objects.clear();

   if (pd.replayDC) {
      DeleteDC(pd.replayDC);
      pd.replayDC = NULL;
   }

   if (pd.currentDCState.clipRgn) {
      DeleteObject (pd.currentDCState.clipRgn);
      pd.currentDCState.clipRgn = NULL;
   }

   vector<struct dcState>::iterator sit, send = pd.savedStates.end();
   for (sit = pd.savedStates.begin(); sit != send; sit++) {
      struct dcState &state = *sit;

      if (state.clipRgn) {
         DeleteObject (state.clipRgn);
         state.clipRgn = NULL;
      }
   }
   pd.savedStates.clear();
}

static bool getIsWindows6x(void)
{
   OSVERSIONINFO osver;

   osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

   if (!GetVersionEx(&osver)) {
      return FALSE;
   }

   return osver.dwMajorVersion >= 6;
}

static int CALLBACK processEnhMetaFileCB(HDC hDC, HANDLETABLE *lpHTable, const ENHMETARECORD *lpEMFR, int nObj, LPARAM lpData);

void PlayMeta2Pdf(HENHMETAFILE emf,
                  const DBLSIZE &page_mm,
                  const SIZE &page_px,
                  PoDoFo::PdfDocument *document,
                  PoDoFo::PdfPainter *painter,
                  litePDF::MEncodingsCache *encodingsCache,
                  unsigned int drawFlags,
                  Meta2PdfErrorCB on_error,
                  FAR void *on_error_user_data)
{
   if (!emf) {
      if (on_error) {
         on_error (GetLastError(), "Failed to close meta file", on_error_user_data);
      }

      return;
   }
   if (!document) {
      if (on_error) {
         on_error (ERROR_INVALID_HANDLE, "PlayMeta2Pdf: document cannot be NULL", on_error_user_data);
      }
   } else if (!painter) {
      if (on_error) {
         on_error (ERROR_INVALID_HANDLE, "PlayMeta2Pdf: painter cannot be NULL", on_error_user_data);
      }
   } else {
      struct processData pd;
      double width, height;
      ENHMETAHEADER header;

      if (GetEnhMetaFileHeader (emf, sizeof (ENHMETAHEADER), &header)) {
         // rectangle in 0.01-millimeter units
         width = (header.rclFrame.right - header.rclFrame.left) / 100.0;
         height = (header.rclFrame.bottom - header.rclFrame.top) / 100.0;
      } else {
         if (on_error) {
            on_error (GetLastError(), "PlayMeta2Pdf: Failed to get meta file header", on_error_user_data);
         }

         DeleteEnhMetaFile (emf);

         return;
      }

      /* Embed font subsets by default, thus readers can show fonts properly */
      if ((drawFlags & (LITEPDF_DRAW_FLAG_EMBED_FONTS_NONE | LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE | LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET)) == 0) {
         drawFlags |= LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET;
      }

      pd.currentDCState.iBkMode = OPAQUE;
      pd.currentDCState.iMapMode = MM_TEXT;
      pd.currentDCState.iArcDirection = AD_COUNTERCLOCKWISE;
      pd.currentDCState.iStretchMode = COLORONCOLOR;
      pd.currentDCState.iPolyFillMode = ALTERNATE;
      pd.currentDCState.textAlign = TA_LEFT | TA_TOP | TA_NOUPDATECP;
      pd.currentDCState.textColor = RGB(0, 0, 0);
      pd.currentDCState.bkColor = RGB(255, 255, 255);
      pd.currentDCState.bkColorSet = false;
      pd.currentDCState.penSet = false;
      pd.currentDCState.extPenSet = false;
      pd.currentDCState.brushSet = false;
      pd.currentDCState.fontSet =  false;
      pd.currentDCState.fontAngle = 0;
      pd.currentDCState.fontAscent = 0.0;
      pd.currentDCState.fontDescent = 0.0;
      pd.currentDCState.brushOrg.x = 0;
      pd.currentDCState.brushOrg.y = 0;
      pd.currentDCState.isClipping = false;
      pd.currentDCState.clipRgn = NULL;
      pd.currentDCState.metaRgnCount = 0;
      pd.currentDCState.worldMatrixValid = false;
      pd.currentDCState.worldMatrixApplied = false;
      pd.currentDCState.worldMatrix.eM11 = 1.0;
      pd.currentDCState.worldMatrix.eM12 = 0.0;
      pd.currentDCState.worldMatrix.eM21 = 0.0;
      pd.currentDCState.worldMatrix.eM22 = 1.0;
      pd.currentDCState.worldMatrix.eDx = 0.0;
      pd.currentDCState.worldMatrix.eDy = 0.0;
      pd.windowOrg.x = 0;
      pd.windowOrg.y = 0;
      pd.windowExt.cx = 0;
      pd.windowExt.cy = 0;
      pd.viewportOrg.x = 0;
      pd.viewportOrg.y = 0;
      pd.viewportExt.cx = 0;
      pd.viewportExt.cy = 0;
      pd.buildingPath = false;
      pd.drawFlags = drawFlags;
      pd.current_posX = 0.0;
      pd.current_posY = 0.0;
      pd.pageSize_lu.cx = page_px.cx;
      pd.pageSize_lu.cy = page_px.cy;
      pd.pdfPageSize_mm.cx = width;
      pd.pdfPageSize_mm.cy = height;
      pd.document = document;
      pd.painter = painter;
      pd.encodingsCache = encodingsCache;
      pd.on_error = on_error;
      pd.on_error_user_data = on_error_user_data;
      pd.replayDC = CreateCompatibleDC(NULL);
      pd.isWindows6x = getIsWindows6x();

      RECT boundary;

      boundary.left = 0;
      boundary.top = 0;
      boundary.right = page_px.cx;
      boundary.bottom = page_px.cy;

      if (!EnumEnhMetaFile (pd.replayDC, emf, processEnhMetaFileCB, &pd, &boundary)) {
         if (on_error) {
            on_error (GetLastError(), "PlayMeta2Pdf: Failed to enumerate meta file", on_error_user_data);
         }
      }

      while (pd.currentDCState.metaRgnCount > 0) {
         pd.painter->Restore();
         pd.currentDCState.metaRgnCount--;
      }

      if (pd.currentDCState.isClipping) {
         pd.painter->Restore();
         pd.currentDCState.isClipping = false;
      }

      freeProcessDataMembers(pd);
   }
}

static void writeDebugPage (HENHMETAFILE emf, const DBLSIZE &page_mm, const SIZE &page_px);

void CloseMeta2PdfDC(HDC hDC,
                     const DBLSIZE &page_mm,
                     const SIZE &page_px,
                     PoDoFo::PdfDocument *document,
                     PoDoFo::PdfPainter *painter,
                     litePDF::MEncodingsCache *encodingsCache,
                     unsigned int drawFlags,
                     Meta2PdfErrorCB on_error,
                     FAR void *on_error_user_data)
{
   HENHMETAFILE emf;

   emf = CloseEnhMetaFile (hDC);
   if (!emf) {
      if (on_error) {
         on_error (GetLastError(), "CloseMeta2PdfDC: Failed to close meta file", on_error_user_data);
      }

      return;
   }

   writeDebugPage (emf, page_mm, page_px);

   PlayMeta2Pdf(emf, page_mm, page_px, document, painter, encodingsCache, drawFlags, on_error, on_error_user_data);

   DeleteEnhMetaFile (emf);
}
//---------------------------------------------------------------------------

void AbortMeta2PdfDC(HDC hDC)
{
   HENHMETAFILE emf;

   if (!hDC) {
      return;
   }

   emf = CloseEnhMetaFile (hDC);
   if (emf) {
      DeleteEnhMetaFile (emf);
   }
}
//---------------------------------------------------------------------------

static const char *get_emfr_type_as_string(DWORD iType)
{
#define cs(x) case x: return #x;
   switch(iType) {
   case 0x1: return "ENHMETAHEADER";
   cs(EMR_ABORTPATH);
   cs(EMR_ALPHABLEND);
   cs(EMR_ANGLEARC);
   cs(EMR_ARC);
   cs(EMR_ARCTO);
   cs(EMR_BEGINPATH);
   cs(EMR_BITBLT);
   cs(EMR_CHORD);
   cs(EMR_CLOSEFIGURE);
   cs(EMR_COLORCORRECTPALETTE);
   cs(EMR_COLORMATCHTOTARGETW);
   cs(EMR_CREATEBRUSHINDIRECT);
   cs(EMR_CREATECOLORSPACE);
   cs(EMR_CREATECOLORSPACEW);
   cs(EMR_CREATEDIBPATTERNBRUSHPT);
   cs(EMR_CREATEMONOBRUSH);
   cs(EMR_CREATEPALETTE);
   cs(EMR_CREATEPEN);
   cs(EMR_DELETECOLORSPACE);
   cs(EMR_DELETEOBJECT);
   cs(EMR_ELLIPSE);
   cs(EMR_ENDPATH);
   cs(EMR_EOF);
   cs(EMR_EXCLUDECLIPRECT);
   cs(EMR_EXTCREATEFONTINDIRECTW);
   cs(EMR_EXTCREATEPEN);
   cs(EMR_EXTFLOODFILL);
   cs(EMR_EXTSELECTCLIPRGN);
   cs(EMR_EXTTEXTOUTA);
   cs(EMR_EXTTEXTOUTW);
   cs(EMR_FILLPATH);
   cs(EMR_FILLRGN);
   cs(EMR_FLATTENPATH);
   cs(EMR_FRAMERGN);
   cs(EMR_GDICOMMENT);
   cs(EMR_GLSBOUNDEDRECORD);
   cs(EMR_GLSRECORD);
   cs(EMR_GRADIENTFILL);
   cs(EMR_INTERSECTCLIPRECT);
   cs(EMR_INVERTRGN);
   cs(EMR_LINETO);
   cs(EMR_MASKBLT);
   cs(EMR_MODIFYWORLDTRANSFORM);
   cs(EMR_MOVETOEX);
   cs(EMR_OFFSETCLIPRGN);
   cs(EMR_PAINTRGN);
   cs(EMR_PIE);
   cs(EMR_PIXELFORMAT);
   cs(EMR_PLGBLT);
   cs(EMR_POLYBEZIER);
   cs(EMR_POLYBEZIER16);
   cs(EMR_POLYBEZIERTO);
   cs(EMR_POLYBEZIERTO16);
   cs(EMR_POLYDRAW);
   cs(EMR_POLYDRAW16);
   cs(EMR_POLYGON);
   cs(EMR_POLYGON16);
   cs(EMR_POLYLINE);
   cs(EMR_POLYLINE16);
   cs(EMR_POLYLINETO);
   cs(EMR_POLYLINETO16);
   cs(EMR_POLYPOLYGON);
   cs(EMR_POLYPOLYGON16);
   cs(EMR_POLYPOLYLINE);
   cs(EMR_POLYPOLYLINE16);
   cs(EMR_POLYTEXTOUTA);
   cs(EMR_POLYTEXTOUTW);
   cs(EMR_REALIZEPALETTE);
   cs(EMR_RECTANGLE);
   cs(EMR_RESIZEPALETTE);
   cs(EMR_RESTOREDC);
   cs(EMR_ROUNDRECT);
   cs(EMR_SAVEDC);
   cs(EMR_SCALEVIEWPORTEXTEX);
   cs(EMR_SCALEWINDOWEXTEX);
   cs(EMR_SELECTCLIPPATH);
   cs(EMR_SELECTOBJECT);
   cs(EMR_SELECTPALETTE);
   cs(EMR_SETARCDIRECTION);
   cs(EMR_SETBKCOLOR);
   cs(EMR_SETBKMODE);
   cs(EMR_SETBRUSHORGEX);
   cs(EMR_SETCOLORADJUSTMENT);
   cs(EMR_SETCOLORSPACE);
   cs(EMR_SETDIBITSTODEVICE);
   cs(EMR_SETICMMODE);
   cs(EMR_SETICMPROFILEA);
   cs(EMR_SETICMPROFILEW);
   cs(EMR_SETLAYOUT);
   cs(EMR_SETMAPMODE);
   cs(EMR_SETMAPPERFLAGS);
   cs(EMR_SETMETARGN);
   cs(EMR_SETMITERLIMIT);
   cs(EMR_SETPALETTEENTRIES);
   cs(EMR_SETPIXELV);
   cs(EMR_SETPOLYFILLMODE);
   cs(EMR_SETROP2);
   cs(EMR_SETSTRETCHBLTMODE);
   cs(EMR_SETTEXTALIGN);
   cs(EMR_SETTEXTCOLOR);
   cs(EMR_SETVIEWPORTEXTEX);
   cs(EMR_SETVIEWPORTORGEX);
   cs(EMR_SETWINDOWEXTEX);
   cs(EMR_SETWINDOWORGEX);
   cs(EMR_SETWORLDTRANSFORM);
   cs(EMR_STRETCHBLT);
   cs(EMR_STRETCHDIBITS);
   cs(EMR_STROKEANDFILLPATH);
   cs(EMR_STROKEPATH);
   cs(EMR_TRANSPARENTBLT);
   cs(EMR_WIDENPATH);
   }
#undef cs

   return "Unknown";
}
//---------------------------------------------------------------------------

bool __stdcall AnsiToUnicode(const char *str, DWORD slen, LPWSTR wbuff, DWORD *bufflen)
{
   if (str == NULL || bufflen == NULL) return false;
   bool res = false;
   int count = MultiByteToWideChar(CP_ACP, 0, str, slen, wbuff, wbuff == NULL ? 0 : (*bufflen));
   if (count > 0) {
      res = true;
      *bufflen = count + (wbuff == NULL ? 1 : 0);
      if (wbuff != NULL) wbuff[count] = 0;
   }
   return res;
}
//---------------------------------------------------------------------------

bool __stdcall UnicodeToUTF8(const LPWSTR wstr, int nChars, char *buff, DWORD *bufflen)
{
   if (wstr == NULL || bufflen == NULL) return false;
   bool res = false;
   int count = WideCharToMultiByte(CP_UTF8, 0, wstr, nChars, buff, buff == NULL ? 0 : (*bufflen), NULL, NULL);
   if (count > 0) {
      res = true;
      *bufflen = count + (buff == NULL ? 1 : 0);
      if (buff != NULL) buff[count] = 0;
   }
   return res;
}
//---------------------------------------------------------------------------

bool __stdcall UnicodeToAnsi(const LPWSTR wstr, int nChars, char *buff, DWORD *bufflen)
{
   if (wstr == NULL || bufflen == NULL) return false;
   bool res = false;
   int count = WideCharToMultiByte(CP_ACP, 0, wstr, nChars, buff, buff == NULL ? 0 : (*bufflen), NULL, NULL);
   if (count > 0) {
      res = true;
      *bufflen = count + (buff == NULL ? 1 : 0);
      if (buff != NULL) buff[count] = 0;
   }
   return res;
}
//---------------------------------------------------------------------------

bool __stdcall AnsiToUTF8(const char *str, DWORD slen, char *buff, DWORD *bufflen)
{
   if (str == NULL || bufflen == NULL) return false;

   bool res=false;
   LPWSTR wstr = NULL;
   DWORD wlen = 0;
   if (AnsiToUnicode(str, slen, wstr, &wlen)) {
      wstr = (LPWSTR) malloc(sizeof(WCHAR) * wlen);
      if (wstr != NULL) {
         if (AnsiToUnicode(str, slen, wstr, &wlen)) {
            res = UnicodeToUTF8(wstr, -1, buff, bufflen);
         }
         free(wstr);
      }
   }

   return res;
}
//---------------------------------------------------------------------------

static double lu2pts(double lu_pt, double lu_sz, double mm_sz)
{
   if (double_equal(lu_sz, 0.0)) {
      PODOFO_RAISE_ERROR_INFO (ePdfError_InternalLogic, "Preventing division by zero");
   }

   double pts = LITEPDF_MM_TO_POINTS(lu_pt * mm_sz / lu_sz);

   if (lu_sz < -1e-6) {
      pts = pts * (-1.0);
   }

   return pts;
}

static void LogicalUnitToPdf(struct processData *pd, int luX, int luY, double &pdfX, double &pdfY, bool translate = true, bool isText = false)
{
   pdfX = 0.0;
   pdfY = 0.0;

   if (!pd) {
      return;
   }

   double dluX = luX, dluY = luY;

   if (pd->currentDCState.worldMatrixValid && pd->currentDCState.worldMatrixApplied) {
      if (double_equal (pd->currentDCState.worldMatrix.eM11, 1.0) && double_equal (pd->currentDCState.worldMatrix.eM12, 0.0) &&
          double_equal (pd->currentDCState.worldMatrix.eM21, 0.0) && double_equal (pd->currentDCState.worldMatrix.eM22, 1.0)) {
         // it's only a translation matrix, thus the translation can be applied if needed
      } else {
         translate = false;
      }
   }

   if (translate && ((pd->currentDCState.iMapMode != MM_ANISOTROPIC && pd->currentDCState.iMapMode != MM_ISOTROPIC) || isText)) {
      dluX += pd->viewportOrg.x - pd->windowOrg.x;
      dluY += pd->viewportOrg.y - pd->windowOrg.y;
   }

   switch(pd->currentDCState.iMapMode) {
   case MM_HIENGLISH:
   case MM_HIMETRIC:
   case MM_LOENGLISH:
   case MM_LOMETRIC:
      break;
   case MM_ANISOTROPIC:
   case MM_ISOTROPIC:
      if (pd->windowExt.cx && pd->windowExt.cy && !isText) {
         double Wo, Vo, We, Ve;

         Wo = translate ? pd->windowOrg.x : 0;
         Vo = translate ? pd->viewportOrg.x : 0;
         We = pd->windowExt.cx;
         Ve = pd->viewportExt.cx;

         dluX = ((dluX - Wo) * Ve / We) + Vo;
         /*if (We < 0) {
            dluX = -We - dluX;
         }*/

         Wo = translate ? pd->windowOrg.y : 0;
         Vo = translate ? pd->viewportOrg.y : 0;
         We = pd->windowExt.cy;
         Ve = pd->viewportExt.cy;

         dluY = ((dluY - Wo) * Ve / We) + Vo;
         /*if (We < 0) {
            dluY = -We - dluY;
         }*/
      }
      break;
   case MM_TEXT:
      // positive Y is down
      if (translate && !isText) {
         dluY = pd->pageSize_lu.cy - dluY;
         translate = false;
      }
      break;
   case MM_TWIPS:
      break;
   default:
      if (pd->on_error) {
         char buff[128];
         sprintf(buff, "Unknown map mode 0x%x", pd->currentDCState.iMapMode);
         pd->on_error (ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
      break;
   }

   pdfX = lu2pts(dluX, pd->pageSize_lu.cx, pd->pdfPageSize_mm.cx);
   pdfY = lu2pts(dluY, pd->pageSize_lu.cy, pd->pdfPageSize_mm.cy);

   if (translate) {
      // logical units has top at the top, while PDF has top-at-bottom
      pdfY = lu2pts(pd->pageSize_lu.cy, pd->pageSize_lu.cy, pd->pdfPageSize_mm.cy) - pdfY;
   }
}

static BOOL DirectoryExists(const char *dir)
{
   DWORD attrs = -1;

   if (dir)
      attrs = GetFileAttributes(dir);

   return attrs != -1 && (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static void writeDebugPage (HENHMETAFILE emf,
                            const DBLSIZE &page_mm,
                            const SIZE &page_px)
{
   static unsigned int pageNo = 0;
   static char filePath[MAX_PATH * 4 + 1] = { 0 };

   if (!emf) {
      return;
   }

   if (pageNo == 0) {
      const char *env = getenv("LITEPDF_DEBUG");
      if (env && *env && (strcmp (env, ".") == 0 || DirectoryExists(env))) {
         char path[3 * MAX_PATH + 1], *p;
         path[0] = 0;
         if (strcmp (env, ".") == 0) {
            DWORD len = GetModuleFileName(NULL, path, 3 * MAX_PATH);
            if (len) {
               path[len] = 0;
               p = strrchr (path, '\\');
               if (p) {
                  *p = 0;
               }
            }
         } else {
            strcpy (path, env);
         }

         while (*path && path[strlen(path) - 1] == '\\') {
            path[strlen(path) - 1] = 0;
         }

         if (*path && strlen(path) + 1 <= MAX_PATH * 3) {
            strcpy (filePath, path);
            strcat (filePath, "\\");
         }
      }
   }

   if (!*filePath) {
      pageNo = 0;
      return;
   }

   char filename[MAX_PATH * 4 + 1], file[MAX_PATH + 1];

   pageNo++;
   sprintf(file, "litePDF-page-%03u.data", pageNo);

   if (strlen(filePath) + strlen(file) + 1 > MAX_PATH * 4) {
      return;
   }

   strcpy (filename, filePath);
   strcat (filename, file);

   FILE *debug = fopen(filename, "wb");
   if (!debug) {
      return;
   }

   BYTE *data;
   UINT sz = GetEnhMetaFileBits (emf, 0, NULL);

   data = (BYTE *) malloc(sizeof(BYTE) * (sz + 1));
   if (data) {
      sz = GetEnhMetaFileBits (emf, sz, data);
      if (sz > 0) {
         fprintf(debug, "litePDF-page: mm:%.05f,%.05f px:%d,%d\n", page_mm.cx, page_mm.cy, page_px.cx, page_px.cy);
         fwrite(data, sizeof(BYTE), sz, debug);
      }
      free(data);
   }

   fclose(debug);
}

static void dumpRgnData(FILE *f, const RGNDATA *rgnData)
{
   if (!f || !rgnData) {
      return;
   }

   fprintf(f, "\tRgnDataHeader: dwSize;%d iType:0x%x (%s) nCount:%d (0x%x) nRgnSize:%d rcBounds[%d,%d,%d,%d]\n", rgnData->rdh.dwSize, rgnData->rdh.iType, rgnData->rdh.iType == RDH_RECTANGLES ? "RDH_RECTANGLES" : "???", rgnData->rdh.nCount, rgnData->rdh.nCount, rgnData->rdh.nRgnSize, rgnData->rdh.rcBound.left, rgnData->rdh.rcBound.top, rgnData->rdh.rcBound.right, rgnData->rdh.rcBound.bottom);

   if (rgnData->rdh.nCount > 0 && rgnData->rdh.iType == RDH_RECTANGLES) {
      const RECT *rects = (const RECT *) rgnData->Buffer;
      DWORD ii;

      fprintf(f, "\tRegion Rectangles:");
      for (ii = 0; ii < rgnData->rdh.nCount; ii++, rects++) {
         const RECT &r = *rects;
         fprintf(f, " [%d,%d,%d,%d]", r.left, r.top, r.right, r.bottom);
      }
      fprintf(f, "\n");
   }
}

static void dumpBitmapInfo(FILE *debug, const char *struct_name, void *data)
{
   if (!debug || !struct_name || !data) {
      return;
   }

   BITMAPINFO *bmi = (BITMAPINFO *) data;

   fprintf(debug, "\t%s: biSize:%d biWidth:%d biHeight:%d biPlanes:%d biBitCount:%d biCompression:%s (0x%x) biSizeImage:%d biXPelsPerMeter:%d biYPelsPerMeter:%d biClrUsed:%d biClrImportant:%d\n",
      struct_name,
      bmi->bmiHeader.biSize,
      bmi->bmiHeader.biWidth,
      bmi->bmiHeader.biHeight,
      bmi->bmiHeader.biPlanes,
      bmi->bmiHeader.biBitCount,
      bmi->bmiHeader.biCompression == BI_RGB ? "BI_RGB" : bmi->bmiHeader.biCompression == BI_RLE8 ? "BI_RLE8" : bmi->bmiHeader.biCompression == BI_RLE4 ? "BI_RLE4" : bmi->bmiHeader.biCompression == BI_BITFIELDS ? "BI_BITFIELDS" : bmi->bmiHeader.biCompression == BI_JPEG ? "BI_JPEG" : bmi->bmiHeader.biCompression == BI_PNG ? "BI_PNG" : "???",
      bmi->bmiHeader.biCompression,
      bmi->bmiHeader.biSizeImage,
      bmi->bmiHeader.biXPelsPerMeter,
      bmi->bmiHeader.biYPelsPerMeter,
      bmi->bmiHeader.biClrUsed,
      bmi->bmiHeader.biClrImportant);
}

static void writeToDebugLog(const ENHMETARECORD *lpEMFR, int nObj, const char *textToWrite)
{
   static bool first = true;
   static char filePath[MAX_PATH * 3 + 1] = { 0 };

   if (first) {
      const char *env = getenv("LITEPDF_DEBUG_LOG");
      if (env && *env && (strcmp (env, ".") == 0 || DirectoryExists(env))) {
         char path[3 * MAX_PATH + 1], *p;
         path[0] = 0;
         if (strcmp (env, ".") == 0) {
            DWORD len = GetModuleFileName(NULL, path, 3 * MAX_PATH);
            if (len) {
               path[len] = 0;
               p = strrchr (path, '\\');
               if (p) {
                  *p = 0;
               }
            }
         } else {
            strcpy (path, env);
         }

         while (*path && path[strlen(path) - 1] == '\\') {
            path[strlen(path) - 1] = 0;
         }

         if (*path && strlen(path) + strlen("\\litePDF-debug.log") <= MAX_PATH * 3) {
            strcpy (filePath, path);
            strcat (filePath, "\\litePDF-debug.log");
         }
      }
   }

   if (!*filePath) {
      first = false;
      return;
   }

   FILE *debug = fopen(filePath, first ? "wb" : "ab");
   first = false;
   if (!debug) {
      return;
   }

   if (textToWrite) {
      fprintf(debug, "%s", textToWrite);
      fclose(debug);
      return;
   }

   fprintf(debug, "%d: nSize:%d type:0x%x (%s)\n", nObj, lpEMFR->nSize, lpEMFR->iType, get_emfr_type_as_string (lpEMFR->iType));
   fwrite(lpEMFR, 1, lpEMFR->nSize, debug);
   fprintf(debug,"\n");
   switch(lpEMFR->iType) {
   case 0x1: {
      const ENHMETAHEADER *dt = (const ENHMETAHEADER *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] rclFrame:[%d,%d,%d,%d]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->rclFrame.left, dt->rclFrame.top, dt->rclFrame.right, dt->rclFrame.bottom);
      fprintf(debug, "\tszlDevice:%d,%d szlMillimeters:%d,%d\n", dt->szlDevice.cx, dt->szlDevice.cy, dt->szlMillimeters.cx, dt->szlMillimeters.cy);
      fprintf(debug, "\tdSignature:0x%x nVersion:0x%x nBytes:%d nRecords:%d nHandles:%d sReserved:0x%x\n", dt->dSignature, dt->nVersion, dt->nBytes, dt->nRecords, dt->nHandles, dt->sReserved);
      fprintf(debug, "\tnDescription:%d offDescription:%d nPalEntries:%d cbPixelFormat:0x%x offPixelFormat:%d bOpenGL:0x%x\n", dt->nDescription, dt->offDescription, dt->nPalEntries, dt->cbPixelFormat, dt->offPixelFormat, dt->bOpenGL);
   } break;

   case EMR_ALPHABLEND: {
      const EMRALPHABLEND *dt = (const EMRALPHABLEND *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] dest:[%d,%d,%d,%d] dwRop:0x%x src:%d,%d,%d,%d xform:%g;%g;%g;%g;%g;%g\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xformSrc.eM11, dt->xformSrc.eM12, dt->xformSrc.eM21, dt->xformSrc.eM22, dt->xformSrc.eDx, dt->xformSrc.eDy);
      fprintf(debug, "\tcrBkColorSrc:%06x iUsageSrc:0x%x offBmiSrc:%d cbBmiSrc:0x%x offBitsSrc:%d cbBitsSrc:0x%x\n", dt->crBkColorSrc, dt->iUsageSrc, dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
   } break;
   case EMR_ANGLEARC: {
      const EMRANGLEARC *dt = (const EMRANGLEARC *) lpEMFR;
      fprintf(debug, "\tptlCenter:%d,%d nRadius:%d eStartAngle:%g eSweepAngle:%g\n", dt->ptlCenter.x, dt->ptlCenter.y, dt->nRadius, dt->eStartAngle, dt->eSweepAngle);
   } break;
   case EMR_ARC: {
      const EMRARC *dt = (const EMRARC *) lpEMFR;
      fprintf(debug, "\trclBox:[%d,%d,%d,%d] ptlStart:%d,%d ptlEnd:%d,%d\n", dt->rclBox.left, dt->rclBox.top, dt->rclBox.right, dt->rclBox.bottom, dt->ptlStart.x, dt->ptlStart.y, dt->ptlEnd.x, dt->ptlEnd.y);
   } break;
   case EMR_ARCTO: {
      const EMRARCTO *dt = (const EMRARCTO *) lpEMFR;
      fprintf(debug, "\trclBox:[%d,%d,%d,%d] ptlStart:%d,%d ptlEnd:%d,%d\n", dt->rclBox.left, dt->rclBox.top, dt->rclBox.right, dt->rclBox.bottom, dt->ptlStart.x, dt->ptlStart.y, dt->ptlEnd.x, dt->ptlEnd.y);
   } break;
   case EMR_BITBLT: {
      const EMRBITBLT *dt = (const EMRBITBLT *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] dest:[%d,%d,%d,%d] dwRop:0x%x src:%d,%d xformSrc:[%g;%g;%g;%g;%g;%g]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->xSrc, dt->ySrc, dt->xformSrc.eM11, dt->xformSrc.eM12, dt->xformSrc.eM21, dt->xformSrc.eM22, dt->xformSrc.eDx, dt->xformSrc.eDy);
      fprintf(debug, "\tcrBkColorSrc:0x%06x dwRop:0x%x(%s) iUsageSrc:0x%x(%s) offBmiSrc:%d cbBmiSrc:%d offBitsSrc:%d cbBitsSrc:%d\n", dt->crBkColorSrc,
         dt->dwRop, dt->dwRop == BLACKNESS ? "BLACKNESS" : dt->dwRop == DSTINVERT ? "DSTINVERT" : dt->dwRop == MERGECOPY ? "MERGECOPY" : dt->dwRop == MERGEPAINT ? "MERGEPAINT" : dt->dwRop == NOTSRCCOPY ? "NOTSRCCOPY" : dt->dwRop == NOTSRCERASE ? "NOTSRCERASE" : dt->dwRop == PATCOPY ? "PATCOPY" :
         dt->dwRop == PATINVERT ? "PATINVERT" : dt->dwRop == PATPAINT ? "PATPAINT" : dt->dwRop == SRCAND ? "SRCAND" : dt->dwRop == SRCCOPY ? "SRCCOPY" : dt->dwRop == SRCERASE ? "SRCERASE" : dt->dwRop == SRCINVERT ? "SRCINVERT" : dt->dwRop == SRCPAINT ? "SRCPAINT" : dt->dwRop == WHITENESS ? "WHITENESS" : "???",
         dt->iUsageSrc, dt->iUsageSrc == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageSrc == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageSrc == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???",
         dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
   } break;
   case EMR_CHORD: {
      const EMRCHORD *dt = (const EMRCHORD *) lpEMFR;
      fprintf(debug, "\trclBox:[%d,%d,%d,%d] ptlStart:%d,%d ptlEnd:%d,%d\n", dt->rclBox.left, dt->rclBox.top, dt->rclBox.right, dt->rclBox.bottom, dt->ptlStart.x, dt->ptlStart.y, dt->ptlEnd.x, dt->ptlEnd.y);
   } break;
   case EMR_COLORCORRECTPALETTE: {
      const EMRCOLORCORRECTPALETTE *dt = (const EMRCOLORCORRECTPALETTE *) lpEMFR;
      fprintf(debug, "\tihPalette:%d (0x%x) nFirstEntry:%d nPalEntries:%d nReserved:0x%x\n", dt->ihPalette, dt->ihPalette, dt->nFirstEntry, dt->nPalEntries, dt->nReserved);
   } break;
   case EMR_COLORMATCHTOTARGETW: {
      const EMRCOLORMATCHTOTARGET *dt = (const EMRCOLORMATCHTOTARGET *) lpEMFR;
      fprintf(debug, "\tdwAction:0x%x (%s) dwFlags:0x%x cbName:%d cbData:%d\n", dt->dwAction, dt->dwAction == CS_ENABLE ? "CS_ENABLE" : dt->dwAction == CS_DISABLE ? "CS_DISABLE" : dt->dwAction == CS_DELETE_TRANSFORM ? "CS_DELETE_TRANSFORM" : "???", dt->dwFlags, dt->cbName, dt->cbData);
   } break;
   case EMR_CREATEBRUSHINDIRECT: {
      const EMRCREATEBRUSHINDIRECT *dt = (const EMRCREATEBRUSHINDIRECT *) lpEMFR;
      fprintf(debug, "\tihBrush:%d (0x%x) lbColor:0x%06x lbStyle:0x%x %s lbHatch:0x%x %s\n", dt->ihBrush, dt->ihBrush, dt->lb.lbColor, dt->lb.lbStyle, dt->lb.lbStyle == BS_DIBPATTERN ? "BS_DIBPATTERN" : dt->lb.lbStyle == BS_DIBPATTERNPT ? "BS_DIBPATTERNPT" : dt->lb.lbStyle == BS_HATCHED ? "BS_HATCHED" : dt->lb.lbStyle == BS_HOLLOW ? "BS_HOLLOW" : dt->lb.lbStyle == BS_PATTERN ? "BS_PATTERN" : dt->lb.lbStyle == BS_SOLID ? "BS_SOLID" : "???", dt->lb.lbHatch, dt->lb.lbStyle == BS_HATCHED ? (dt->lb.lbHatch == HS_BDIAGONAL ? "HS_BDIAGONAL" : dt->lb.lbHatch == HS_CROSS ? "HS_CROSS" : dt->lb.lbHatch == HS_DIAGCROSS ? "HS_DIAGCROSS" : dt->lb.lbHatch == HS_FDIAGONAL ? "HS_FDIAGONAL" : dt->lb.lbHatch == HS_HORIZONTAL ? "HS_HORIZONTAL" : dt->lb.lbHatch == HS_VERTICAL ? "HS_VERTICAL" : "???") : "");
   } break;
   case EMR_CREATECOLORSPACEW: {
      const EMRCREATECOLORSPACEW *dt = (const EMRCREATECOLORSPACEW *) lpEMFR;
      fprintf(debug, "\tihCS:%d (0x%x) lcsSignature: 0x%x(%s) lcsVersion:0x%x lcsSize:%d lcsCSType:0x%x lcsIntent:0x%x lcsGammaRed:0x%x lcsGammaGreen:0x%x lcsGammaBlue:0x%x lcsFilename:'%s' dwFlags:0x%x (%s) cbData:%d (0x%x) Data:%p\n",
         dt->ihCS, dt->ihCS,
         dt->lcs.lcsSignature,
         dt->lcs.lcsVersion,
         dt->lcs.lcsSize,
         dt->lcs.lcsCSType,
         dt->lcs.lcsIntent,
         dt->lcs.lcsGammaRed,
         dt->lcs.lcsGammaGreen,
         dt->lcs.lcsGammaBlue,
         dt->lcs.lcsFilename,
         dt->dwFlags, dt->dwFlags == 0 ? "none" : dt->dwFlags == CREATECOLORSPACE_EMBEDED ? "CREATECOLORSPACE_EMBEDED" : "???",
         dt->cbData, dt->cbData,
         dt->Data);
   } break;
   case EMR_CREATECOLORSPACE: {
      const EMRCREATECOLORSPACE *dt = (const EMRCREATECOLORSPACE *) lpEMFR;
      fprintf(debug, "\tihCS:%d (0x%x) lcsCSType:0x%x lcsSize:%d lcsVersion:0x%x lcsFilename:'%s'\n", dt->ihCS, dt->ihCS, dt->lcs.lcsSize, dt->lcs.lcsVersion, dt->lcs.lcsCSType, dt->lcs.lcsFilename);
   } break;
   case EMR_CREATEDIBPATTERNBRUSHPT: {
      const EMRCREATEDIBPATTERNBRUSHPT *dt = (const EMRCREATEDIBPATTERNBRUSHPT *) lpEMFR;
      fprintf(debug, "\tihBrush:%d (0x%x) iUsage:0x%x (%s) offBmi:%d cbBmi:%d offBits:%d cbBits:%d\n", dt->ihBrush, dt->ihBrush, dt->iUsage, dt->iUsage == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsage == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsage == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmi, dt->cbBmi, dt->offBits, dt->cbBits);
      if (dt->offBmi) {
         dumpBitmapInfo(debug, "Bmi", STRUCT_OFFSET (lpEMFR, dt->offBmi));
      }
   } break;
   case EMR_CREATEMONOBRUSH: {
      const EMRCREATEMONOBRUSH *dt = (const EMRCREATEMONOBRUSH *) lpEMFR;
      fprintf(debug, "\tihBrush:%d (%x) iUsage:%x (%s) offBmi:%d cbBmi:%d offBits:%d cbBits:%d\n", dt->ihBrush, dt->ihBrush, dt->iUsage, dt->iUsage == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsage == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsage == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmi, dt->cbBmi, dt->offBits, dt->cbBits);
      if (dt->offBmi) {
         dumpBitmapInfo(debug, "Bmi", STRUCT_OFFSET (lpEMFR, dt->offBmi));
      }
   } break;
   case EMR_CREATEPALETTE: {
      const EMRCREATEPALETTE *dt = (const EMRCREATEPALETTE *) lpEMFR;
      fprintf(debug, "\tihPal:%d (0x%x) palVersion:0x%x palNumEntries:%d\n", dt->ihPal, dt->ihPal, dt->lgpl.palVersion, dt->lgpl.palNumEntries);
   } break;
   case EMR_CREATEPEN: {
      const EMRCREATEPEN *dt = (const EMRCREATEPEN *) lpEMFR;
      fprintf(debug, "\tihPen:%d (0x%x) lopnColor:0x%06x lopnStyle:0x%x %s lopnWidth:[%d,%d]\n", dt->ihPen, dt->ihPen, dt->lopn.lopnColor, dt->lopn.lopnStyle,
         dt->lopn.lopnStyle == PS_SOLID ? "PS_SOLID" : dt->lopn.lopnStyle == PS_DASH ? "PS_DASH" : dt->lopn.lopnStyle == PS_DOT ? "PS_DOT" : dt->lopn.lopnStyle == PS_DASHDOT ? "PS_DASHDOT" : dt->lopn.lopnStyle == PS_DASHDOTDOT ? "PS_DASHDOTDOT" : dt->lopn.lopnStyle == PS_NULL ? "PS_NULL" : dt->lopn.lopnStyle == PS_INSIDEFRAME ? "PS_INSIDEFRAME" : "???",
         dt->lopn.lopnWidth.x, dt->lopn.lopnWidth.y);
   } break;
   case EMR_DELETECOLORSPACE: {
      const EMRSELECTCOLORSPACE *dt = (const EMRSELECTCOLORSPACE *) lpEMFR;
      fprintf(debug, "\tihCS:%d (0x%x)\n", dt->ihCS, dt->ihCS);
   } break;
   case EMR_DELETEOBJECT: {
      const EMRDELETEOBJECT *dt = (const EMRDELETEOBJECT *) lpEMFR;
      fprintf (debug, "\tihObject:%d (0x%x)\n", dt->ihObject, dt->ihObject);
   } break;
   case EMR_ELLIPSE: {
      const EMRELLIPSE *dt = (const EMRELLIPSE *) lpEMFR;
      fprintf(debug, "\trclBox:[%d,%d,%d,%d]\n", dt->rclBox.left, dt->rclBox.top, dt->rclBox.right, dt->rclBox.bottom);
   } break;
   case EMR_EOF: {
      const EMREOF *dt = (const EMREOF *) lpEMFR;
      fprintf(debug, "\tnPalEntries:%d offPalEntries:%d nSizeLast:%d\n", dt->nPalEntries, dt->offPalEntries, dt->nSizeLast);
   } break;
   case EMR_EXCLUDECLIPRECT: {
      const EMREXCLUDECLIPRECT *dt = (const EMREXCLUDECLIPRECT *) lpEMFR;
      fprintf(debug, "\trclClip:[%d,%d,%d,%d]\n", dt->rclClip.left, dt->rclClip.top, dt->rclClip.right, dt->rclClip.bottom);
   } break;
   case EMR_EXTCREATEFONTINDIRECTW: {
      const EMREXTCREATEFONTINDIRECTW *dt = (const EMREXTCREATEFONTINDIRECTW *) lpEMFR;
      fprintf(debug, "\tihFont:%d (0x%x) elfVersion:0x%x elfStyleSize:0x%x elfMatch:0x%x lfHeight:%d lfWidth:%d lfEscapement:%d lfOrientation:%d lfWeight:%d lfItalic:%d lfUnderline:%d lfStrikeOut:%d lfCharSet:0x%d lfOutPrecision:0x%d lfClipPrecision:0x%d lfQuality:0x%x lfPitchAndFamily:0x%x lfFaceName:'%S'\n", dt->ihFont, dt->ihFont, dt->elfw.elfVersion, dt->elfw.elfStyleSize, dt->elfw.elfMatch, dt->elfw.elfLogFont.lfHeight, dt->elfw.elfLogFont.lfWidth, dt->elfw.elfLogFont.lfEscapement, dt->elfw.elfLogFont.lfOrientation, dt->elfw.elfLogFont.lfWeight, dt->elfw.elfLogFont.lfItalic, dt->elfw.elfLogFont.lfUnderline, dt->elfw.elfLogFont.lfStrikeOut, dt->elfw.elfLogFont.lfCharSet, dt->elfw.elfLogFont.lfOutPrecision, dt->elfw.elfLogFont.lfClipPrecision, dt->elfw.elfLogFont.lfQuality, dt->elfw.elfLogFont.lfPitchAndFamily, dt->elfw.elfLogFont.lfFaceName);
   } break;
   case EMR_EXTCREATEPEN: {
      const EMREXTCREATEPEN *dt = (const EMREXTCREATEPEN *) lpEMFR;
      fprintf(debug, "\tihPen:%d (0x%x) offBmi:%d cbBmi:%d offBits:%d cbBits:%d elpPenStyle:0x%x %s/%s/%s/%s elpWidth:%d elpBrushStyle:0x%x %s elpColor:0x%06x elpHatch:0x%x elpNumEntries:%d",
         dt->ihPen, dt->ihPen, dt->offBmi, dt->cbBmi, dt->offBits, dt->cbBits, dt->elp.elpPenStyle,
         (dt->elp.elpPenStyle & PS_TYPE_MASK) == PS_GEOMETRIC ? "PS_GEOMETRIC" : (dt->elp.elpPenStyle & PS_TYPE_MASK) == PS_COSMETIC ? "PS_COSMETIC" : "???",
         (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_SOLID ? "PS_SOLID" : (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_DASH ? "PS_DASH" : (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_DOT ? "PS_DOT" : (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_DASHDOT ? "PS_DASHDOT" : (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_DASHDOTDOT ? "PS_DASHDOTDOT" : (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_NULL ? "PS_NULL" : (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_USERSTYLE ? "PS_USERSTYLE" : (dt->elp.elpPenStyle & PS_STYLE_MASK) == PS_INSIDEFRAME ? "PS_INSIDEFRAME" : "???",
         (dt->elp.elpPenStyle & PS_ENDCAP_MASK) == PS_ENDCAP_ROUND ? "PS_ENDCAP_ROUND" : (dt->elp.elpPenStyle & PS_ENDCAP_MASK) == PS_ENDCAP_SQUARE ? "PS_ENDCAP_SQUARE" : (dt->elp.elpPenStyle & PS_ENDCAP_MASK) == PS_ENDCAP_FLAT ? "PS_ENDCAP_FLAT" : "???", // PS_GEOMETRIC pens only
         (dt->elp.elpPenStyle & PS_JOIN_MASK) == PS_JOIN_BEVEL ? "PS_JOIN_BEVEL" : (dt->elp.elpPenStyle & PS_JOIN_MASK) == PS_JOIN_MITER ? "PS_JOIN_MITER" : (dt->elp.elpPenStyle & PS_JOIN_MASK) == PS_JOIN_ROUND ? "PS_JOIN_ROUND" : "???", // PS_GEOMETRIC pens only
         dt->elp.elpWidth, dt->elp.elpBrushStyle,
         dt->elp.elpBrushStyle == BS_DIBPATTERN ? "BS_DIBPATTERN" : dt->elp.elpBrushStyle == BS_DIBPATTERNPT ? "BS_DIBPATTERNPT" : dt->elp.elpBrushStyle == BS_HATCHED ? "BS_HATCHED" : dt->elp.elpBrushStyle == BS_HOLLOW ? "BS_HOLLOW" : dt->elp.elpBrushStyle == BS_PATTERN ? "BS_PATTERN" : dt->elp.elpBrushStyle == BS_SOLID ? "BS_SOLID" : "???",
         dt->elp.elpColor, dt->elp.elpHatch, dt->elp.elpNumEntries);

      if (dt->offBmi) {
         dumpBitmapInfo(debug, "Bmi", STRUCT_OFFSET (lpEMFR, dt->offBmi));
      }
      if (dt->elp.elpNumEntries > 0) {
         DWORD ii;

         fprintf(debug, "elpStyleEntry[]: ");
         for (ii = 0; ii < dt->elp.elpNumEntries; ii++) {
            if (ii > 0) {
               fprintf(debug, ",");
            }

            fprintf(debug, "%d", dt->elp.elpStyleEntry[ii]);
         }
      }

      fprintf(debug, "\n");
   } break;
   case EMR_EXTFLOODFILL: {
      const EMREXTFLOODFILL *dt = (const EMREXTFLOODFILL *) lpEMFR;
      fprintf(debug, "\tptlStart:%d,%d crColor:0x%06x iMode:0x%x (%s)\n", dt->ptlStart.x, dt->ptlStart.y, dt->crColor, dt->iMode, dt->iMode == FLOODFILLBORDER ? "FLOODFILLBORDER" : dt->iMode == FLOODFILLSURFACE ? "FLOODFILLSURFACE" : "???");
   } break;
   case EMR_EXTSELECTCLIPRGN: {
      const EMREXTSELECTCLIPRGN *dt = (const EMREXTSELECTCLIPRGN *) lpEMFR;
      fprintf(debug, "\tcbRgnData:%d iMode:0x%x (%s)\n", dt->cbRgnData, dt->iMode, dt->iMode == RGN_AND ? "RGN_AND" : dt->iMode == RGN_COPY ? "RGN_COPY" : dt->iMode == RGN_DIFF ? "RGN_DIFF" : dt->iMode == RGN_OR ? "RGN_OR" : dt->iMode == RGN_XOR ? "RGN_XOR" : "???");
      if (dt->cbRgnData != 0) {
         dumpRgnData(debug, (const RGNDATA *) dt->RgnData);
      }
   } break;
   case EMR_EXTTEXTOUTA:
   case EMR_EXTTEXTOUTW: {
      const EMREXTTEXTOUTW *dt = (const EMREXTTEXTOUTW *) lpEMFR;
      fprintf (debug, "\toptions:%x (%s) nChars:%d offDx:%d offString:%d, ptlReference:%d,%d rcl:[%d,%d,%d,%d]\n", dt->emrtext.fOptions, (dt->emrtext.fOptions == (ETO_CLIPPED | ETO_OPAQUE)) ? "ETO_CLIPPED | ETO_OPAQUE" : (dt->emrtext.fOptions == ETO_CLIPPED) ? "ETO_CLIPPED" : (dt->emrtext.fOptions == ETO_OPAQUE) ? "ETO_OPAQUE" : (dt->emrtext.fOptions == 0) ? "" : "???",
         dt->emrtext.nChars, dt->emrtext.offDx, dt->emrtext.offString, dt->emrtext.ptlReference.x, dt->emrtext.ptlReference.y, dt->emrtext.rcl.left, dt->emrtext.rcl.top, dt->emrtext.rcl.right, dt->emrtext.rcl.bottom);
      if (dt->emrtext.nChars > 0) {
         DWORD bufflen = 2 + dt->emrtext.nChars * 2;
         char *buff = (char *) malloc(sizeof(char) * bufflen);
         if (buff && UnicodeToAnsi((const LPWSTR) STRUCT_OFFSET (dt, dt->emrtext.offString), dt->emrtext.nChars, buff, &bufflen)) {
            fprintf(debug, "\tstring[%d]:'%s'\n", bufflen, buff);
         } else {
            fprintf(debug, "\tfailed to convert string; buff:%p bufflen:%d\n", buff, bufflen);
         }
         DWORD *Dx = (DWORD *) STRUCT_OFFSET (dt, dt->emrtext.offDx);
         if ((BYTE *) Dx < (BYTE *) (dt + lpEMFR->nSize) && (BYTE *) Dx + (sizeof (DWORD) * dt->emrtext.nChars) <= (BYTE *) (dt + lpEMFR->nSize)) {
            DWORD ii;

            fprintf (debug,"\tDx:");
            for (ii = 0; ii < dt->emrtext.nChars; ii++) {
               if (ii) {
                  fprintf (debug,",");
               }
               fprintf(debug, "%d", Dx[ii]);
            }
            fprintf (debug,"\n");
         }
         if (buff) free(buff);
      }
      fprintf (debug, "\texScale:%g eyScale:%g graphicsMode:%d (%s) bounds:[%d,%d,%d,%d]\n", dt->exScale, dt->eyScale, dt->iGraphicsMode, dt->iGraphicsMode == GM_COMPATIBLE ? "GM_COMPATIBLE" : dt->iGraphicsMode == GM_ADVANCED ? "GM_ADVANCED" : "???", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom);
   } break;
   case EMR_FILLPATH: {
      const EMRFILLPATH *dt = (const EMRFILLPATH *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom);
   } break;
   case EMR_FILLRGN: {
      const EMRFILLRGN *dt = (const EMRFILLRGN *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] ihBrush:%d (0x%x) cbRgnData:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->ihBrush, dt->ihBrush, dt->cbRgnData);
      dumpRgnData(debug, (const RGNDATA *) dt->RgnData);
   } break;
   case EMR_FRAMERGN: {
      const EMRFRAMERGN *dt = (const EMRFRAMERGN *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cbRgnData:%d ihBrush:%d (0x%x) szlStroke:[%d,%d]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cbRgnData, dt->ihBrush, dt->ihBrush, dt->szlStroke.cx, dt->szlStroke.cy);
      dumpRgnData(debug, (const RGNDATA *) dt->RgnData);
   } break;
   case EMR_GDICOMMENT: {
      const EMRGDICOMMENT *dt = (const EMRGDICOMMENT *) lpEMFR;
      fprintf(debug, "\tcbData:%d\n", dt->cbData);
   } break;
   case EMR_GLSBOUNDEDRECORD: {
      const EMRGLSBOUNDEDRECORD *dt = (const EMRGLSBOUNDEDRECORD *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cbData:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cbData);
   } break;
   case EMR_GLSRECORD: {
      const EMRGLSRECORD *dt = (const EMRGLSRECORD *) lpEMFR;
      fprintf(debug, "\tcbData:%d\n", dt->cbData);
   } break;
   case EMR_GRADIENTFILL: {
      const EMRGRADIENTFILL *dt = (const EMRGRADIENTFILL *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] nVer:%d nTri:%d ulMode:0x%x (%s)\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->nVer, dt->nTri, dt->ulMode, dt->ulMode == GRADIENT_FILL_RECT_H ? "GRADIENT_FILL_RECT_H" : dt->ulMode == GRADIENT_FILL_RECT_V ? "GRADIENT_FILL_RECT_V" : dt->ulMode == GRADIENT_FILL_TRIANGLE ? "GRADIENT_FILL_TRIANGLE" : "???");
   } break;
   case EMR_INTERSECTCLIPRECT: {
      const EMRINTERSECTCLIPRECT *dt = (const EMRINTERSECTCLIPRECT *) lpEMFR;
      fprintf(debug, "\trclClip:[%d,%d,%d,%d]\n", dt->rclClip.left, dt->rclClip.top, dt->rclClip.right, dt->rclClip.bottom);
   } break;
   case EMR_INVERTRGN: {
      const EMRINVERTRGN *dt = (const EMRINVERTRGN *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cbRgnData:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cbRgnData);
      dumpRgnData(debug, (const RGNDATA *) dt->RgnData);
   } break;
   case EMR_LINETO: {
      const EMRLINETO *dt = (const EMRLINETO *) lpEMFR;
      fprintf (debug, "\tpoint:%d,%d\n", dt->ptl.x, dt->ptl.y);
   } break;
   case EMR_MASKBLT: {
      const EMRMASKBLT *dt = (const EMRMASKBLT *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] dest:[%d,%d,%d,%d] dwRop:0x%x src:%d,%d xformSrc:[%g;%g;%g;%g;%g;%g]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->xSrc, dt->ySrc, dt->xformSrc.eM11, dt->xformSrc.eM12, dt->xformSrc.eM21, dt->xformSrc.eM22, dt->xformSrc.eDx, dt->xformSrc.eDy);
      fprintf(debug, "\tcrBkColorSrc:0x%06x iUsageSrc:0x%x(%s) offBmiSrc:%d cbBmiSrc:%d offBitsSrc:%d cbBitsSrc:%d\n", dt->crBkColorSrc, dt->iUsageSrc, dt->iUsageSrc == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageSrc == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageSrc == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc);
      fprintf(debug, "\tMask:%d,%d iUsageMask:0x%x(%s) offBmiMask:%d cbBmiMask:%d offBitsMask:%d cbBitsMask:%d\n", dt->xMask, dt->yMask, dt->iUsageMask, dt->iUsageMask == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageMask == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageMask == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmiMask, dt->cbBmiMask, dt->offBitsMask, dt->cbBitsMask);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
      if (dt->offBmiMask) {
         dumpBitmapInfo(debug, "BmiMask", STRUCT_OFFSET (lpEMFR, dt->offBmiMask));
      }
   } break;
   case EMR_MODIFYWORLDTRANSFORM: {
      const EMRMODIFYWORLDTRANSFORM *dt = (const EMRMODIFYWORLDTRANSFORM *) lpEMFR;
      fprintf(debug, "\txform:[%g;%g;%g;%g;%g;%g] iMode:0x%x(%s)\n", dt->xform.eM11, dt->xform.eM12, dt->xform.eM21, dt->xform.eM22, dt->xform.eDx, dt->xform.eDy, dt->iMode, dt->iMode == MWT_IDENTITY ? "MWT_IDENTITY": dt->iMode == MWT_LEFTMULTIPLY ? "MWT_LEFTMULTIPLY" : dt->iMode == MWT_RIGHTMULTIPLY ? "MWT_RIGHTMULTIPLY" : "???");
   } break;
   case EMR_MOVETOEX: {
      const EMRMOVETOEX *dt = (const EMRMOVETOEX *) lpEMFR;
      fprintf (debug, "\tpoint:%d,%d\n", dt->ptl.x, dt->ptl.y);
   } break;
   case EMR_OFFSETCLIPRGN: {
      const EMROFFSETCLIPRGN *dt = (const EMROFFSETCLIPRGN *) lpEMFR;
      fprintf(debug, "\tptlOffset:%d,%d\n", dt->ptlOffset.x, dt->ptlOffset.y);
   } break;
   case EMR_PAINTRGN: {
      const EMRPAINTRGN *dt = (const EMRPAINTRGN *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cbRgnData:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cbRgnData);
      dumpRgnData(debug, (const RGNDATA *) dt->RgnData);
   } break;
   case EMR_PIE: {
      const EMRPIE *dt = (const EMRPIE *) lpEMFR;
      fprintf(debug, "\trclBox:[%d,%d,%d,%d] ptlStart:%d,%d ptlEnd:%d,%d\n", dt->rclBox.left, dt->rclBox.top, dt->rclBox.right, dt->rclBox.bottom, dt->ptlStart.x, dt->ptlStart.y, dt->ptlEnd.x, dt->ptlEnd.y);
   } break;
   case EMR_PIXELFORMAT: {
      const EMRPIXELFORMAT *dt = (const EMRPIXELFORMAT *) lpEMFR;
      fprintf(debug, "\tnSize:%d nVersion:0x%x\n", dt->pfd.nSize, dt->pfd.nVersion);
   } break;
   case EMR_PLGBLT: {
      const EMRPLGBLT *dt = (const EMRPLGBLT *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] aptlDest:[%d,%d][%d,%d][%d,%d] src:%d,%d,%d,%d xformSrc:[%g;%g;%g;%g;%g;%g]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->aptlDest[0].x, dt->aptlDest[0].y, dt->aptlDest[1].x, dt->aptlDest[1].y, dt->aptlDest[2].x, dt->aptlDest[2].y, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xformSrc.eM11, dt->xformSrc.eM12, dt->xformSrc.eM21, dt->xformSrc.eM22, dt->xformSrc.eDx, dt->xformSrc.eDy);
      fprintf(debug, "\tcrBkColorSrc:0x%06x iUsageSrc:0x%x(%s) offBmiSrc:%d cbBmiSrc:%d offBitsSrc:%d cbBitsSrc:%d\n", dt->crBkColorSrc, dt->iUsageSrc, dt->iUsageSrc == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageSrc == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageSrc == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc);
      fprintf(debug, "\tMask:%d,%d iUsageMask:0x%x(%s) offBmiMask:%d cbBmiMask:%d offBitsMask:%d cbBitsMask:%d\n", dt->xMask, dt->yMask, dt->iUsageMask, dt->iUsageMask == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageMask == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageMask == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmiMask, dt->cbBmiMask, dt->offBitsMask, dt->cbBitsMask);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
      if (dt->offBmiMask) {
         dumpBitmapInfo(debug, "BmiMask", STRUCT_OFFSET (lpEMFR, dt->offBmiMask));
      }
   } break;
   case EMR_POLYBEZIER: {
      const EMRPOLYBEZIER *dt = (const EMRPOLYBEZIER *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cptl:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cptl);
      DWORD ii;
      for (ii = 0; ii < dt->cptl; ii++) {
         fprintf(debug, " [%d,%d]", dt->aptl[ii].x, dt->aptl[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYBEZIER16: {
      const EMRPOLYBEZIER16 *dt = (const EMRPOLYBEZIER16 *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cpts:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cpts);
      DWORD ii;
      for (ii = 0; ii < dt->cpts; ii++) {
         fprintf(debug, " [%d,%d]", dt->apts[ii].x, dt->apts[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYBEZIERTO: {
      const EMRPOLYBEZIERTO *dt = (const EMRPOLYBEZIERTO *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cptl:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cptl);
      DWORD ii;
      for (ii = 0; ii < dt->cptl; ii++) {
         fprintf(debug, " [%d,%d]", dt->aptl[ii].x, dt->aptl[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYBEZIERTO16: {
      const EMRPOLYBEZIERTO16 *dt = (const EMRPOLYBEZIERTO16 *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cpts:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cpts);
      DWORD ii;
      for (ii = 0; ii < dt->cpts; ii++) {
         fprintf(debug, " [%d,%d]", dt->apts[ii].x, dt->apts[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYDRAW: {
      const EMRPOLYDRAW *dt = (const EMRPOLYDRAW *) lpEMFR;
      const BYTE *types = (const BYTE *) STRUCT_OFFSET(dt->aptl, dt->cptl * sizeof(dt->aptl[0]));
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cptl:%d%s", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cptl, types == dt->abTypes ? "" : " (Fixing bug in abTypes)");
      DWORD ii;
      for (ii = 0; ii < dt->cptl; ii++) {
         BYTE striptype = types[ii] & (~PT_CLOSEFIGURE);
         fprintf(debug, " [%d,%d; 0x%x(%s%s)]", dt->aptl[ii].x, dt->aptl[ii].y, types[ii], striptype == PT_MOVETO ? "PT_MOVETO" : striptype == PT_LINETO ? "PT_LINETO" : striptype == PT_BEZIERTO ? "PT_BEZIERTO" : "???", types[ii] & PT_CLOSEFIGURE ? " | PT_CLOSEFIGURE" : "");
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYDRAW16: {
      const EMRPOLYDRAW16 *dt = (const EMRPOLYDRAW16 *) lpEMFR;
      const BYTE *types = (const BYTE *) STRUCT_OFFSET(dt->apts, dt->cpts * sizeof(dt->apts[0]));
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cpts:%d%s", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cpts, types == dt->abTypes ? "" : " (Fixing bug in abTypes)");
      DWORD ii;
      for (ii = 0; ii < dt->cpts; ii++) {
         BYTE striptype = types[ii] & (~PT_CLOSEFIGURE);
         fprintf(debug, " [%d,%d; 0x%x(%s%s)]", dt->apts[ii].x, dt->apts[ii].y, types[ii], striptype == PT_MOVETO ? "PT_MOVETO" : striptype == PT_LINETO ? "PT_LINETO" : striptype == PT_BEZIERTO ? "PT_BEZIERTO" : "???", types[ii] & PT_CLOSEFIGURE ? " | PT_CLOSEFIGURE" : "");
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYGON: {
      const EMRPOLYGON *dt = (const EMRPOLYGON *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cptl:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cptl);
      DWORD ii;
      for (ii = 0; ii < dt->cptl; ii++) {
         fprintf(debug, " [%d,%d]", dt->aptl[ii].x, dt->aptl[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYGON16: {
      const EMRPOLYGON16 *dt = (const EMRPOLYGON16 *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cpts:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cpts);
      DWORD ii;
      for (ii = 0; ii < dt->cpts; ii++) {
         fprintf(debug, " [%d,%d]", dt->apts[ii].x, dt->apts[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYLINE: {
      const EMRPOLYLINE *dt = (const EMRPOLYLINE *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cptl:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cptl);
      DWORD ii;
      for (ii = 0; ii < dt->cptl; ii++) {
         fprintf(debug, " [%d,%d]", dt->aptl[ii].x, dt->aptl[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYLINE16: {
      const EMRPOLYLINE16 *dt = (const EMRPOLYLINE16 *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cpts:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cpts);
      DWORD ii;
      for (ii = 0; ii < dt->cpts; ii++) {
         fprintf(debug, " [%d,%d]", dt->apts[ii].x, dt->apts[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYLINETO: {
      const EMRPOLYLINETO *dt = (const EMRPOLYLINETO *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cptl:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cptl);
      DWORD ii;
      for (ii = 0; ii < dt->cptl; ii++) {
         fprintf(debug, " [%d,%d]", dt->aptl[ii].x, dt->aptl[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYLINETO16: {
      const EMRPOLYLINETO16 *dt = (const EMRPOLYLINETO16 *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] cpts:%d", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->cpts);
      DWORD ii;
      for (ii = 0; ii < dt->cpts; ii++) {
         fprintf(debug, " [%d,%d]", dt->apts[ii].x, dt->apts[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYPOLYGON: {
      const EMRPOLYPOLYGON *dt = (const EMRPOLYPOLYGON *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] nPolys:%d cptl:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->nPolys, dt->cptl);
      DWORD ii;
      fprintf(debug, "\taPolyCounts[]: ");
      for (ii = 0; ii < dt->nPolys; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "%d", dt->aPolyCounts[ii]);
      }
      fprintf(debug, "\n\taptl[]: ");
      for (ii = 0; ii < dt->cptl; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "[%d,%d]", dt->aptl[ii].x, dt->aptl[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYPOLYGON16: {
      const EMRPOLYPOLYGON16 *dt = (const EMRPOLYPOLYGON16 *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] nPolys:%d cptl:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->nPolys, dt->cpts);
      DWORD ii;
      fprintf(debug, "\taPolyCounts[]: ");
      for (ii = 0; ii < dt->nPolys; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "%d", dt->aPolyCounts[ii]);
      }
      fprintf(debug, "\n\tapts[]: ");
      for (ii = 0; ii < dt->cpts; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "[%d,%d]", dt->apts[ii].x, dt->apts[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYPOLYLINE: {
      const EMRPOLYPOLYLINE *dt = (const EMRPOLYPOLYLINE *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] nPolys:%d cptl:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->nPolys, dt->cptl);
      DWORD ii;
      fprintf(debug, "\taPolyCounts[]: ");
      for (ii = 0; ii < dt->nPolys; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "%d", dt->aPolyCounts[ii]);
      }
      fprintf(debug, "\n\taptl[]: ");
      for (ii = 0; ii < dt->cptl; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "[%d,%d]", dt->aptl[ii].x, dt->aptl[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYPOLYLINE16: {
      const EMRPOLYPOLYLINE16 *dt = (const EMRPOLYPOLYLINE16 *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] nPolys:%d cptl:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->nPolys, dt->cpts);
      DWORD ii;
      fprintf(debug, "\taPolyCounts[]: ");
      for (ii = 0; ii < dt->nPolys; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "%d", dt->aPolyCounts[ii]);
      }
      fprintf(debug, "\n\tapts[]: ");
      for (ii = 0; ii < dt->cpts; ii++) {
         if (ii != 0) {
            fprintf(debug, ", ");
         }
         fprintf(debug, "[%d,%d]", dt->apts[ii].x, dt->apts[ii].y);
      }
      fprintf(debug, "\n");
   } break;
   case EMR_POLYTEXTOUTA: {
      const EMRPOLYTEXTOUTA *dt = (const EMRPOLYTEXTOUTA *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] iGraphicsMode:0x%x(%s) eScale:%g;%g cStrings:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->iGraphicsMode, dt->iGraphicsMode == GM_COMPATIBLE ? "GM_COMPATIBLE" : dt->iGraphicsMode == GM_ADVANCED ? "GM_ADVANCED" : "???", dt->exScale, dt->eyScale, dt->cStrings);
   } break;
   case EMR_POLYTEXTOUTW: {
      const EMRPOLYTEXTOUTW *dt = (const EMRPOLYTEXTOUTW *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] iGraphicsMode:0x%x(%s) eScale:%g;%g cStrings:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->iGraphicsMode, dt->iGraphicsMode == GM_COMPATIBLE ? "GM_COMPATIBLE" : dt->iGraphicsMode == GM_ADVANCED ? "GM_ADVANCED" : "???", dt->exScale, dt->eyScale, dt->cStrings);
   } break;
   case EMR_RECTANGLE: {
      const EMRRECTANGLE *dt = (const EMRRECTANGLE *) lpEMFR;
      fprintf(debug, "\trclBox:[%d,%d,%d,%d]\n", dt->rclBox.left, dt->rclBox.top, dt->rclBox.right, dt->rclBox.bottom);
   } break;
   case EMR_RESIZEPALETTE: {
      const EMRRESIZEPALETTE *dt = (const EMRRESIZEPALETTE *) lpEMFR;
      fprintf(debug, "\tihPal:%d(0x%x) cEntries:%d\n", dt->ihPal, dt->ihPal, dt->cEntries);
   } break;
   case EMR_RESTOREDC: {
      const EMRRESTOREDC *dt = (const EMRRESTOREDC *) lpEMFR;
      fprintf(debug, "\tiRelative:%d(0x%x)\n", dt->iRelative, dt->iRelative);
   } break;
   case EMR_ROUNDRECT: {
      const EMRROUNDRECT *dt = (const EMRROUNDRECT *) lpEMFR;
      fprintf(debug, "\trclBox:[%d,%d,%d,%d] szlCorner:%d,%d\n", dt->rclBox.left, dt->rclBox.top, dt->rclBox.right, dt->rclBox.bottom, dt->szlCorner.cx, dt->szlCorner.cy);
   } break;
   case EMR_SCALEVIEWPORTEXTEX: {
      const EMRSCALEVIEWPORTEXTEX *dt = (const EMRSCALEVIEWPORTEXTEX *) lpEMFR;
      fprintf(debug, "\txNum:%d xDenom:%d yNum:%d yDenom:%d\n", dt->xNum, dt->xDenom, dt->yNum, dt->yDenom);
   } break;
   case EMR_SCALEWINDOWEXTEX: {
      const EMRSCALEWINDOWEXTEX *dt = (const EMRSCALEWINDOWEXTEX *) lpEMFR;
      fprintf(debug, "\txNum:%d xDenom:%d yNum:%d yDenom:%d\n", dt->xNum, dt->xDenom, dt->yNum, dt->yDenom);
   } break;
   case EMR_SELECTCLIPPATH: {
      const EMRSELECTCLIPPATH *dt = (const EMRSELECTCLIPPATH *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == RGN_AND ? "RGN_AND" : dt->iMode == RGN_COPY ? "RGN_COPY" : dt->iMode == RGN_DIFF ? "RGN_DIFF" : dt->iMode == RGN_OR ? "RGN_OR" : dt->iMode == RGN_XOR ? "RGN_XOR" : "???");
   } break;
   case EMR_SELECTOBJECT: {
      const EMRSELECTOBJECT *dt = (const EMRSELECTOBJECT *) lpEMFR;
      fprintf(debug, "\tihObject:%d (0x%x)", dt->ihObject, dt->ihObject);
      if ((dt->ihObject & ENHMETA_STOCK_OBJECT) == ENHMETA_STOCK_OBJECT) {
         DWORD stockID = dt->ihObject & (~ENHMETA_STOCK_OBJECT);
         bool have = false;
         #define ex(x) if (!have && stockID == x) { fprintf(debug, " %s", #x); have = true; }
         ex(BLACK_BRUSH);
         ex(DKGRAY_BRUSH);
         //ex(DC_BRUSH);
         ex(GRAY_BRUSH);
         ex(LTGRAY_BRUSH);
         ex(NULL_BRUSH);
         ex(HOLLOW_BRUSH);
         ex(WHITE_BRUSH);
         ex(BLACK_PEN);
         //ex(DC_PEN);
         ex(NULL_PEN);
         ex(WHITE_PEN);
         ex(ANSI_FIXED_FONT);
         ex(ANSI_VAR_FONT);
         ex(DEVICE_DEFAULT_FONT);
         ex(DEFAULT_GUI_FONT);
         ex(OEM_FIXED_FONT);
         ex(SYSTEM_FONT);
         ex(SYSTEM_FIXED_FONT);
         ex(DEFAULT_PALETTE);
         #undef ex
      }
      fprintf(debug, "\n");
   } break;
   case EMR_SELECTPALETTE: {
      const EMRSELECTPALETTE *dt = (const EMRSELECTPALETTE *) lpEMFR;
      fprintf(debug, "\tihPal:%d (0x%x)\n", dt->ihPal, dt->ihPal);
   } break;
   case EMR_SETARCDIRECTION: {
      const EMRSETARCDIRECTION *dt = (const EMRSETARCDIRECTION *) lpEMFR;
      fprintf(debug, "\tiArcDirection:0x%x (%s)\n", dt->iArcDirection, dt->iArcDirection == AD_CLOCKWISE ? "AD_CLOCKWISE" : dt->iArcDirection == AD_COUNTERCLOCKWISE ? "AD_COUNTERCLOCKWISE" : "???");
   } break;
   case EMR_SETBKCOLOR: {
      const EMRSETBKCOLOR *dt = (const EMRSETBKCOLOR *) lpEMFR;
      fprintf(debug, "\tcrColor:0x%06x\n", dt->crColor);
   } break;
   case EMR_SETBKMODE: {
      const EMRSETBKMODE *dt = (const EMRSETBKMODE *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == OPAQUE ? "OPAQUE" : dt->iMode == TRANSPARENT ? "TRANSPARENT" : "???");
   } break;
   case EMR_SETBRUSHORGEX: {
      const EMRSETBRUSHORGEX *dt = (const EMRSETBRUSHORGEX *) lpEMFR;
      fprintf(debug, "\tptlOrigin:%d,%d\n", dt->ptlOrigin.x, dt->ptlOrigin.y);
   } break;
   case EMR_SETCOLORADJUSTMENT: {
      const EMRSETCOLORADJUSTMENT *dt = (const EMRSETCOLORADJUSTMENT *) lpEMFR;
      fprintf(debug, "\tcaSize:%d\n", dt->ColorAdjustment.caSize);
   } break;
   case EMR_SETCOLORSPACE: {
      const EMRSETCOLORSPACE *dt = (const EMRSETCOLORSPACE *) lpEMFR;
      fprintf(debug, "\tihCS:%d (0x%x)\n", dt->ihCS, dt->ihCS);
   } break;
   case EMR_SETDIBITSTODEVICE: {
      const EMRSETDIBITSTODEVICE *dt = (const EMRSETDIBITSTODEVICE *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] Dest:%d,%d Src:%d,%d,%d,%d offBmiSrc:%d cbBmiSrc:%d offBitsSrc:%d cbBitsSrc:%d iUsageSrc:0x%x (%s) iStartScan:%d cScans:%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->xDest, dt->yDest, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc, dt->iUsageSrc, dt->iUsageSrc == DIB_PAL_COLORS ? "DIB_PAL_COLORS" /*: dt->iUsageSrc == DIB_PAL_INDICES ? "DIB_PAL_INDICES"*/ : dt->iUsageSrc == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->iStartScan, dt->cScans);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
   } break;
   case EMR_SETICMMODE: {
      const EMRSETICMMODE *dt = (const EMRSETICMMODE *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == ICM_ON ? "ICM_ON" : dt->iMode == ICM_OFF ? "ICM_OFF" : dt->iMode == ICM_QUERY ? "ICM_QUERY" : dt->iMode == ICM_DONE_OUTSIDEDC ? "ICM_DONE_OUTSIDEDC" : "???");
   } break;
   case EMR_SETICMPROFILEA: {
      const EMRSETICMPROFILEA *dt = (const EMRSETICMPROFILEA *) lpEMFR;
      fprintf(debug, "\tdwFlags:0x%x cbName:%d cbData:%d\n", dt->dwFlags, dt->cbName, dt->cbData);
   } break;
   case EMR_SETICMPROFILEW: {
      const EMRSETICMPROFILEW *dt = (const EMRSETICMPROFILEW *) lpEMFR;
      fprintf(debug, "\tdwFlags:0x%x cbName:%d cbData:%d\n", dt->dwFlags, dt->cbName, dt->cbData);
   } break;
   case EMR_SETLAYOUT: {
      const EMRSETLAYOUT *dt = (const EMRSETLAYOUT *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == LAYOUT_BITMAPORIENTATIONPRESERVED ? "LAYOUT_BITMAPORIENTATIONPRESERVED" : dt->iMode == LAYOUT_RTL ? "LAYOUT_RTL" : "???");
   } break;
   case EMR_SETMAPMODE: {
      const EMRSETMAPMODE *dt = (const EMRSETMAPMODE *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == MM_ANISOTROPIC ? "MM_ANISOTROPIC" : dt->iMode == MM_HIENGLISH ? "MM_HIENGLISH" : dt->iMode == MM_HIMETRIC ? "MM_HIMETRIC" : dt->iMode == MM_ISOTROPIC ? "MM_ISOTROPIC" : dt->iMode == MM_LOENGLISH ? "MM_LOENGLISH" : dt->iMode == MM_LOMETRIC ? "MM_LOMETRIC" : dt->iMode == MM_TEXT ? "MM_TEXT" : dt->iMode == MM_TWIPS ? "MM_TWIPS" : "???");
   } break;
   case EMR_SETMAPPERFLAGS: {
      const EMRSETMAPPERFLAGS *dt = (const EMRSETMAPPERFLAGS *) lpEMFR;
      fprintf(debug, "\tdwFlags:0x%x\n", dt->dwFlags);
   } break;
   case EMR_SETMITERLIMIT: {
      const EMRSETMITERLIMIT *dt = (const EMRSETMITERLIMIT *) lpEMFR;
      fprintf(debug, "\teMiterLimit:%g\n", dt->eMiterLimit);
   } break;
   case EMR_SETPALETTEENTRIES: {
      const EMRSETPALETTEENTRIES *dt = (const EMRSETPALETTEENTRIES *) lpEMFR;
      fprintf(debug, "\tihPal:%d (0x%x) iStart:%d cEntries:%d\n", dt->ihPal, dt->ihPal, dt->iStart, dt->cEntries);
   } break;
   case EMR_SETPIXELV: {
      const EMRSETPIXELV *dt = (const EMRSETPIXELV *) lpEMFR;
      fprintf(debug, "\tptlPixel:%d,%d crColor:0x%06x\n", dt->ptlPixel.x, dt->ptlPixel.y, dt->crColor);
   } break;
   case EMR_SETPOLYFILLMODE: {
      const EMRSETPOLYFILLMODE *dt = (const EMRSETPOLYFILLMODE *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == ALTERNATE ? "ALTERNATE" : dt->iMode == WINDING ? "WINDING" : "???");
   } break;
   case EMR_SETROP2: {
      const EMRSETROP2 *dt = (const EMRSETROP2 *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == R2_BLACK ? "R2_BLACK" : dt->iMode == R2_COPYPEN ? "R2_COPYPEN" : dt->iMode == R2_MASKNOTPEN ? "R2_MASKNOTPEN" : dt->iMode == R2_MASKPEN ? "R2_MASKPEN" : dt->iMode == R2_MASKPENNOT ? "R2_MASKPENNOT" : dt->iMode == R2_MERGENOTPEN ? "R2_MERGENOTPEN" : dt->iMode == R2_MERGEPEN ? "R2_MERGEPEN" : dt->iMode == R2_MERGEPENNOT ? "R2_MERGEPENNOT" : dt->iMode == R2_NOP ? "R2_NOP" : dt->iMode == R2_NOT ? "R2_NOT" : dt->iMode == R2_NOTCOPYPEN ? "R2_NOTCOPYPEN" : dt->iMode == R2_NOTMASKPEN ? "R2_NOTMASKPEN" : dt->iMode == R2_NOTMERGEPEN ? "R2_NOTMERGEPEN" : dt->iMode == R2_NOTXORPEN ? "R2_NOTXORPEN" : dt->iMode == R2_WHITE ? "R2_WHITE" : dt->iMode == R2_XORPEN ? "R2_XORPEN" : "???");
   } break;
   case EMR_SETSTRETCHBLTMODE: {
      const EMRSETSTRETCHBLTMODE *dt = (const EMRSETSTRETCHBLTMODE *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (%s)\n", dt->iMode, dt->iMode == BLACKONWHITE ? "BLACKONWHITE" : dt->iMode == COLORONCOLOR ? "COLORONCOLOR" : dt->iMode == HALFTONE ? "HALFTONE" : dt->iMode == WHITEONBLACK ? "WHITEONBLACK" : dt->iMode == STRETCH_ANDSCANS ? "STRETCH_ANDSCANS" : dt->iMode == STRETCH_DELETESCANS ? "STRETCH_DELETESCANS" : dt->iMode == STRETCH_HALFTONE ? "STRETCH_HALFTONE" : dt->iMode == STRETCH_ORSCANS ? "STRETCH_ORSCANS" : "???");
   } break;
   case EMR_SETTEXTALIGN: {
      const EMRSETTEXTALIGN *dt = (const EMRSETTEXTALIGN *) lpEMFR;
      fprintf(debug, "\tiMode:0x%x (", dt->iMode);
      bool have = false;
      #define ex(x) if (have) { fprintf(debug, " | "); } if (dt->iMode & (x)) {have = true; fprintf(debug,"%s", #x); }
      ex(TA_BASELINE);
      ex(TA_BOTTOM);
      ex(TA_TOP);
      ex(TA_CENTER);
      ex(TA_LEFT);
      ex(TA_RIGHT);
      ex(TA_NOUPDATECP);
      ex(TA_RTLREADING);
      ex(TA_UPDATECP);
      ex(VTA_BASELINE);
      ex(VTA_CENTER);
      if (!have) { fprintf(debug, "TA_LEFT|TA_TOP|TA_NOUPDATECP"); }
      fprintf(debug, ")\n");
      #undef ex
   } break;
   case EMR_SETTEXTCOLOR: {
      const EMRSETTEXTCOLOR *dt = (const EMRSETTEXTCOLOR *) lpEMFR;
      fprintf(debug, "\tcrColor:0x%06x\n", dt->crColor);
   } break;
   case EMR_SETVIEWPORTEXTEX: {
      const EMRSETVIEWPORTEXTEX *dt = (const EMRSETVIEWPORTEXTEX *) lpEMFR;
      fprintf(debug, "\tszlExtent:%d,%d\n", dt->szlExtent.cx, dt->szlExtent.cy);
   } break;
   case EMR_SETVIEWPORTORGEX: {
      const EMRSETVIEWPORTORGEX *dt = (const EMRSETVIEWPORTORGEX *) lpEMFR;
      fprintf(debug, "\tptlOrigin:%d,%d\n", dt->ptlOrigin.x, dt->ptlOrigin.y);
   } break;
   case EMR_SETWINDOWEXTEX: {
      const EMRSETWINDOWEXTEX *dt = (const EMRSETWINDOWEXTEX *) lpEMFR;
      fprintf(debug, "\tszlExtent:%d,%d\n", dt->szlExtent.cx, dt->szlExtent.cy);
   } break;
   case EMR_SETWINDOWORGEX: {
      const EMRSETWINDOWORGEX *dt = (const EMRSETWINDOWORGEX *) lpEMFR;
      fprintf(debug, "\tptlOrigin:%d,%d\n", dt->ptlOrigin.x, dt->ptlOrigin.y);
   } break;
   case EMR_SETWORLDTRANSFORM: {
      const EMRSETWORLDTRANSFORM *dt = (const EMRSETWORLDTRANSFORM *) lpEMFR;
      fprintf(debug, "\txform:[%g;%g;%g;%g;%g;%g]\n", dt->xform.eM11, dt->xform.eM12, dt->xform.eM21, dt->xform.eM22, dt->xform.eDx, dt->xform.eDy);
   } break;
   case EMR_STRETCHBLT: {
      const EMRSTRETCHBLT *dt = (const EMRSTRETCHBLT *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] dest:[%d,%d,%d,%d] dwRop:0x%x src:%d,%d,%d,%d xformSrc:[%g;%g;%g;%g;%g;%g]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xformSrc.eM11, dt->xformSrc.eM12, dt->xformSrc.eM21, dt->xformSrc.eM22, dt->xformSrc.eDx, dt->xformSrc.eDy);
      fprintf(debug, "\tcrBkColorSrc:0x%06x dwRop:0x%x(%s) iUsageSrc:0x%x(%s) offBmiSrc:%d cbBmiSrc:%d offBitsSrc:%d cbBitsSrc:%d\n", dt->crBkColorSrc,
         dt->dwRop, dt->dwRop == BLACKNESS ? "BLACKNESS" : dt->dwRop == DSTINVERT ? "DSTINVERT" : dt->dwRop == MERGECOPY ? "MERGECOPY" : dt->dwRop == MERGEPAINT ? "MERGEPAINT" : dt->dwRop == NOTSRCCOPY ? "NOTSRCCOPY" : dt->dwRop == NOTSRCERASE ? "NOTSRCERASE" : dt->dwRop == PATCOPY ? "PATCOPY" :
         dt->dwRop == PATINVERT ? "PATINVERT" : dt->dwRop == PATPAINT ? "PATPAINT" : dt->dwRop == SRCAND ? "SRCAND" : dt->dwRop == SRCCOPY ? "SRCCOPY" : dt->dwRop == SRCERASE ? "SRCERASE" : dt->dwRop == SRCINVERT ? "SRCINVERT" : dt->dwRop == SRCPAINT ? "SRCPAINT" : dt->dwRop == WHITENESS ? "WHITENESS" : "???",
         dt->iUsageSrc, dt->iUsageSrc == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageSrc == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageSrc == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???",
         dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
   } break;
   case EMR_STRETCHDIBITS: {
      const EMRSTRETCHDIBITS *dt = (const EMRSTRETCHDIBITS *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] dest:[%d,%d,%d,%d] dwRop:0x%x src:%d,%d,%d,%d\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc);
      fprintf(debug, "\tiUsageSrc:0x%x(%s) offBmiSrc:%d cbBmiSrc:%d offBitsSrc:%d cbBitsSrc:%d\n", dt->iUsageSrc, dt->iUsageSrc == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageSrc == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageSrc == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
   } break;
   case EMR_STROKEANDFILLPATH: {
      const EMRSTROKEANDFILLPATH *dt = (const EMRSTROKEANDFILLPATH *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom);
   } break;
   case EMR_STROKEPATH: {
      const EMRSTROKEPATH *dt = (const EMRSTROKEPATH *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom);
   } break;
   case EMR_TRANSPARENTBLT: {
      const EMRTRANSPARENTBLT *dt = (const EMRTRANSPARENTBLT *) lpEMFR;
      fprintf(debug, "\trclBounds:[%d,%d,%d,%d] dest:[%d,%d,%d,%d] dwRop:0x%x src:%d,%d,%d,%d xformSrc:[%g;%g;%g;%g;%g;%g]\n", dt->rclBounds.left, dt->rclBounds.top, dt->rclBounds.right, dt->rclBounds.bottom, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xformSrc.eM11, dt->xformSrc.eM12, dt->xformSrc.eM21, dt->xformSrc.eM22, dt->xformSrc.eDx, dt->xformSrc.eDy);
      fprintf(debug, "\tcrBkColorSrc:0x%06x iUsageSrc:0x%x(%s) offBmiSrc:%d cbBmiSrc:%d offBitsSrc:%d cbBitsSrc:%d\n", dt->crBkColorSrc, dt->iUsageSrc, dt->iUsageSrc == DIB_PAL_COLORS ? "DIB_PAL_COLORS" : /*dt->iUsageSrc == DIB_PAL_INDICES ? "DIB_PAL_INDICES" :*/ dt->iUsageSrc == DIB_RGB_COLORS ? "DIB_RGB_COLORS" : "???", dt->offBmiSrc, dt->cbBmiSrc, dt->offBitsSrc, dt->cbBitsSrc);
      if (dt->offBmiSrc) {
         dumpBitmapInfo(debug, "BmiSrc", STRUCT_OFFSET (lpEMFR, dt->offBmiSrc));
      }
   } break;
   }
   fprintf(debug, "---\n\n");
   fclose(debug);
}

extern "C" {
static bool litePDF_jpegError = false;
static void litePDF_JPegErrorExit(j_common_ptr cinfo)
{
   /*char buffer[JMSG_LENGTH_MAX];
   (*cinfo->err->format_message) (cinfo, buffer);*/
   litePDF_jpegError = true;
}

static void litePDF_JPegErrorOutput(j_common_ptr, int)
{
}
};

// it's always RGB here
static bool setImageDataAsJpeg(PdfImage *pdfImage,
                               DWORD imgWidth,
                               DWORD imgHeight,
                               const BYTE *subBits,
                               DWORD nSubBits)
{
   PODOFO_RAISE_LOGIC_IF(!pdfImage, "setImageDataAsJpeg: pdfImage cannot be NULL");
   PODOFO_RAISE_LOGIC_IF(!subBits, "setImageDataAsJpeg: subBits cannot be NULL");

   litePDF_jpegError = false;

   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   unsigned char *jpegData = NULL;
   unsigned long jpegSize = 0;
   JSAMPROW row[1];

   cinfo.err = jpeg_std_error(&jerr);
   jerr.error_exit = &litePDF_JPegErrorExit;
   jerr.emit_message = &litePDF_JPegErrorOutput;

   jpeg_create_compress(&cinfo);

   jpeg_mem_dest(&cinfo, &jpegData, &jpegSize);

   cinfo.image_width = imgWidth;
   cinfo.image_height = imgHeight;
   cinfo.input_components = 3; // 1 for JCS_GRAYSCALE
   cinfo.in_color_space = JCS_RGB;

   jpeg_set_defaults(&cinfo);
   
   cinfo.num_components = 3;
   cinfo.dct_method = JDCT_FLOAT;

   jpeg_set_quality(&cinfo, 75, TRUE);
   jpeg_start_compress(&cinfo, true);

   while (cinfo.next_scanline < cinfo.image_height && !litePDF_jpegError) {
      DWORD from = cinfo.next_scanline * cinfo.image_width * cinfo.input_components;
      if (from + (cinfo.image_width * cinfo.input_components) > nSubBits) {
         litePDF_jpegError = true;
         break;
      }

      row[0] = (unsigned char *) (subBits + from);
      jpeg_write_scanlines(&cinfo, row, 1);
   }

   jpeg_finish_compress(&cinfo);
   jpeg_destroy_compress(&cinfo);

   if (!litePDF_jpegError && jpegData && jpegSize) {
      PdfMemoryInputStream stream((const char *) jpegData, jpegSize);

      pdfImage->SetImageColorSpace(ePdfColorSpace_DeviceRGB);
      pdfImage->GetObject()->GetDictionary().AddKey(PdfName::KeyFilter, PdfName("DCTDecode"));
      pdfImage->SetImageDataRaw(imgWidth, imgHeight, 8, &stream);
   } else {
      litePDF_jpegError = true;
   }

   if (jpegData) {
      free(jpegData);
   }

   return !litePDF_jpegError;
}

typedef bool (* ForeachPixelCB)(struct processData *pd, DWORD xx, DWORD yy, DWORD imgWidth, DWORD imgHeight, COLORREF pixel, void *user_data);

static bool foreachPixel(struct processData *pd, PdfImage *pdfImage, const RECT *srcRect, DWORD iUsageColor, const BITMAPINFO *bmi, const BYTE *bits, DWORD cbBits, bool isMaskImg, ForeachPixelCB cb, void *user_data)
{
   PODOFO_RAISE_LOGIC_IF( !pd, "foreachPixel: pd cannot be NULL." );
   PODOFO_RAISE_LOGIC_IF( !pdfImage, "foreachPixel: pdfImage cannot be NULL." );
   PODOFO_RAISE_LOGIC_IF( !bmi, "foreachPixel: bmi cannot be NULL." );
   PODOFO_RAISE_LOGIC_IF( !bits, "foreachPixel: bits cannot be NULL." );
   PODOFO_RAISE_LOGIC_IF( !cb, "foreachPixel: cb cannot be NULL." );

   COLORREF pixel;
   const RGBQUAD *palette = (const RGBQUAD *) STRUCT_OFFSET (bmi, bmi->bmiHeader.biSize);
   RGBQUAD pColor;
   DWORD scanlineBytes, xx, yy, inc;
   const BYTE *scanline = bits, *maxBits;
   BYTE mask;

   DWORD imgWidth = srcRect ? srcRect->right - srcRect->left : bmi->bmiHeader.biWidth,
         imgHeight = (srcRect ? srcRect->bottom - srcRect->top : bmi->bmiHeader.biHeight) * (bmi->bmiHeader.biHeight < 0 ? (-1) : (+1));

   xx = 0;
   yy = 0;
   if (bmi->bmiHeader.biHeight < 0) {
      yy = (bmi->bmiHeader.biHeight * (-1)) - 1;
   }

   switch(bmi->bmiHeader.biBitCount) {
   //case 0: JPEG
   case 1:
   case 4:
   case 8:
      scanlineBytes = bmi->bmiHeader.biWidth / (8 / bmi->bmiHeader.biBitCount);
      if ((bmi->bmiHeader.biWidth % 8) != 0) {
         scanlineBytes++;
      }
      if ((scanlineBytes % sizeof(LONG)) != 0) {
         scanlineBytes = scanlineBytes + sizeof(LONG);
         scanlineBytes = scanlineBytes - (scanlineBytes % sizeof(LONG));
      }
      maxBits = bits + (scanlineBytes * bmi->bmiHeader.biHeight * (bmi->bmiHeader.biHeight < 0 ? -1 : +1));
      mask = (1 << bmi->bmiHeader.biBitCount) - 1;

      for (; bits < maxBits; bits++) {
         BYTE b = *bits;
         int offset;

         for (offset = 8 - bmi->bmiHeader.biBitCount; offset >= 0 && xx < bmi->bmiHeader.biWidth; offset -= bmi->bmiHeader.biBitCount, xx++) {
            if (offset != 0) {
               pixel = (b >> offset);
            } else {
               pixel = b;
            }
            if (isMaskImg) {
               if ((pixel & mask) == 0) {
                  pixel = RGB(0,0,0);
               } else {
                  pixel = RGB(0xff,0xff,0xff);
               }
            } else if (iUsageColor == 2) {
               if ((pixel & mask) == 0) {
                  pixel = pd->currentDCState.textColor;
               } else {
                  pixel = pd->currentDCState.bkColor;
               }
            } else {
               pColor = palette[(pixel & mask)];
               pixel = RGB(pColor.rgbRed, pColor.rgbGreen, pColor.rgbBlue);
            }

            if ((!srcRect || (xx >= srcRect->left && yy >= srcRect->top && xx < srcRect->right && yy < srcRect->bottom)) &&
               !cb(pd, xx - (srcRect ? srcRect->left : 0), yy - (srcRect ? srcRect->top : 0), imgWidth, imgHeight, pixel, user_data)) {
               return false;
            }
         }

         if (xx >= bmi->bmiHeader.biWidth) {
            xx = 0;
            if (bmi->bmiHeader.biHeight < 0) {
               yy--;
               if (yy <= bmi->bmiHeader.biHeight) {
                  break;
               }
            } else {
               yy++;
               if (yy >= bmi->bmiHeader.biHeight) {
                  break;
               }
            }

            bits = scanline + scanlineBytes;
            scanline = bits;
            bits -= 1; // because 'bits++' in 'for'
         }
      }
      break;
   case 16:
   case 24:
   case 32:
      scanlineBytes = bmi->bmiHeader.biWidth * bmi->bmiHeader.biBitCount / 8;

      if ((scanlineBytes % sizeof(LONG)) != 0) {
         scanlineBytes = scanlineBytes + sizeof(LONG);
         scanlineBytes = scanlineBytes - (scanlineBytes % sizeof(LONG));
      }
      maxBits = bits + (scanlineBytes * bmi->bmiHeader.biHeight * (bmi->bmiHeader.biHeight < 0 ? -1 : +1));

      if (iUsageColor == DIB_PAL_COLORS) {
         inc = 2;
      } else if (iUsageColor == DIB_RGB_COLORS) {
         inc = bmi->bmiHeader.biBitCount / 8;
      } else {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_SUPPORTED, "foreachPixel: Unsupported image color usage", pd->on_error_user_data);
         }
         return false;
      }

      for (; bits < maxBits; bits += inc) {
         if (!srcRect || (xx >= srcRect->left && yy >= srcRect->top && xx < srcRect->right && yy < srcRect->bottom)) {
            if (iUsageColor == DIB_PAL_COLORS) {
               WORD idx = (bits[0] << 8) | bits[1];
               pColor = palette[idx];
               pixel = RGB(pColor.rgbRed, pColor.rgbGreen, pColor.rgbBlue);
            } else { // DIB_RGB_COLORS
               const DWORD *masks = NULL;
               if (bmi->bmiColors) {
                  masks = (const DWORD *) bmi->bmiColors;
               }

               if (bmi->bmiHeader.biBitCount == 16) {
                  WORD w = (((WORD) bits[1]) << 8) | bits[0], r, g, b;
                  if (bmi->bmiHeader.biCompression == BI_BITFIELDS && masks) {
                     DWORD rmask = masks[0], gmask = masks[1], bmask = masks[2];

                     r = w & rmask;
                     while (r && rmask && !(rmask & 1)) {
                        rmask = rmask >> 1;
                        r = r >> 1;
                     }
                     r = (r / ((double) rmask)) * 255;

                     g = w & gmask;
                     while (g && gmask && !(gmask & 1)) {
                        gmask = gmask >> 1;
                        g = g >> 1;
                     }
                     g = (g / ((double) gmask)) * 255;

                     b = w & bmask;
                     while (b && bmask && !(bmask & 1)) {
                        bmask = bmask >> 1;
                        b = b >> 1;
                     }
                     b = (b / ((double) bmask)) * 255;
                  } else { // hopefully BI_RGB
                     r = (w & (0x1f << 10)) >> 10;
                     g = (w & (0x1f <<  5)) >>  5;
                     b =  w &  0x1f;
                  }
                  pixel = RGB(r, g, b);
               } else if (bmi->bmiHeader.biBitCount == 32 && bmi->bmiHeader.biCompression == BI_BITFIELDS &&
                  masks && (masks[0] != 0xff0000 || masks[1] != 0x00ff00 || masks[2] != 0x0000ff)) {
                  DWORD w = (bits[3] << 24) | (bits[2] << 16) | (bits[1] << 8) | bits[0], r, g, b, a;
                  const DWORD *masks = (const DWORD *) bmi->bmiColors;
                  DWORD rmask = masks[0], gmask = masks[1], bmask = masks[2], amask = ~ (rmask | gmask | bmask);
                  r = w & rmask;
                  while (r && rmask && !(rmask & 1)) {
                     rmask = rmask >> 1;
                     r = r >> 1;
                  }
                  r = (r / ((double) rmask)) * 255;

                  g = w & gmask;
                  while (g && gmask && !(gmask & 1)) {
                     gmask = gmask >> 1;
                     g = g >> 1;
                  }
                  g = (g / ((double) gmask)) * 255;

                  b = w & bmask;
                  while (b && bmask && !(bmask & 1)) {
                     bmask = bmask >> 1;
                     b = b >> 1;
                  }
                  b = (b / ((double) bmask)) * 255;

                  a = w & amask;
                  while (a && amask && !(amask & 1)) {
                     amask = amask >> 1;
                     a = a >> 1;
                  }
                  a = (a / ((double) amask)) * 255;

                  pixel = RGB(r, g, b) | (a ? ((a & 0xFF) << 24) : 0);
               } else {
                  pixel = RGB(bits[2], bits[1], bits[0]);
                  if (bmi->bmiHeader.biBitCount == 32) {
                     pixel = pixel | (bits[3] << 24);
                  }
               }
            }

            if (!cb(pd, xx - (srcRect ? srcRect->left : 0), yy - (srcRect ? srcRect->top : 0), imgWidth, imgHeight, pixel, user_data)) {
               return false;
            }
         }

         xx++;
         if (xx >= bmi->bmiHeader.biWidth) {
            xx = 0;
            if (bmi->bmiHeader.biHeight < 0) {
               yy--;
               if (yy <= bmi->bmiHeader.biHeight) {
                  break;
               }
            } else {
               yy++;
               if (yy >= bmi->bmiHeader.biHeight) {
                  break;
               }
            }

            bits = scanline + scanlineBytes;
            scanline = bits;
            bits -= inc; // befause 'bits += inc' in 'for'
         }
      }
      break;
   default:
      if (pd->on_error) {
         pd->on_error (ERROR_NOT_SUPPORTED, "foreachPixel: Unsupported image format", pd->on_error_user_data);
      }
      return false;
   }

   return true;
}

static bool gatherImageColors(struct processData *pd, DWORD xx, DWORD yy, DWORD imgWidth, DWORD imgHeight, COLORREF pixel, void *user_data)
{
   map<COLORREF, BYTE> *colors = (map<COLORREF, BYTE> *) user_data;
   if (colors) {
      (*colors)[pixel & 0xFFFFFF] = 1;
      return colors->size() <= 255;
   }

   return false;
}

static void
updateAlpha(BYTE **bitsAlpha, DWORD &cbBitsAlpha, DWORD xx, DWORD yy, DWORD imgWidth, DWORD imgHeight, COLORREF pixel)
{
   (*bitsAlpha)[xx + (yy * imgWidth)] = ((pixel >> 24) & 0xFF);

   if (!cbBitsAlpha) {
      *bitsAlpha = NULL;
   } else {
      cbBitsAlpha--;
   }
}

struct FillImgPaletteData {
   map<COLORREF, BYTE> *colorMap;
   BYTE *subPixels;
   unsigned short bitOffset;
   BYTE bitShift;
   BYTE *bitsAlpha;
   DWORD cbBitsAlpha;
};

static bool fillPaletteIndexesArray(struct processData *pd, DWORD xx, DWORD yy, DWORD imgWidth, DWORD imgHeight, COLORREF pixel, void *user_data)
{
   struct FillImgPaletteData *data = (struct FillImgPaletteData *) user_data;
   BYTE index = (*data->colorMap)[pixel & 0xFFFFFF];

   if (data->bitOffset != 0) {
      *(data->subPixels) = *(data->subPixels) << data->bitShift;
   }

   *(data->subPixels) |= index;

   data->bitOffset += data->bitShift;
   if (data->bitOffset >= 8) {
      data->subPixels++;
      *(data->subPixels) = 0;
      data->bitOffset = 0;
   }

   if (xx == imgWidth - 1) {
      // the last pixel on this row, make sure the padding is done properly
      if (data->bitOffset != 0) {
         while (data->bitOffset < 8) {
            *(data->subPixels) = *(data->subPixels) << data->bitShift;
            data->bitOffset += data->bitShift;
         }

         data->subPixels++;
         *(data->subPixels) = 0;
         data->bitOffset = 0;
      }
   }

   if (data->bitsAlpha) {
      updateAlpha(&(data->bitsAlpha), data->cbBitsAlpha, xx, yy, imgWidth, imgHeight, pixel);
   }

   return true;
}

struct FillRGBData
{
   BYTE *subPixels;
   bool invertColors;
   BYTE *bitsAlpha;
   DWORD cbBitsAlpha;
};

static bool fillRGBColorArray(struct processData *pd, DWORD xx, DWORD yy, DWORD imgWidth, DWORD imgHeight, COLORREF pixel, void *user_data)
{
   struct FillRGBData *data = (struct FillRGBData *) user_data;

   data->subPixels[0] = GetRValue(pixel);
   data->subPixels[1] = GetGValue(pixel);
   data->subPixels[2] = GetBValue(pixel);

   if (data->invertColors) {
      data->subPixels[0] = ~data->subPixels[0];
      data->subPixels[1] = ~data->subPixels[1];
      data->subPixels[2] = ~data->subPixels[2];
   }

   data->subPixels += 3;

   if (data->bitsAlpha) {
      updateAlpha(&(data->bitsAlpha), data->cbBitsAlpha, xx, yy, imgWidth, imgHeight, pixel);
   }

   return true;
}

static bool fillPdfImageFromBitmap(struct processData *pd,
                                   PdfImage *pdfImage,
                                   const RECT *srcRect,
                                   DWORD dwRop,
                                   DWORD iUsageColor,
                                   const BITMAPINFO *bmi,
                                   const BYTE *bits,
                                   DWORD cbBits,
                                   bool disableJpegCompression,
                                   bool isMaskImg = false,
                                   BYTE **bitsAlpha = NULL,
                                   DWORD *cbBitsAlpha = NULL)
{
   BYTE *subBits = NULL;
   const BYTE *pBits = bits;
   DWORD nBitsPerComponent;

   if (!pd) {
      return false;
   }

   if (!pdfImage || !bmi || !bits || !cbBits) {
      if (pd->on_error) {
         pd->on_error (ERROR_INVALID_PARAMETER, "fillPdfImageFromBitmap: parameters cannot be NULL, nor zero cbBits", pd->on_error_user_data);
      }
      return false;
   }

   #define MAX(a,b) ((a) > (b) ? (a) : (b))
   nBitsPerComponent = MAX(bmi->bmiHeader.biBitCount / 3, 1);
   #undef MAX

   bool invertColors = dwRop == NOTSRCCOPY || dwRop == SRCINVERT;
   bool doJpegCompression = !disableJpegCompression && !isMaskImg && !bitsAlpha && !cbBitsAlpha && (pd->drawFlags & LITEPDF_DRAW_FLAG_COMPRESS_IMAGES_WITH_JPEG) != 0;
   map<COLORREF, BYTE> colorMap;

   // ignore false, because it stops when there are more than 255 colors 
   if (!foreachPixel(pd, pdfImage, srcRect, iUsageColor, bmi, bits, cbBits, isMaskImg, gatherImageColors, &colorMap)) {
      colorMap.clear();
   }

   DWORD imgWidth = srcRect ? srcRect->right - srcRect->left : bmi->bmiHeader.biWidth,
         imgHeight = (srcRect ? srcRect->bottom - srcRect->top : (bmi->bmiHeader.biHeight * (bmi->bmiHeader.biHeight < 0 ? (-1) : (+1))));
   DWORD pixels = 3 * imgWidth * imgHeight;
   if (!colorMap.empty() && colorMap.size() <= 255) {
      // valid BitsPerComponent is only 1,2,4,8 and since PDF1.5 also 16
      pixels = colorMap.size();
      if (pixels <= (1 << 1)) {
         nBitsPerComponent = 1;
      } else if (pixels <= (1 << 2)) {
         nBitsPerComponent = 2;
      } else if (pixels <= (1 << 4)) {
         nBitsPerComponent = 4;
      } else {
         nBitsPerComponent = 8;
      }

      // do not do Jpeg compression on indexed images
      doJpegCompression = false;

      DWORD width = bmi->bmiHeader.biWidth;
      if ((width % (8 / nBitsPerComponent)) != 0) {
         width += 8;
      }
      width = width / (8 / nBitsPerComponent);
      pixels = width * imgHeight;

      // always writes one byte ahead - resets it to zero, thus "pixels + 1"
      subBits = (BYTE *) malloc(sizeof(BYTE) * (pixels + 1));
      if (!subBits) {
         if (pd->on_error) {
            pd->on_error (ERROR_OUTOFMEMORY, "fillPdfImageFromBitmap: Out of memory", pd->on_error_user_data);
         }
         return false;
      }

      DWORD ii;
      PdfArray array;
      char info[10];
      char *colors = (char *) malloc (sizeof(char) * (1 + 2 + 3 + colorMap.size() * 7)), *clr;
      if (!colors) {
         free (subBits);
         if (pd->on_error) {
            pd->on_error (ERROR_OUTOFMEMORY, "fillPdfImageFromBitmap: Out of memory", pd->on_error_user_data);
         }
         return false;
      }

      *colors = 0;
      strcat(colors, " <");
      clr = colors + 1 + 1;

      bool isGray = true;
      map<COLORREF, BYTE>::iterator it, end = colorMap.end();
      for (it = colorMap.begin(); isGray && it != end; it++) {
         isGray = GetRValue(it->first) == GetGValue(it->first) && GetRValue(it->first) == GetBValue(it->first);
      }

      array.push_back(PdfColor::GetNameForColorSpace(isGray ? ePdfColorSpace_DeviceGray : ePdfColorSpace_DeviceRGB));
      array.push_back(PdfVariant(static_cast<pdf_int64>(colorMap.size() - 1)));

      BYTE r,g,b;
      for (it = colorMap.begin(), ii = 0; it != end; it++, ii++) {
         if (isMaskImg) {
            it->second = it->first == RGB(0,0,0) ? 1 : 0;
         } else {
            it->second = ii;
         }

         if (isGray) {
            g = GetGValue(it->first);
            if (invertColors) {
               g = ~g;
            }

            sprintf(info, "%02x", g & 0xFF);

            if (clr != colors + 1 + 1) {
               strcat(clr, " ");
               clr++;
            }

            strcat(clr, info);
            clr += 2;
         } else {
            r = GetRValue(it->first);
            g = GetGValue(it->first);
            b = GetBValue(it->first);

            if (invertColors) {
               r = ~r;
               g = ~g;
               b = ~b;
            }

            sprintf(info, "%02x%02x%02x", r & 0xFF, g & 0xFF, b & 0xFF);

            if (clr != colors + 1 + 1) {
               strcat(clr, " ");
               clr++;
            }

            strcat(clr, info);
            clr += 6;
         }
      }

      strcat(clr, ">");

      array.push_back(PdfVariant(PdfData(colors)));

      free (colors);

      pdfImage->SetImageColorSpace(ePdfColorSpace_Indexed, &array);

      struct FillImgPaletteData data;

      *subBits = 0;

      data.colorMap = &colorMap;
      data.subPixels = subBits;
      data.bitOffset = 0;
      data.bitShift = nBitsPerComponent;
      data.bitsAlpha = NULL;
      data.cbBitsAlpha = 0;

      if (bitsAlpha && cbBitsAlpha) {
         *cbBitsAlpha = imgWidth * imgHeight;
         *bitsAlpha = (BYTE *) malloc(sizeof(BYTE) * (*cbBitsAlpha));

         if (!*bitsAlpha) {
            if (pd->on_error) {
               pd->on_error (ERROR_OUTOFMEMORY, "fillPdfImageFromBitmap: Out of memory for alpha bits", pd->on_error_user_data);
            }
            free (subBits);
            return false;
         }

         data.bitsAlpha = *bitsAlpha;
         data.cbBitsAlpha = *cbBitsAlpha;
      }

      if (!foreachPixel(pd, pdfImage, srcRect, iUsageColor, bmi, bits, cbBits, isMaskImg, fillPaletteIndexesArray, &data)) {
         return false;
      }
   } else {
      subBits = (BYTE *) malloc(sizeof(BYTE) * pixels);
      if (!subBits) {
         if (pd->on_error) {
            pd->on_error (ERROR_OUTOFMEMORY, "fillPdfImageFromBitmap: Out of memory", pd->on_error_user_data);
         }
         return false;
      }

      struct FillRGBData frgb;
      frgb.subPixels = subBits;
      frgb.invertColors = invertColors;
      frgb.bitsAlpha = NULL;
      frgb.cbBitsAlpha = 0;

      if (bitsAlpha && cbBitsAlpha) {
         *cbBitsAlpha = imgWidth * imgHeight;
         *bitsAlpha = (BYTE *) malloc(sizeof(BYTE) * (*cbBitsAlpha));

         if (!*bitsAlpha) {
            if (pd->on_error) {
               pd->on_error (ERROR_OUTOFMEMORY, "fillPdfImageFromBitmap: Out of memory for alpha bits", pd->on_error_user_data);
            }
            free (subBits);
            return false;
         }

         frgb.bitsAlpha = *bitsAlpha;
         frgb.cbBitsAlpha = *cbBitsAlpha;
      }

      // windows stores RGB as BGR, thus swap the two
      if (!foreachPixel(pd, pdfImage, srcRect, iUsageColor, bmi, bits, cbBits, isMaskImg, fillRGBColorArray, &frgb)) {
         return false;
      }

      nBitsPerComponent = 8;
   }

   // if the Jpeg compression fails, store image as is
   if (!doJpegCompression || !setImageDataAsJpeg(pdfImage, imgWidth, imgHeight, subBits, pixels)) {
      PdfMemoryInputStream stream((const char *) subBits, pixels);

      pdfImage->SetImageData(imgWidth, imgHeight, nBitsPerComponent, &stream);
   }

   if (subBits){
      free (subBits);
   }

   return true;
}

static bool selectPen(struct processData *pd, bool invert = false)
{
   if (!pd || !pd->currentDCState.penSet) {
      return false;
   }

   const LOGPEN &lp = pd->currentDCState.setPen;
   UINT iStyle = lp.lopnStyle;
   int penW = lp.lopnWidth.x, penH = lp.lopnWidth.y;

   if (pd->currentDCState.worldMatrixValid) {
      double pw = penW, ph = penH;

      penW = pw * pd->currentDCState.worldMatrix.eM11 + ph * pd->currentDCState.worldMatrix.eM21;
      penH = pw * pd->currentDCState.worldMatrix.eM12 + ph * pd->currentDCState.worldMatrix.eM22;
   }

   if (iStyle != PS_SOLID && iStyle != PS_NULL && (penW > 1 || penW < -1 || penH > 0 || penH <= -1)) {
      // compatibility with windows, other than solid pen is drawn only with width 1
      iStyle = PS_SOLID;
   }

   switch(iStyle) {
   case PS_INSIDEFRAME:
   case PS_SOLID:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Solid);
      break;
   case PS_DASH:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Dash, NULL, invert);
      break;
   case PS_DOT:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Dot, NULL, invert);
      break;
   case PS_DASHDOT:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_DashDot, NULL, invert);
      break;
   case PS_DASHDOTDOT:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_DashDotDot, NULL, invert);
      break;
   case PS_NULL:
      pd->currentDCState.penSet = false;
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Solid);
      break;
   default:
      if (pd->on_error) {
         char buff[128];
         sprintf(buff, "Unknown Pen style 0x%x", iStyle);
         pd->on_error(ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
      pd->currentDCState.brushSet = false;
      return false;
   }

   double pw, ph;
   LogicalUnitToPdf(pd, lp.lopnWidth.x, lp.lopnWidth.y, pw, ph, false);

   if (ph > pw) {
      pw = ph;
   }

   pd->painter->SetStrokeWidth(lp.lopnStyle != PS_NULL ? pw : 0.0);
   pd->painter->SetStrokingColor(GetRValue(lp.lopnColor)/255.0, GetGValue(lp.lopnColor)/255.0, GetBValue(lp.lopnColor)/255.0);

   return true;
}

static bool selectBrush(struct processData *pd, bool forceFill = false, bool fromExtPen = false);

static bool selectExtPen(struct processData *pd)
{
   if (!pd || !pd->currentDCState.extPenSet || !pd->currentDCState.setExtPen) {
      return false;
   }

   const EXTLOGPEN32 *extPen = pd->currentDCState.setExtPen;

   bool cosmetic = (extPen->elpPenStyle & PS_TYPE_MASK) == PS_COSMETIC;
   bool needsShorterLines = !cosmetic && ((extPen->elpPenStyle & PS_ENDCAP_MASK) == PS_ENDCAP_ROUND || (extPen->elpPenStyle & PS_ENDCAP_MASK) == PS_ENDCAP_SQUARE);
   double pw, ph, penW;

   LogicalUnitToPdf(pd, extPen->elpWidth, extPen->elpWidth, pw, ph, false);

   if (ph > pw) {
      pw = ph;
   }

   penW = pw;

   switch (extPen->elpPenStyle & PS_STYLE_MASK) {
   case PS_INSIDEFRAME:
   case PS_SOLID:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Solid);
      break;
   case PS_DASH:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Dash, NULL, false, cosmetic ? 1.0 : penW, needsShorterLines);
      break;
   case PS_DOT:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Dot, NULL, false, cosmetic ? 1.0 : penW, needsShorterLines);
      break;
   case PS_DASHDOT:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_DashDot, NULL, false, cosmetic ? 1.0 : penW, needsShorterLines);
      break;
   case PS_DASHDOTDOT:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_DashDotDot, NULL, false, cosmetic ? 1.0 : penW, needsShorterLines);
      break;
   case PS_NULL:
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Solid);
      return false;
      break;
   case PS_USERSTYLE: {
      DWORD ii;
      char buff[128];
      std::string custom = "";

      for (ii = 0; ii < extPen->elpNumEntries; ii++) {
         LogicalUnitToPdf(pd, extPen->elpStyleEntry[ii], extPen->elpStyleEntry[ii], pw, ph, false);

         if (ph > pw) {
            pw = ph;
         }

         sprintf(buff, "%f", pw);
         if (!custom.empty()) {
            custom += " ";
         }
         custom += buff;
      }

      custom = "[" + custom + "] 0";
      pd->painter->SetStrokeStyle(ePdfStrokeStyle_Custom, custom.c_str());
      } break;
   default:
      if (pd->on_error) {
         char buff[128];
         sprintf(buff, "Unknown ExtPen style 0x%x", extPen->elpPenStyle & PS_STYLE_MASK);
         pd->on_error(ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
      return false;
   }

   if (!cosmetic) {
      switch (extPen->elpPenStyle & PS_ENDCAP_MASK) {
      case PS_ENDCAP_ROUND:
         pd->painter->SetLineCapStyle (ePdfLineCapStyle_Round);
         break;
      case PS_ENDCAP_SQUARE:
         pd->painter->SetLineCapStyle (ePdfLineCapStyle_Square);
         break;
      case PS_ENDCAP_FLAT:
         pd->painter->SetLineCapStyle (ePdfLineCapStyle_Butt);
         break;
      }

      switch (extPen->elpPenStyle & PS_JOIN_MASK) {
      case PS_JOIN_BEVEL:
         pd->painter->SetLineJoinStyle(ePdfLineJoinStyle_Bevel);
         break;
      case PS_JOIN_MITER:
         pd->painter->SetLineJoinStyle(ePdfLineJoinStyle_Miter);
         break;
      case PS_JOIN_ROUND:
         pd->painter->SetLineJoinStyle(ePdfLineJoinStyle_Round);
         break;
      }
   }

   pd->painter->SetStrokeWidth(penW);

   if (extPen->elpBrushStyle == BS_SOLID) {
      COLORREF color;

      color = extPen->elpColor;
      pd->painter->SetStrokingColor(GetRValue(color)/255.0, GetGValue(color)/255.0, GetBValue(color)/255.0);
   } else {
      selectBrush(pd, false, true);
   }

   return true;
}

static bool selectBrush(struct processData *pd, bool forceFill, bool fromExtPen)
{
   if (!pd || (!pd->currentDCState.brushSet && !fromExtPen)) {
      return false;
   }

   std::string brushResourceName = "";
   LOGBRUSH32 extPenBrush;
   const LOGBRUSH32 *lb = pd->currentDCState.setBrush;
   if (fromExtPen) {
      extPenBrush.lbStyle = pd->currentDCState.setExtPen->elpBrushStyle;
      extPenBrush.lbHatch = pd->currentDCState.setExtPen->elpHatch;
      extPenBrush.lbColor = pd->currentDCState.setExtPen->elpColor;
      lb = &extPenBrush;
   }

   UINT lbStyle = -1;
   const ENHMETARECORD *lpEMFR = NULL;
   COLORREF bkColor = pd->currentDCState.bkColor;
   if ((bkColor & 0xFF000000) != 0) {
      bkColor = RGB(0, 0, 0);
   }

   if (lb) {
      lbStyle = lb->lbStyle;
   } else {
      if (pd->objects.find(pd->currentDCState.setBrushObjectIndex) == pd->objects.end()) {
         return false;
      }

      lpEMFR = (const ENHMETARECORD *) (pd->objects[pd->currentDCState.setBrushObjectIndex]->bytes);
      if (lpEMFR) {
         switch(lpEMFR->iType) {
         case EMR_CREATEMONOBRUSH:
            lbStyle = BS_PATTERN;
            break;
         case EMR_CREATEDIBPATTERNBRUSHPT:
            lbStyle = BS_DIBPATTERNPT;
            break;
         }
      }
   }

   switch (lbStyle) {
   //case BS_DIBPATTERN:
   case BS_DIBPATTERNPT:
      if (!lpEMFR) {
         if (pd->on_error) {
            pd->on_error(ERROR_INVALID_PARAMETER, "Faild to apply brush BS_DIBPATTERNPT", pd->on_error_user_data);
         }
         if (!fromExtPen) {
            pd->currentDCState.brushSet = false;
         }
         return false;
      } else if (fromExtPen) {
         if (pd->currentDCState.setExtPenBrushName) {
            brushResourceName = *pd->currentDCState.setExtPenBrushName;
         }
      } else {
         if (pd->currentDCState.setBrushName) {
            brushResourceName = *pd->currentDCState.setBrushName;
         }
      }
      if (brushResourceName.empty()) {
         const EMRCREATEDIBPATTERNBRUSHPT *dt = (const EMRCREATEDIBPATTERNBRUSHPT *) lpEMFR;

         PdfImage image(pd->document);
         if (fillPdfImageFromBitmap(pd, &image, NULL, SRCCOPY, dt->iUsage, (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmi), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBits), dt->cbBits, true)) {
            PdfTilingPattern ptrn(
               ePdfTilingPatternType_Image,
               0.0, 0.0, 0.0,
               forceFill || (pd->currentDCState.iBkMode == OPAQUE && pd->currentDCState.bkColorSet),
               GetRValue(bkColor)/255.0, GetGValue(bkColor)/255.0, GetBValue(bkColor)/255.0,
               pd->currentDCState.brushOrg.x, pd->currentDCState.brushOrg.y,
               &image, pd->document);
            if (fromExtPen) {
               pd->painter->SetStrokingTilingPattern(ptrn);
            } else {
               pd->painter->SetTilingPattern(ptrn);
            }
            brushResourceName = ptrn.GetIdentifier().GetName();
         } else {
            if (pd->on_error) {
               pd->on_error(ERROR_NOT_SUPPORTED, "Failed to convert DIBPATTERNBRUSH image", pd->on_error_user_data);
            }
            if (!fromExtPen) {
               pd->currentDCState.brushSet = false;
            }
            return false;
         }
      } else {
         if (fromExtPen) {
            pd->painter->SetStrokingTilingPattern(brushResourceName);
         } else {
            pd->painter->SetTilingPattern(brushResourceName);
         }
         brushResourceName = ""; // to not re-set at the bottom
      }
      break;

   case BS_HATCHED:
      switch (lb->lbHatch) {
      case HS_BDIAGONAL:
      case HS_CROSS:
      case HS_DIAGCROSS:
      case HS_FDIAGONAL:
      case HS_HORIZONTAL:
      case HS_VERTICAL: {
         if (fromExtPen) {
            if (pd->currentDCState.setExtPenBrushName) {
               brushResourceName = *pd->currentDCState.setExtPenBrushName;
            }
         } else {
            if (pd->currentDCState.setBrushName) {
               brushResourceName = *pd->currentDCState.setBrushName;
            }
         }
         if (brushResourceName.empty()) {
            PdfTilingPattern ptrn(
               lb->lbHatch == HS_BDIAGONAL ? ePdfTilingPatternType_BDiagonal :
               lb->lbHatch == HS_CROSS ? ePdfTilingPatternType_Cross:
               lb->lbHatch == HS_DIAGCROSS ? ePdfTilingPatternType_DiagCross :
               lb->lbHatch == HS_FDIAGONAL ? ePdfTilingPatternType_FDiagonal :
               lb->lbHatch == HS_HORIZONTAL ? ePdfTilingPatternType_Horizontal :
               /* lb->lbHatch == HS_VERTICAL ? */ ePdfTilingPatternType_Vertical,
               GetRValue(lb->lbColor)/255.0, GetGValue(lb->lbColor)/255.0, GetBValue(lb->lbColor)/255.0,
               forceFill || (pd->currentDCState.iBkMode == OPAQUE && pd->currentDCState.bkColorSet),
               GetRValue(bkColor)/255.0, GetGValue(bkColor)/255.0, GetBValue(bkColor)/255.0,
               pd->currentDCState.brushOrg.x, pd->currentDCState.brushOrg.y,
               NULL, pd->document);
            if (fromExtPen) {
               pd->painter->SetStrokingTilingPattern(ptrn);
            } else {
               pd->painter->SetTilingPattern(ptrn);
            }
            brushResourceName = ptrn.GetIdentifier().GetName();
         } else {
            if (fromExtPen) {
               pd->painter->SetStrokingTilingPattern(brushResourceName);
            } else {
               pd->painter->SetTilingPattern(brushResourceName);
            }
            brushResourceName = ""; // to not re-set at the bottom
         }
         } break;
      default:
         if (pd->on_error) {
            char buff[128];
            sprintf(buff, "Faild to apply brush hatch 0x%x", lb->lbHatch);
            pd->on_error(ERROR_INVALID_PARAMETER, buff, pd->on_error_user_data);
         }
         if (!fromExtPen) {
            pd->currentDCState.brushSet = false;
         }
         return false;
      }
      break;
   case BS_HOLLOW: //BS_NULL
      if (!fromExtPen) {
         pd->currentDCState.brushSet = false;
      }
      return false;
   case BS_PATTERN: //BS_PATTERN8X8
      if (!lpEMFR) {
         if (pd->on_error) {
            pd->on_error(ERROR_INVALID_PARAMETER, "Faild to apply brush BS_DIBPATTERNPT", pd->on_error_user_data);
         }
         if (!fromExtPen) {
            pd->currentDCState.brushSet = false;
         }
         return false;
      } else if (fromExtPen) {
         if (pd->currentDCState.setExtPenBrushName) {
            brushResourceName = *pd->currentDCState.setExtPenBrushName;
         }
      } else {
         if (pd->currentDCState.setBrushName) {
            brushResourceName = *pd->currentDCState.setBrushName;
         }
      }
      if (brushResourceName.empty()) {
         const EMRCREATEMONOBRUSH *dt = (const EMRCREATEMONOBRUSH *) lpEMFR;

         PdfImage image(pd->document);
         if (fillPdfImageFromBitmap(pd, &image, NULL, SRCCOPY, dt->iUsage, (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmi), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBits), dt->cbBits, true)) {
            PdfTilingPattern ptrn(
               ePdfTilingPatternType_Image,
               0.0, 0.0, 0.0,
               forceFill || (pd->currentDCState.iBkMode == OPAQUE && pd->currentDCState.bkColorSet),
               GetRValue(bkColor)/255.0, GetGValue(bkColor)/255.0, GetBValue(bkColor)/255.0,
               pd->currentDCState.brushOrg.x, pd->currentDCState.brushOrg.y,
               &image, pd->document);
            if (fromExtPen) {
               pd->painter->SetStrokingTilingPattern(ptrn);
            } else {
               pd->painter->SetTilingPattern(ptrn);
            }
            brushResourceName = ptrn.GetIdentifier().GetName();
         } else {
            if (pd->on_error) {
               pd->on_error(ERROR_NOT_SUPPORTED, "Failed to convert MONOBRUSH image", pd->on_error_user_data);
            }
            if (!fromExtPen) {
               pd->currentDCState.brushSet = false;
            }
            return false;
         }
      } else {
         if (fromExtPen) {
            pd->painter->SetStrokingTilingPattern(brushResourceName);
         } else {
            pd->painter->SetTilingPattern(brushResourceName);
         }
         brushResourceName = ""; // to not re-set at the bottom
      }
      break;
   case BS_SOLID:
      pd->painter->SetColor(GetRValue(lb->lbColor)/255.0, GetGValue(lb->lbColor)/255.0, GetBValue(lb->lbColor)/255.0);
      break;
   default:
      if (pd->on_error) {
         char buff[128];
         sprintf(buff, "Unknown Brush style 0x%x", lb ? lb->lbStyle : -1);
         pd->on_error(ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
      if (!fromExtPen) {
         pd->currentDCState.brushSet = false;
      }
      return false;
   }

   if (!brushResourceName.empty()) {
      if (fromExtPen) {
         if (pd->currentDCState.setExtPenBrushName) {
            *pd->currentDCState.setExtPenBrushName = brushResourceName;
         }
      } else {
         if (pd->currentDCState.setBrushName) {
            *pd->currentDCState.setBrushName = brushResourceName;
         }
      }
   }

   return true;
}

static const PdfEncoding *getNativeEncoding (struct processData *pd, BYTE lfCharSet)
{
   if (!pd || !pd->encodingsCache) {
      return PdfEncodingFactory::GlobalWinAnsiEncodingInstance();
   }

   const PdfEncoding *nativeEncoding = pd->encodingsCache->getEncodingForCharSet(lfCharSet);

   if (!nativeEncoding) {
      nativeEncoding = PdfEncodingFactory::GlobalWinAnsiEncodingInstance();
      pd->encodingsCache->put_encodingReplacement(lfCharSet, nativeEncoding);

      if (pd->on_error) {
         const char *charsetIdent = NULL;

         switch (lfCharSet) {
         case ANSI_CHARSET: charsetIdent = "ANSI_CHARSET"; break;
         case BALTIC_CHARSET: charsetIdent = "BALTIC_CHARSET"; break;
         case CHINESEBIG5_CHARSET: charsetIdent = "CHINESEBIG5_CHARSET"; break;
         case DEFAULT_CHARSET: charsetIdent = "DEFAULT_CHARSET"; break;
         case EASTEUROPE_CHARSET: charsetIdent = "EASTEUROPE_CHARSET"; break;
         case GB2312_CHARSET: charsetIdent = "GB2312_CHARSET"; break;
         case GREEK_CHARSET: charsetIdent = "GREEK_CHARSET"; break;
         case HANGUL_CHARSET: charsetIdent = "HANGUL_CHARSET"; break;
         case MAC_CHARSET: charsetIdent = "MAC_CHARSET"; break;
         case OEM_CHARSET: charsetIdent = "OEM_CHARSET"; break;
         case RUSSIAN_CHARSET: charsetIdent = "RUSSIAN_CHARSET"; break;
         case SHIFTJIS_CHARSET: charsetIdent = "SHIFTJIS_CHARSET"; break;
         case SYMBOL_CHARSET: charsetIdent = "SYMBOL_CHARSET"; break;
         case TURKISH_CHARSET: charsetIdent = "TURKISH_CHARSET"; break;
         case JOHAB_CHARSET: charsetIdent = "JOHAB_CHARSET"; break;
         case HEBREW_CHARSET: charsetIdent = "HEBREW_CHARSET"; break;
         case ARABIC_CHARSET: charsetIdent = "ARABIC_CHARSET"; break;
         case THAI_CHARSET: charsetIdent = "THAI_CHARSET"; break;
         }

         char buff[256];
         sprintf(buff, "Unsupported CharSet 0x%x (%s), using WinAnsiEncoding instead", lfCharSet, charsetIdent ? charsetIdent : "???");
         pd->on_error(ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
   }

   return nativeEncoding;
}

static bool isTrueTypeFontName(const char *truetypeName, ...)
{
   va_list ap;
   const char *name;

   va_start(ap, truetypeName);
   while (name = va_arg(ap, const char *), name) {
      if (stricmp(truetypeName, name) == 0) {
         return true;
      }

      int len = strlen(name);
      if (strnicmp(truetypeName, name, len) == 0 &&
          truetypeName[len] == ' ' && strlen(truetypeName + len) == 3) {
         return true;
      }
   }

   return false;
}

static const char *getSubstituteFontName(const char *truetypeName, bool bBold, bool bItalic, bool bIsSymbol)
{
   if (bIsSymbol) {
      return NULL;
   }

   if (isTrueTypeFontName(truetypeName,
         "Arial",
         PODOFO_HPDF_FONT_HELVETICA,
         NULL)) {
      if (bBold && bItalic) {
         return PODOFO_HPDF_FONT_HELVETICA_BOLD_OBLIQUE;
      }

      if (bBold) {
         return PODOFO_HPDF_FONT_HELVETICA_BOLD;
      }

      if (bItalic) {
         return PODOFO_HPDF_FONT_HELVETICA_OBLIQUE;
      }

      return PODOFO_HPDF_FONT_HELVETICA;
   } else if (isTrueTypeFontName(truetypeName,
         "Times New Roman",
         PODOFO_HPDF_FONT_TIMES_ROMAN,
         NULL)) {
      if (bBold && bItalic) {
         return PODOFO_HPDF_FONT_TIMES_BOLD_ITALIC;
      }

      if (bBold) {
         return PODOFO_HPDF_FONT_TIMES_BOLD;
      }

      if (bItalic) {
         return PODOFO_HPDF_FONT_TIMES_ITALIC;
      }

      return PODOFO_HPDF_FONT_TIMES_ROMAN;
   } else if (isTrueTypeFontName(truetypeName,
         "Courier",
         "Courier New",
         PODOFO_HPDF_FONT_COURIER,
         NULL)) {
      if (bBold && bItalic) {
         return PODOFO_HPDF_FONT_COURIER_BOLD_OBLIQUE;
      }

      if (bBold) {
         return PODOFO_HPDF_FONT_COURIER_BOLD;
      }

      if (bItalic) {
         return PODOFO_HPDF_FONT_COURIER_OBLIQUE;
      }

      return PODOFO_HPDF_FONT_COURIER;
   }

   return NULL;
}

static bool selectFont(struct processData *pd)
{
   if (!pd || !pd->currentDCState.fontSet) {
      return false;
   }

   const LOGFONT &lf = pd->currentDCState.setFont;
   PdfFont *font = NULL;
   bool canEmbedFont = true;
   double fontAscent = -1.0, fontDescent = -1;

   HDC hDC = CreateCompatibleDC(NULL);
   if (hDC) {
      HGDIOBJ myFont = CreateFontIndirect(&lf);
      HGDIOBJ oldFont = SelectObject(hDC, myFont);
      OUTLINETEXTMETRIC *otm;
      UINT reqsz;

      reqsz = GetOutlineTextMetrics (hDC, 0, NULL);
      if (reqsz > 0) {
         otm = (OUTLINETEXTMETRIC *) malloc (reqsz);
         if (otm) {
            if (GetOutlineTextMetrics (hDC, reqsz, otm)) {
               canEmbedFont = (otm->otmfsType & 1) == 0;
               fontAscent = otm->otmTextMetrics.tmAscent;
               fontDescent = otm->otmTextMetrics.tmDescent;
            }
            free(otm);
         }
      }

      if (fontAscent < 0) {
         TEXTMETRIC tm;

         if (GetTextMetrics(hDC, &tm)) {
            fontAscent = tm.tmAscent;
            fontDescent = tm.tmDescent;
         }
      }

      SelectObject(hDC, oldFont);
      DeleteObject(myFont);
      DeleteDC(hDC);
   }
 
   const PdfEncoding *nativeEncoding = NULL;

   if ((pd->drawFlags & LITEPDF_DRAW_FLAG_SUBSTITUTE_FONTS) != 0) {
      const char *substName = getSubstituteFontName (lf.lfFaceName, lf.lfWeight > 500, lf.lfItalic != FALSE, lf.lfCharSet == SYMBOL_CHARSET);
      if (substName) {
         if (!nativeEncoding) {
            nativeEncoding = getNativeEncoding (pd, lf.lfCharSet);
         }
         font = pd->document->CreateFont(substName, lf.lfWeight > 500, lf.lfItalic != FALSE, lf.lfCharSet == SYMBOL_CHARSET, nativeEncoding);
         pd->encodingsCache->populateBase14FontWidths(font);
      }
   }

   if (!font && canEmbedFont && (pd->drawFlags & LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET) != 0) {
      // subset fonts are always embedded
      font = pd->document->CreateFontSubset(lf.lfFaceName, lf.lfWeight > 500, lf.lfItalic != FALSE, lf.lfCharSet == SYMBOL_CHARSET, PdfEncodingFactory::GlobalIdentityEncodingInstance());
   }

   if (!font) {
      LOGFONT local_lf = lf;

      if (local_lf.lfWeight > 500) {
         local_lf.lfWeight = FW_BOLD;
      }

      font = pd->document->CreateFont(local_lf, PdfEncodingFactory::GlobalIdentityEncodingInstance(), (pd->drawFlags & LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE) != 0 && canEmbedFont);
      if (!font) {
         if (pd->on_error) {
            char buff[256];
            sprintf(buff, "Faild to create font '%s', bold:%s italic:%s; fallbacking to Helvetica", lf.lfFaceName, lf.lfWeight > 500 ? "yes" : "no", lf.lfItalic != FALSE ? "yes" : "no");
            pd->on_error(ERROR_INVALID_PARAMETER, buff, pd->on_error_user_data);
         }

         if (!nativeEncoding) {
            nativeEncoding = getNativeEncoding (pd, lf.lfCharSet);
         }

         // fallback, when failed to get the font
         const char *substName = getSubstituteFontName ("Arial", lf.lfWeight > 500, lf.lfItalic != FALSE, false /*lf.lfCharSet == SYMBOL_CHARSET*/);
         font = pd->document->CreateFont(substName, lf.lfWeight > 500, lf.lfItalic != FALSE, false /*lf.lfCharSet == SYMBOL_CHARSET*/, nativeEncoding, PdfFontCache::eFontCreationFlags_AutoSelectBase14);
         pd->encodingsCache->populateBase14FontWidths(font);
      }
   }

   if (!font) {
      if (pd->on_error) {
         char buff[128];
         sprintf(buff, "Faild to create font '%s', bold:%s italic:%s", lf.lfFaceName, lf.lfWeight > 500 ? "yes" : "no", lf.lfItalic != FALSE ? "yes" : "no");
         pd->on_error(ERROR_INVALID_PARAMETER, buff, pd->on_error_user_data);
      }
      return false;
   }

   double fw, fh;
   LogicalUnitToPdf(pd, lf.lfWidth, (lf.lfHeight < 0 || fontAscent <= 0) ? lf.lfHeight : fontAscent, fw, fh, false);

   font->SetFontSize(fabs(fh));
   font->SetFontCharSpace(fw);
   font->SetStrikeOut(lf.lfStrikeOut != FALSE);
   font->SetUnderlined(lf.lfUnderline != FALSE);

   pd->painter->SetFont(font);

   if (fontAscent > 0) {
      LogicalUnitToPdf(pd, lf.lfWidth, fontAscent, fw, pd->currentDCState.fontAscent, false);
      pd->currentDCState.fontAscent = fabs(pd->currentDCState.fontAscent);

      LogicalUnitToPdf(pd, lf.lfWidth, fontDescent, fw, pd->currentDCState.fontDescent, false);
      pd->currentDCState.fontDescent = fabs(pd->currentDCState.fontDescent);
   } else {
      pd->currentDCState.fontAscent = fabs(font->GetFontMetrics()->GetAscent());
      pd->currentDCState.fontDescent = fabs(font->GetFontMetrics()->GetDescent());
   }

   pd->currentDCState.fontAngle = lf.lfEscapement / 10;

   return true;
}

static PdfImage *prepareImage(struct processData *pd,
                              int xSrc,
                              int ySrc,
                              int cxSrc,
                              int cySrc,
                              DWORD dwRop,
                              DWORD iUsageColor,
                              const BITMAPINFO *bmi,
                              const BYTE *bits,
                              DWORD cbBits,
                              bool disableJpegCompression,
                              bool isMaskImg = false,
                              BYTE **bitsAlpha = NULL,
                              DWORD *cbBitsAlpha = NULL)
{
   if (!pd || !pd->document || !pd->painter || !bmi || !bits || cxSrc == 0 || cySrc == 0) {
      return NULL;
   }

   RECT srcRect;
   srcRect.left = xSrc;
   srcRect.top = ySrc;
   srcRect.right = xSrc + cxSrc;
   srcRect.bottom = ySrc + cySrc;

   if (srcRect.right > bmi->bmiHeader.biWidth ||
      srcRect.bottom > bmi->bmiHeader.biHeight * (bmi->bmiHeader.biHeight < 0 ? -1 : +1)) {
      OffsetRect(&srcRect, bmi->bmiHeader.biWidth - srcRect.right, (bmi->bmiHeader.biHeight * (bmi->bmiHeader.biHeight < 0 ? -1 : +1)) - srcRect.bottom);

      if (srcRect.left < 0) {
         srcRect.left = 0;
      }
      if (srcRect.top < 0) {
         srcRect.top = 0;
      }
   }

   PdfImage *image = new PdfImage(pd->document);
   if (!image) {
      if (pd->on_error) {
         pd->on_error(ERROR_OUTOFMEMORY, "prepareImage: Out of memory", pd->on_error_user_data);
      }
      return NULL;
   }

   if (!fillPdfImageFromBitmap(pd, image, &srcRect, dwRop, iUsageColor, bmi, bits, cbBits, disableJpegCompression, isMaskImg, bitsAlpha, cbBitsAlpha)) {
      delete image;
      return NULL;
   }

   return image;
}

static void drawImage(struct processData *pd, int xSrc, int ySrc, int cxSrc, int cySrc, int xDest, int yDest, int cxDest, int cyDest,
   DWORD dwRop, DWORD iUsageColor, const BITMAPINFO *bmi, const BYTE *bits, DWORD cbBits, bool disableJpegCompression, const COLORREF *transparentColor = NULL)
{
   PdfImage *image = prepareImage(pd, xSrc, ySrc, cxSrc, cySrc, dwRop, iUsageColor, bmi, bits, cbBits, disableJpegCompression);
   if (image) {
      double dx, dy, dw, dh;

      LogicalUnitToPdf(pd, xDest, yDest, dx, dy);
      LogicalUnitToPdf(pd, xDest + cxDest, yDest + cyDest, dw, dh);

      dw -= dx;
      dh -= dy;

      if (transparentColor) {
         PdfArray array;

         array.push_back(PdfVariant(static_cast<pdf_int64>(GetRValue(*transparentColor))));
         array.push_back(PdfVariant(static_cast<pdf_int64>(GetRValue(*transparentColor))));
         array.push_back(PdfVariant(static_cast<pdf_int64>(GetGValue(*transparentColor))));
         array.push_back(PdfVariant(static_cast<pdf_int64>(GetGValue(*transparentColor))));
         array.push_back(PdfVariant(static_cast<pdf_int64>(GetBValue(*transparentColor))));
         array.push_back(PdfVariant(static_cast<pdf_int64>(GetBValue(*transparentColor))));

         image->GetObject()->GetDictionary().AddKey(PdfName("Mask"), PdfVariant(array));
      }

      pd->painter->DrawImage(dx, dy, image, dw / (double) cxSrc, dh / (double) cySrc);

      delete image;
   } else {
      if (pd->on_error) {
         pd->on_error(ERROR_INVALID_PARAMETER, "Failed to convert image", pd->on_error_user_data);
      }
   }
}

static void drawMaskedImage(struct processData *pd,
   int xMask, int yMask, int cxMask, int cyMask, DWORD iUsageColorMask,
   const BITMAPINFO *bmiMask, const BYTE *bitsMask, DWORD cbBitsMask,
   int xSrc, int ySrc, int cxSrc, int cySrc, DWORD iUsageColorSrc,
   const BITMAPINFO *bmiSrc, const BYTE *bitsSrc, DWORD cbBitsSrc,
   int xDest, int yDest, int cxDest, int cyDest, DWORD dwRopSrc,
   bool isRelativeDest = false)
{
   if (!pd || !pd->document || !pd->painter || !bmiMask || !bitsMask || !cbBitsMask || !bmiSrc || !bitsSrc) {
      return;
   }

   PdfImage *imgMask = prepareImage(pd, xMask, yMask, cxMask, cyMask, SRCCOPY, iUsageColorMask, bmiMask, bitsMask, cbBitsMask, true, true);
   if (imgMask) {
      imgMask->GetObject()->GetDictionary().AddKey(PdfName("ImageMask"), PdfVariant(true));
      imgMask->GetObject()->GetDictionary().RemoveKey(PdfName("ColorSpace"));

      PdfImage *imgSrc = prepareImage(pd, xSrc, ySrc, cxSrc, cySrc, dwRopSrc, iUsageColorSrc,bmiSrc, bitsSrc, cbBitsSrc, true);
      if (imgSrc) {
         imgSrc->GetObject()->GetDictionary().AddKey(PdfName("Mask"), imgMask->GetObject()->Reference());

         double dx, dy, dw, dh;

         LogicalUnitToPdf(pd, xDest, yDest, dx, dy, true, isRelativeDest);
         LogicalUnitToPdf(pd, xDest + cxDest, yDest + cyDest, dw, dh, true, isRelativeDest);

         dw -= dx;
         dh -= dy;

         if (isRelativeDest) {
            dx = 0.0;
            dy = 0.0;
         }

         pd->painter->DrawImage(dx, dy, imgSrc, dw / (double) cxDest, dh / (double) cyDest);

         delete imgSrc;
      }

      delete imgMask;
   }
}

static void drawPath(struct processData *pd, bool onlyStroke, bool polyFillMode = false)
{
   if (!pd || pd->buildingPath || !pd->painter) {
      return;
   }

   PdfPainter *painter = pd->painter;
   bool alreadyFilled = false;
   PdfStream *m_pCanvas = painter->GetCanvas();

   PODOFO_RAISE_LOGIC_IF(!m_pCanvas, "Call SetPage() first before doing drawing operations.");

   painter->Save();

   if (pd->currentDCState.penSet && pd->currentDCState.iBkMode == OPAQUE && pd->currentDCState.bkColorSet) {
      if (selectPen(pd, true)) {
         std::string path = painter->GetCurrentPath().str();
         COLORREF color = pd->currentDCState.bkColor;

         painter->SetStrokingColor(GetRValue(color)/255.0, GetGValue(color)/255.0, GetBValue(color)/255.0);
         if (!onlyStroke && selectBrush(pd)) {
            painter->FillAndStroke(polyFillMode ? pd->currentDCState.iPolyFillMode == ALTERNATE: false);
            alreadyFilled = true;
         } else {
            painter->Stroke();
         }

         if (!path.empty()) {
            m_pCanvas->Append(path.c_str());
         }
      }
   }

   if (!alreadyFilled && !onlyStroke && selectBrush(pd)) {
      if (selectPen(pd) || selectExtPen(pd)) {
         painter->FillAndStroke(polyFillMode ? pd->currentDCState.iPolyFillMode == ALTERNATE: false);
      } else {
         painter->Fill(polyFillMode ? pd->currentDCState.iPolyFillMode == ALTERNATE: false);
      }
   } else if (selectPen(pd) || selectExtPen(pd)) {
      painter->Stroke();
   } else {
      painter->EndPath();
   }

   painter->Restore();
}

/* returns angle in radians of AB and AC */
static double get_angle_rad(double ax, double ay, double bx, double by, double cx, double cy)
{
   if (double_equal (ax, bx)) {
      return 0.0;
   }

   if (double_equal (ax, cx)) {
      if (double_equal (ay, cy)) {
         return 0;
      }

      if (ay > cy) {
         return M_PI / 2.0;
      }

      return 3.0 * M_PI / 2.0;
   }

   double k_ab = (by - ay) / (bx - ax);
   double k_ac = (cy - ay) / (cx - ax);

   if (double_equal (k_ab * k_ac, -1.0)) {
      return 0.0;
   }

   double toabs = (k_ab - k_ac) / (1.0 + k_ab * k_ac);
   double res = atan(fabs(toabs));

   if (cx < ax) {                   // correct for the quadrant of C
      if (cy < ay) {                //          ^
         res = M_PI - res;          //          |  +
      } else {                      //          | /C
         res = M_PI + res;          //          |/
      }                             //   -------+------+-->
   } else if (cy > ay) {            //         A|      B
      res = 2.0 * M_PI - res;       //          |
   }                                //          |

   return res;
}

static void drawArcPath(struct processData *pd, double rx, double ry, double rw, double rh, double startx, double starty, double endx, double endy, bool counterclockwise, bool updatePos = false, double minStep = 0.1)
{
   if (!pd || !pd->painter) {
      return;
   }

   double start_angle_rad, end_angle_rad;
   double a = rw / 2.0, b = rh / 2.0, angle;
   double midx = rx + rw / 2.0, midy = ry + rh / 2.0;

   start_angle_rad = get_angle_rad(midx, midy, midx + a, midy, startx, starty);
   end_angle_rad = get_angle_rad(midx, midy, midx + a, midy, endx, endy);

   if (start_angle_rad < -1e-6 || start_angle_rad > 2.0 * M_PI + 1e-6) {
      return;
   }
   if (end_angle_rad < -1e-6 || end_angle_rad > 2.0 * M_PI + 1e-6) {
      return;
   }

   minStep *= minStep;

   std::map<double, std::pair<double, double> > points; // angle -> [x,y] with [rx, ry] = [0, 0]

   for (angle = 0.0; angle <= M_PI / 2.0; angle += 0.001 * M_PI / 2.0) {
      double x, y;

      x = cos(angle) * a;
      y = - sqrt((b * b * ((a * a) - (x * x))) / (a * a));

      if (points.empty() || ((points.rbegin()->second.first - x) * (points.rbegin()->second.first - x) + (points.rbegin()->second.second - y) * (points.rbegin()->second.second - y)) >= minStep) {
         points[angle] = std::pair<double, double>(x, y);
      }
   }

   struct Quadrant {
      double multx, multy, rad_offset;
   } quadrant[4] = {
      {  1.0,  1.0, 0                }, //   0 ..  90 deg
      { -1.0,  1.0, M_PI / 2.0       }, //  90 .. 180 deg
      { -1.0, -1.0, M_PI             }, // 180 .. 270 deg
      {  1.0, -1.0, 3.0 * M_PI / 2.0 }  // 270 .. 360 deg
   };

   int q;
   for (q = 0; q < 4; q++) {
      if (start_angle_rad >= quadrant[q].rad_offset && start_angle_rad <= quadrant[q].rad_offset + M_PI / 2.0) {
         break;
      }
   }

   if (q == 4) {
      return;
   }

   std::map<double, std::pair<double, double> >::const_iterator it, end = points.end(), begin = points.begin();
   if (!(q & 1)) {
      for (it = begin; it != end; it++) {
         if (it->first + quadrant[q].rad_offset >= start_angle_rad) {
            break;
         }
      }
   } else {
      it = end;
      for (it--; it != begin; it--) {
         if (quadrant[q].rad_offset + (M_PI / 2.0) - it->first >= start_angle_rad) {
            break;
         }
      }
   }

   if (it == end) {
      it--;
   }

   if (double_equal(start_angle_rad - end_angle_rad, 0.0)) {
      angle = 2 * M_PI;
   } else if (counterclockwise) {
      angle = start_angle_rad - end_angle_rad;
   } else {
      angle = end_angle_rad - start_angle_rad;
   }

   if (angle < -1e-6) {
      angle += 2.0 * M_PI;
   }

   bool first = !pd->buildingPath;
   double multx, multy, x, y, last_angle = it->first;
   int ii, qq;
   for (ii = 0; ii <= 4 && angle >= 1e-6; ii++) {
      qq = (4 + q + (ii * (counterclockwise ? -1 : 1))) % 4;
      multx = quadrant[qq].multx;
      multy = quadrant[qq].multy;

      while (angle = angle - fabs(last_angle - it->first), angle >= 1e-6) {
         last_angle = it->first;
         x = midx + (multx) * it->second.first;
         y = midy + (multy) * it->second.second;

         if (first) {
            first = false;
            if (updatePos) {
               pd->painter->MoveTo(pd->current_posX, pd->current_posY);
               pd->painter->LineTo(x, y);
               drawPath(pd, true);
            }
            pd->painter->MoveTo(x, y);
         } else {
            pd->painter->LineTo(x, y);
         }

         if (updatePos) {
            pd->current_posX = x;
            pd->current_posY = y;
         }

         if (!(qq & 1)) {
            if (!counterclockwise) {
               it++;
               if (it == end) {
                  it--;
                  break;
               }
            } else if (it == begin) {
               break;
            } else {
               it--;
            }
         } else {
            if (!counterclockwise) {
               if (it == begin) {
                  break;
               }

               it--;
            } else {
               it++;
               if (it == end) {
                  it--;
                  break;
               }
            }
         }
      }
   }
}

static void storeState(struct processData *pd)
{
   if (!pd || !pd->painter) {
      return;
   }

   pd->savedStates.push_back(pd->currentDCState);
   pd->painter->Save();

   if (pd->currentDCState.clipRgn) {
      HRGN rgn = pd->currentDCState.clipRgn;

      // create copy of the current clipping region (or whole page, if not clipping);
      pd->currentDCState.clipRgn = CreateRectRgn(0, 0, 1, 1);
      CombineRgn (pd->currentDCState.clipRgn, rgn, rgn, RGN_COPY);
   }

   pd->currentDCState.isClipping = false;
}

static void restoreState(struct processData *pd)
{
   if (!pd || !pd->painter) {
      return;
   }

   while (pd->currentDCState.metaRgnCount > 0) {
      pd->painter->Restore();
      pd->currentDCState.metaRgnCount--;
   }

   if (pd->currentDCState.isClipping) {
      pd->painter->Restore();
      pd->currentDCState.isClipping = false;
   }

   if (pd->currentDCState.clipRgn) {
      DeleteObject(pd->currentDCState.clipRgn);
      pd->currentDCState.clipRgn = NULL;
   }

   pd->painter->Restore();
   pd->currentDCState = *pd->savedStates.rbegin();
   pd->savedStates.pop_back();
}

static void multiplyMatrixes(DWorldMatrix &result, const DWorldMatrix left, const DWorldMatrix right)
{
   result.eM11 = (left.eM11 * right.eM11) + (left.eM12 * right.eM21);
   result.eM12 = (left.eM11 * right.eM12) + (left.eM12 * right.eM22);
   result.eM21 = (left.eM21 * right.eM11) + (left.eM22 * right.eM21);
   result.eM22 = (left.eM21 * right.eM12) + (left.eM22 * right.eM22);
   result.eDx  = (left.eDx  * right.eM11) + (left.eDy  * right.eM21) + right.eDx;
   result.eDy  = (left.eDx  * right.eM12) + (left.eDy  * right.eM22) + right.eDy;
}

static void maybePushWorldMatrix(struct processData *pd)
{
   if (!pd || !pd->painter || !pd->currentDCState.worldMatrixValid) {
      return;
   }

   if (pd->currentDCState.worldMatrixApplied) {
      PODOFO_RAISE_ERROR_INFO (ePdfError_InternalLogic, "World matrix already applied");
   }

   double eDx = 0.0, eDy = 0.0;
   bool isOnlyTranslate = double_equal (pd->currentDCState.worldMatrix.eM11, 1.0) && double_equal (pd->currentDCState.worldMatrix.eM12, 0.0) &&
                          double_equal (pd->currentDCState.worldMatrix.eM21, 0.0) && double_equal (pd->currentDCState.worldMatrix.eM22, 1.0);

   pd->currentDCState.worldMatrixValid = false;
   LogicalUnitToPdf(pd, pd->currentDCState.worldMatrix.eDx, pd->currentDCState.worldMatrix.eDy, eDx, eDy, !isOnlyTranslate);
   pd->currentDCState.worldMatrixValid = true;
   pd->currentDCState.worldMatrixApplied = true;

   pd->painter->Save();

   DWorldMatrix worldMatrix = pd->currentDCState.worldMatrix;

   worldMatrix.eDx = 0.0;
   worldMatrix.eDy = 0.0; 

   if (isOnlyTranslate) {
      /* just a translation matrix, no rotation or scale, thus ignore it */
      eDy *= -1.0;
   } else {
      DWorldMatrix mult;

      mult.eM11 = 1.0;
      mult.eM12 = 0.0;
      mult.eM21 = 0.0;
      mult.eM22 = -1.0;
      mult.eDx = 0.0;
      mult.eDy = 0.0; 

      if ((pd->currentDCState.iMapMode == MM_ANISOTROPIC || pd->currentDCState.iMapMode == MM_ISOTROPIC) && (pd->viewportExt.cy < 0 || pd->windowExt.cy < 0)) {
         multiplyMatrixes(worldMatrix, mult, worldMatrix);
      } else {
         multiplyMatrixes(worldMatrix, worldMatrix, mult);
      }
   }

   pd->painter->SetTransformationMatrix(
      worldMatrix.eM11, worldMatrix.eM12,
      worldMatrix.eM21, worldMatrix.eM22,
      eDx, eDy);
}

static void maybePopWorldMatrix(struct processData *pd)
{
   if (!pd || !pd->painter || !pd->currentDCState.worldMatrixApplied) {
      return;
   }

   pd->currentDCState.worldMatrixApplied = false;
   pd->painter->Restore();
}

static bool drawTextWithICSpacing(struct processData *pd,
                                  PdfPainter *painter,
                                  double dX,
                                  double dY,
                                  const PdfString &str,
                                  COLORREF textColor,
                                  const DWORD *Dx,
                                  DWORD nDx)
{
   if (!painter || !Dx || !nDx || nDx != str.GetCharacterLength() || !str.IsUnicode()) {
      return false;
   }

   std::wstring wstr = str.GetStringW();
   if (wstr.size() != nDx) {
      return false;
   }

   PdfCanvas *m_pPage = painter->GetPage();
   PdfStream *m_pCanvas = painter->GetCanvas();
   PdfFont *m_pFont = painter->GetFont();

   PODOFO_RAISE_LOGIC_IF(!m_pCanvas, "Call SetPage() first before doing drawing operations.");

   if(!m_pFont || !m_pPage || !str.IsValid()) {
      PODOFO_RAISE_ERROR( ePdfError_InvalidHandle );
   }

   double subtrWidth = static_cast<double>(m_pFont->GetFontSize() * m_pFont->GetFontScale() / 100.0) / 1000.0;

   if (double_equal(m_pFont->GetFontSize(), 0.0) ||
       double_equal(m_pFont->GetFontScale(), 0.0) ||
       double_equal(m_pFont->GetFontCharSpace(), 0.0) ||
       double_equal(subtrWidth, 0.0)) {
      return false;
   }

   m_pPage->AddResource(m_pFont->GetIdentifier(), m_pFont->GetObject()->Reference(), PdfName("Font"));
   if (m_pFont->IsSubsetting()) {
      m_pFont->AddUsedSubsettingGlyphs(str, nDx);
   }

   std::ostringstream  m_oss;
   m_oss.flags(std::ios_base::fixed);
   m_oss.precision(3L);
   PdfLocaleImbue(m_oss);

   m_oss.str("");
   m_oss << "BT" << std::endl << "/" << m_pFont->GetIdentifier().GetName()
         << " "  << m_pFont->GetFontSize()
         << " Tf" << std::endl;

   if (painter->GetTextRenderingMode() != ePdfTextRenderingMode_Fill) {
      m_oss << (int) painter->GetTextRenderingMode() << " Tr" << std::endl;
	}

   //if( m_pFont->GetFontScale() != 100.0F ) - this value is kept between text blocks
   m_oss << m_pFont->GetFontScale() << " Tz" << std::endl;

   //if( m_pFont->GetFontCharSpace() != 0.0F )  - this value is kept between text blocks
   m_oss << m_pFont->GetFontCharSpace() * m_pFont->GetFontSize() / 100.0 << " Tc" << std::endl;

   m_oss << dX << " " << dY << " Td" << std::endl;

   m_pCanvas->Append(m_oss.str());

   DWORD ii;
   double decWidth = static_cast<double>(m_pFont->GetFontSize() * m_pFont->GetFontScale() / 100.0 * m_pFont->GetFontCharSpace() / 100.0);
   double cWidthScaled, cWidth, needPdfDx;
   double pdfWidth = 0.0;
   const wchar_t *wchars = wstr.c_str();
   std::wstring wStringRow = L"";

   // an array of "character,add_spacing" pairs
   m_pCanvas->Append("[");

   PdfString onechar, pdfRow;
   for (ii = 0; ii < nDx; ii++) {
      onechar.setFromWchar_t(wchars + ii, 1);
      wStringRow += onechar.GetStringW();

      cWidthScaled = m_pFont->GetFontMetrics()->StringWidth(onechar);
      cWidth = (cWidthScaled - decWidth) / subtrWidth;
      needPdfDx = Dx[ii];

      pdfWidth += cWidthScaled;

      ABC abc;
      if (GetCharABCWidthsW(pd->replayDC, wchars[ii], wchars[ii], &abc)) {
         double defWidth = abc.abcA + abc.abcB + abc.abcC;
         if (!double_equal (defWidth, 0.0)) {
            pdfWidth = pdfWidth - cWidthScaled + (cWidthScaled * needPdfDx / defWidth);
            needPdfDx = (cWidth * needPdfDx / defWidth) - cWidth;
            pdf_long lPdfDx = static_cast<pdf_long>(needPdfDx);

            if (lPdfDx != 0 && ii + 1 < nDx) {
               m_oss.str("");
               m_oss << -lPdfDx;

               pdfRow.setFromWchar_t(wStringRow.c_str(), wStringRow.size());
               m_pFont->WriteStringToStream(pdfRow, m_pCanvas);

               wStringRow = L"";

               m_pCanvas->Append(m_oss.str());
            }
         }
      }
   }

   if (!wStringRow.empty()) {
      pdfRow.setFromWchar_t(wStringRow.c_str(), wStringRow.size());
      m_pFont->WriteStringToStream(pdfRow, m_pCanvas);
   }

   m_pCanvas->Append("]");

   m_pCanvas->Append(" TJ\nET\n");

   if(m_pFont->IsUnderlined() || m_pFont->IsStrikeOut()) {
      painter->Save();
      painter->SetStrokingColor(GetRValue(textColor)/255.0, GetGValue(textColor)/255.0, GetBValue(textColor)/255.0);

      // Draw underline
      if (m_pFont->IsUnderlined()) {
         painter->SetStrokeWidth(m_pFont->GetFontMetrics()->GetUnderlineThickness());
         painter->DrawLine(dX,
                           dY + m_pFont->GetFontMetrics()->GetUnderlinePosition(),
                           dX + pdfWidth,
                           dY + m_pFont->GetFontMetrics()->GetUnderlinePosition());
      }

      // Draw strikeout
      if (m_pFont->IsStrikeOut()) {
         painter->SetStrokeWidth( m_pFont->GetFontMetrics()->GetStrikeoutThickness());
         painter->DrawLine(dX,
                           dY + m_pFont->GetFontMetrics()->GetStrikeOutPosition(),
                           dX + pdfWidth,
                           dY + m_pFont->GetFontMetrics()->GetStrikeOutPosition());
      }

      painter->Restore();
   }

   return true;
}

static void drawText(struct processData *pd, const ENHMETARECORD *lpEMFR, const EMRTEXT &emrtext, bool switchY)
{
   if (!pd || !pd->painter || !pd->replayDC || pd->buildingPath) {
      return;
   }

   PdfPainter *painter = pd->painter;
   wchar_t *newString = NULL;

   maybePushWorldMatrix(pd);

   if (emrtext.nChars > 0) {
      const wchar_t *wstr = (const wchar_t *) STRUCT_OFFSET(lpEMFR, emrtext.offString);
      RECT wstrRect = {0, 0, 0, 0};
      RECTL rcl;

      DrawTextW(pd->replayDC, wstr, emrtext.nChars, &wstrRect, DT_NOCLIP | DT_CALCRECT | DT_SINGLELINE);

      if (emrtext.rcl.left == 0 && emrtext.rcl.top == 0 && emrtext.rcl.right == -1 && emrtext.rcl.bottom == -1) {
         rcl.left = wstrRect.left + emrtext.ptlReference.x;
         rcl.top = wstrRect.top + (emrtext.ptlReference.y * (switchY ? -1 : 1));
         rcl.right = wstrRect.right + emrtext.ptlReference.x;
         rcl.bottom = wstrRect.bottom + (emrtext.ptlReference.y * (switchY ? -1 : 1));
      } else {
         rcl = emrtext.rcl;
      }

      double dx, dy, w, h;

      LogicalUnitToPdf(pd, emrtext.ptlReference.x, emrtext.ptlReference.y, dx, dy, (pd->currentDCState.textAlign & TA_UPDATECP) == 0);
      LogicalUnitToPdf(pd, emrtext.ptlReference.x + wstrRect.right - wstrRect.left, emrtext.ptlReference.y + wstrRect.bottom - wstrRect.top, w, h, (pd->currentDCState.textAlign & TA_UPDATECP) == 0);

      w -= dx;
      h -= dy;

      double pdfX, pdfY, incX = 0.0, incY = 0.0;
      bool forceFontScale = false;

      painter->Save();

      if (!selectFont(pd)) {
         PdfFont *font;

         /* fallback, because PoDoFo throws exception if no font is set; this one is not embed */
         const char *substName = getSubstituteFontName ("Arial", false, false, false);
         font = pd->document->CreateFont(substName, false, false, false, PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), PdfFontCache::eFontCreationFlags_AutoSelectBase14, false);
         pd->encodingsCache->populateBase14FontWidths(font);
         pd->currentDCState.fontAngle = 0;

         font->SetFontSize(3.0);

         painter->SetFont(font);
      } else {
         forceFontScale = pd->currentDCState.setFont.lfWidth != 0;
      }

      wstr = pd->encodingsCache->maybeFixSymbolCharsetString(painter->GetFont(), wstr, emrtext.nChars, &newString);

      PdfString str;
      str.setFromWchar_t ((const wchar_t *) wstr, emrtext.nChars);
      pdf_long ii, strLeng = str.GetCharacterLength();
      pdf_utf16be *utf16be = reinterpret_cast<pdf_utf16be*>(str.GetBuffer().GetBuffer());

      // replace tab ('\t') with a "rectangle" character
      for (ii = 0; ii < strLeng && utf16be; ii++) {
         if (utf16be[ii] == 0x0900) {
            utf16be[ii] = 0xA125;
         }
      }

      bool rotated = (pd->currentDCState.fontAngle % 360) != 0;

      if ((pd->currentDCState.textAlign & (TA_TOP | TA_BASELINE | TA_BOTTOM)) == TA_BASELINE) {
         incY += pd->currentDCState.fontAscent;
      } else if ((pd->currentDCState.textAlign & (TA_TOP | TA_BASELINE | TA_BOTTOM)) == TA_BOTTOM) {
         incY += pd->currentDCState.fontAscent + pd->currentDCState.fontDescent;
      } else { // TA_TOP
         //incY += pd->currentDCState.fontAscent;
      }

      if ((pd->currentDCState.textAlign & (TA_LEFT | TA_CENTER | TA_RIGHT)) == TA_CENTER) {
         incX -= w / 2.0;
      } else if ((pd->currentDCState.textAlign & (TA_LEFT | TA_CENTER | TA_RIGHT)) == TA_RIGHT) {
         incX -= w;
      } // else TA_LEFT

      if ((pd->currentDCState.textAlign & TA_UPDATECP) != 0) {
         dx += pd->current_posX;
         dy += pd->current_posY;

         pd->current_posX = dx + w;
      }

      if ((emrtext.fOptions & (ETO_OPAQUE | ETO_CLIPPED)) != 0) {
         double rclx, rcly, rclw, rclh;

         LogicalUnitToPdf(pd, rcl.left, rcl.top, rclx, rcly);
         LogicalUnitToPdf(pd, rcl.right, rcl.bottom, rclw, rclh);

         rclw -= rclx;
         rclh -= rcly;

         if ((emrtext.fOptions & ETO_CLIPPED) != 0) {
            painter->Rectangle(rclx, rcly, rclw, rclh);
            painter->Clip();
            painter->EndPath();
         }

         if ((emrtext.fOptions & ETO_OPAQUE) != 0 && pd->currentDCState.bkColorSet) {
            painter->SetColor(GetRValue(pd->currentDCState.bkColor)/255.0, GetGValue(pd->currentDCState.bkColor)/255.0, GetBValue(pd->currentDCState.bkColor)/255.0);
            painter->Rectangle(rclx, rcly, rclw, rclh);
            painter->Fill();
         }
      }

      pdfX = dx + incX;
      pdfY = dy + incY - pd->currentDCState.fontAscent;

      double textw = painter->GetFont()->GetFontMetrics()->StringWidth(str);

      if (rotated) {
         double angle = (pd->currentDCState.fontAngle / 180.0) * M_PI;
         double sval = sin(angle), cval = cos(angle);
         int inc = angle / 90;

         sval += inc;
         cval += inc;

         if (!double_equal(incX, 0.0) || !double_equal(incY, 0.0)) {
            pdfX = dx - (incX * (switchY ? -1 : 1));
            pdfY = dy;
         } else if (switchY) {
            pdfX = dx + incX - ((pd->currentDCState.fontAscent) * sval);
            pdfY = dy + incY - ((pd->currentDCState.fontAscent) * cval);
         } else {
            pdfX = dx - incX + ((pd->currentDCState.fontAscent) * sval);
            pdfY = dy - incY - ((pd->currentDCState.fontAscent) * cval);
         }

         if (switchY) {
            sval *= -1.0;
         }

         painter->SetTransformationMatrix(cval, sval, -sval, cval, pdfX, pdfY);

         pdfX = incX;
         pdfY = incY;

         if ((pd->currentDCState.textAlign & (TA_LEFT | TA_CENTER | TA_RIGHT)) != TA_LEFT) {
            //pdfX += pd->currentDCState.fontAscent;
         }

         if ((pd->currentDCState.textAlign & (TA_TOP | TA_BASELINE | TA_BOTTOM)) != TA_TOP) {
            pdfY -= pd->currentDCState.fontAscent;
         }
      }

      if (!rotated || pd->currentDCState.fontAngle == 0 || pd->currentDCState.fontAngle == 180) {
         if (forceFontScale) {
            painter->GetFont()->SetFontScale(100.0 * w / textw);
         } else if ((textw + 1 < w || textw > w) && emrtext.nChars > 1) {
            painter->GetFont()->SetFontScale(100.0);
            painter->GetFont()->SetFontCharSpace(100.0);
            double tw0, tw1;
            tw0 = painter->GetFont()->GetFontMetrics()->StringWidth(str);
            painter->GetFont()->SetFontCharSpace(200.0);
            tw1 = painter->GetFont()->GetFontMetrics()->StringWidth(str);
            if (tw1 > tw0) {
               painter->GetFont()->SetFontCharSpace(100.0 * (w - textw) / (tw1 - tw0));
            }
         } else {
            painter->GetFont()->SetFontScale(100.0);
         }
      } else {
         painter->GetFont()->SetFontScale(100.0);
      }

      if (pd->currentDCState.bkColorSet && pd->currentDCState.iBkMode == OPAQUE) {
         if (pd->isWindows6x && emrtext.fOptions == 0 && emrtext.nChars == 1 && wstr[0] == 0x20) {
            DWORD *Dx = (DWORD *) STRUCT_OFFSET (lpEMFR, emrtext.offDx);
            if ((BYTE *) Dx < (BYTE *) (lpEMFR + lpEMFR->nSize) && (BYTE *) Dx + (sizeof (DWORD) * emrtext.nChars) <= (BYTE *) (lpEMFR + lpEMFR->nSize)) {
               textw = Dx[0];
            } else {
               textw = 0.0;
            }

            double fakeY = 0;
            LogicalUnitToPdf(pd, textw, 0, textw, fakeY, false, false);
         } else {
            textw = painter->GetFont()->GetFontMetrics()->StringWidth(str);
         }

         double texth = pd->currentDCState.fontAscent + pd->currentDCState.fontDescent;

         painter->SetColor(GetRValue(pd->currentDCState.bkColor)/255.0, GetGValue(pd->currentDCState.bkColor)/255.0, GetBValue(pd->currentDCState.bkColor)/255.0);
         painter->Rectangle(pdfX, pdfY - pd->currentDCState.fontDescent, textw, texth);
         painter->Fill();
      }

      painter->SetColor(GetRValue(pd->currentDCState.textColor)/255.0, GetGValue(pd->currentDCState.textColor)/255.0, GetBValue(pd->currentDCState.textColor)/255.0);
      if (emrtext.offDx > 0) {
         DWORD *Dx = (DWORD *) STRUCT_OFFSET (lpEMFR, emrtext.offDx);
         if ((BYTE *) Dx < (BYTE *) (lpEMFR + lpEMFR->nSize) && (BYTE *) Dx + (sizeof (DWORD) * emrtext.nChars) <= (BYTE *) (lpEMFR + lpEMFR->nSize) &&
            drawTextWithICSpacing(pd, painter, pdfX, pdfY, str, pd->currentDCState.textColor, Dx, emrtext.nChars)) {
            /* text was drawn properly */
         } else {
            painter->DrawText(pdfX, pdfY, str);
         }
      } else {
         painter->DrawText(pdfX, pdfY, str);
      }
      painter->GetFont()->SetFontCharSpace(100.0);
      painter->GetFont()->SetFontScale(100.0);

      painter->Restore();

      /*{ // this draws text reference point, as computed by litePDF
         painter->Save();
         painter->SetStrokingColor(0.0, 0.5, 0.5);
         painter->SetStrokeWidth(0.1);
         painter->SetStrokeStyle(ePdfStrokeStyle_Solid);
         painter->MoveTo(pdfX - 1.0, pdfY - 1.0);
         painter->LineTo(pdfX + 1.0, pdfY + 1.0);
         painter->Stroke();
         painter->MoveTo(pdfX + 1.0, pdfY - 1.0);
         painter->LineTo(pdfX - 1.0, pdfY + 1.0);
         painter->Stroke();
         painter->Restore();
      } // */

      /*{ // this draws expected text bounds, as reported by GDI

         painter->Save();
         painter->SetStrokeWidth(0.1);
         painter->SetStrokeStyle (ePdfStrokeStyle_Dot);

         painter->SetStrokingColor(0.0, 0.5, 0.0);
         painter->Rectangle(dx, dy, w, h);
         painter->Stroke();

         painter->SetStrokingColor(0.5, 0.0, 0.0);
         painter->Rectangle(dx + painter->GetFont()->GetFontMetrics()->StringWidth(str), dy - 1, 1, h + 2);
         painter->Stroke();

         LogicalUnitToPdf(pd, emrtext.ptlReference.x, emrtext.ptlReference.y, dx, dy);
         painter->SetStrokingColor(0.5, 0.0, 0.5);
         painter->SetStrokeWidth(0.1);
         painter->SetStrokeStyle(ePdfStrokeStyle_Solid);
         painter->MoveTo(dx - 1.0, dy - 1.0);
         painter->LineTo(dx + 1.0, dy + 1.0);
         painter->Stroke();
         painter->MoveTo(dx + 1.0, dy - 1.0);
         painter->LineTo(dx - 1.0, dy + 1.0);
         painter->Stroke();

         painter->Restore();
      } // */
   } else if ((emrtext.fOptions & ETO_OPAQUE) != 0 && pd->currentDCState.bkColorSet &&
      (emrtext.rcl.left != 0 || emrtext.rcl.top != 0 || emrtext.rcl.right != 0 || emrtext.rcl.bottom != 0)) {
      double rclx, rcly, rclw, rclh;
      RECTL rcl;

      rcl = emrtext.rcl;

      if (switchY) {
         rcl.top = rcl.top * (-1);
         rcl.bottom = rcl.bottom * (-1);
      }

      LogicalUnitToPdf(pd, rcl.left, rcl.top, rclx, rcly, true, true);
      LogicalUnitToPdf(pd, rcl.right, rcl.bottom, rclw, rclh, true, true);

      rclw -= rclx;
      rclh -= rcly;

      painter->Save();
      painter->SetColor(GetRValue(pd->currentDCState.bkColor)/255.0, GetGValue(pd->currentDCState.bkColor)/255.0, GetBValue(pd->currentDCState.bkColor)/255.0);
      painter->Rectangle(rclx, rcly, rclw, rclh);
      painter->Fill();
      painter->Restore();
   }

   maybePopWorldMatrix(pd);

   if (newString) {
      free(newString);
   }
}

static RECT fillRECT(int left, int top, int right, int bottom)
{
   RECT rect;

   rect.left = left;
   rect.top = top;
   rect.right = right;
   rect.bottom = bottom;

   return rect;
}

static RECT fillRelRECT(int left, int top, int width, int height)
{
   return fillRECT(left, top, left + width, top + height);
}

static vector<RECT> evalRegionBorders(const RECT *rect,
                                      const SIZE &szlStroke,
                                      map<int, vector <const RECT *> > &rleft,
                                      map<int, vector <const RECT *> > &rtop,
                                      map<int, vector <const RECT *> > &rright,
                                      map<int, vector <const RECT *> > &rbottom)
{
   vector<RECT> borders;

   if (!rect) {
      return borders;
   }

   int cx = szlStroke.cx, cy = szlStroke.cy;

   if (rright.find(rect->left) != rright.end()) {
      vector <const RECT *> &vrects = rright[rect->left];
      vector <const RECT *>::const_iterator it, end = vrects.end();
      int top = rect->top, bottom = rect->bottom;

      for (it = vrects.begin(); it != end && top < bottom; it++) {
         const RECT *r = *it;

         if (!r || r == rect) {
            continue;
         }

         if (bottom <= r->top) {
            break;
         }

         if (top < r->top) {
            int decy = (top + cy < r->top) ? cy : top + cy - r->top;
            if (top - rect->top < decy) {
               decy = top - rect->top;
            }

            int incy = rect->bottom - r->top;
            if (incy > cy) {
               incy = cy;
            }

            borders.push_back(fillRelRECT(rect->left, top - decy, cx, r->top - top + decy + incy));
         }

         if (r->bottom > top) {
            top = r->bottom;
         }
      }

      if (top < bottom) {
         int decy = cy;
         if (top - rect->top < decy) {
            decy = top - rect->top;
         }
         if (decy < 0) {
            decy = 0;
         }

         borders.push_back(fillRelRECT(rect->left, top - decy, cx, bottom - top));
      }
   } else {
      borders.push_back(fillRelRECT(rect->left, rect->top, cx, rect->bottom - rect->top));
   }

   if (rleft.find(rect->right) != rleft.end()) {
      vector <const RECT *> &vrects = rleft[rect->right];
      vector <const RECT *>::const_iterator it, end = vrects.end();
      int top = rect->top, bottom = rect->bottom;

      for (it = vrects.begin(); it != end && top < bottom; it++) {
         const RECT *r = *it;

         if (!r || r == rect) {
            continue;
         }

         if (bottom <= r->top) {
            break;
         }

         if (top < r->top) {
            int decy = (top + cy < r->top) ? cy : top + cy - r->top;
            if (top - rect->top < decy) {
               decy = top - rect->top;
            }

            int incy = rect->bottom - r->top;
            if (incy > cy) {
               incy = cy;
            }

            borders.push_back(fillRelRECT(rect->right - cx, top - decy, cx, r->top - top + decy + incy));
         }

         if (r->bottom > top) {
            top = r->bottom;
         }
      }

      if (top < bottom) {
         int decy = cy;
         if (top - rect->top < decy) {
            decy = top - rect->top;
         }
         if (decy < 0) {
            decy = 0;
         }

         borders.push_back(fillRelRECT(rect->right - cx, top - decy, cx, bottom - top));
      }
   } else {
         borders.push_back(fillRelRECT(rect->right - cx, rect->top, cx, rect->bottom - rect->top));
   }

   if (rbottom.find(rect->top) != rbottom.end()) {
      vector <const RECT *> &vrects = rbottom[rect->top];
      vector <const RECT *>::const_iterator it, end = vrects.end();
      int left = rect->left, right = rect->right;

      for (it = vrects.begin(); it != end && left < right; it++) {
         const RECT *r = *it;

         if (!r || r == rect) {
            continue;
         }

         if (right <= r->left) {
            break;
         }

         if (left < r->left) {
            int decx = (left + cx < r->left) ? cx : left + cx - r->left;
            if (left - rect->left < decx) {
               decx = left - rect->left;
            }

            int incx = rect->right - r->left;
            if (incx > cx) {
               incx = cx;
            }

            borders.push_back(fillRelRECT(left - decx, rect->top, r->left - left + decx + incx, cy));
         }

         if (r->right > left) {
            left = r->right;
         }
      }

      if (left < right) {
         int decx = cx;
         if (left - rect->left < decx) {
            decx = left - rect->left;
         }
         if (decx < 0) {
            decx = 0;
         }

         borders.push_back(fillRelRECT(left - decx, rect->top, right - left, cy));
      }
   } else {
      borders.push_back(fillRelRECT(rect->left, rect->top, rect->right - rect->left, cy));
   }

   if (rtop.find(rect->bottom) != rtop.end()) {
      vector <const RECT *> &vrects = rtop[rect->bottom];
      vector <const RECT *>::const_iterator it, end = vrects.end();
      int left = rect->left, right = rect->right;

      for (it = vrects.begin(); it != end && left < right; it++) {
         const RECT *r = *it;

         if (!r || r == rect) {
            continue;
         }

         if (right <= r->left) {
            break;
         }

         if (left < r->left) {
            int decx = (left + cx < r->left) ? cx : left + cx - r->left;
            if (left - rect->left < decx) {
               decx = left - rect->left;
            }

            int incx = rect->right - r->left;
            if (incx > cx) {
               incx = cx;
            }

            borders.push_back(fillRelRECT(left - decx, rect->bottom - cy, r->left - left + decx + incx, cy));
         }

         if (r->right > left) {
            left = r->right;
         }
      }

      if (left < right) {
         int decx = cx;
         if (left - rect->left < decx) {
            decx = left - rect->left;
         }
         if (decx < 0) {
            decx = 0;
         }

         borders.push_back(fillRelRECT(left - decx, rect->bottom - cy, right - left, cy));
      }
   } else {
      borders.push_back(fillRelRECT(rect->left, rect->bottom - cy, rect->right - rect->left, cy));
   }

   return borders;
}

static bool sortRECTbyLeft(const RECT *r1, const RECT *r2)
{
   return r1->left < r2->left;
}

static bool sortRECTbyTop(const RECT *r1, const RECT *r2)
{
   return r1->top < r2->top;
}

static void sortRegionRects(map<int, vector <const RECT *> > &rects, bool (* sortFunc)(const RECT *r1, const RECT *r2))
{
   map<int, vector <const RECT *> >::iterator it, end = rects.end();
   for (it = rects.begin(); it != end; it++) {
      sort(it->second.begin(), it->second.end(), sortFunc);
   }
}

static bool applyDCPath(struct processData *pd, HDC fromDC, HDC toDC = NULL)
{
   if (!pd || !fromDC || (!pd->painter && !toDC)) {
      return false;
   }

   LPPOINT points = NULL;
   LPBYTE types = NULL;
   int nPoints;
   double px, py;

   if (!toDC) {
      pd->painter->EndPath();
   }

   nPoints = GetPath(fromDC, NULL, NULL, 0);
   if (nPoints > 0) {
      points = (LPPOINT) malloc(sizeof(POINT) * nPoints);
      types = (LPBYTE) malloc(sizeof(BYTE) * nPoints);

      if (points && types) {
         nPoints = GetPath(fromDC, points, types, nPoints);
         DWORD ii;

         for (ii = 0; ii < nPoints; ii++) {
            switch(types[ii] & (~PT_CLOSEFIGURE)) {
            case PT_MOVETO:
               if (toDC) {
                  MoveToEx(toDC, points[ii].x, points[ii].y, NULL);
               } else {
                  LogicalUnitToPdf(pd, points[ii].x, points[ii].y, px, py);
                  pd->painter->MoveTo(px, py);
               }
               break;
            case PT_LINETO:
               if (toDC) {
                  LineTo(toDC, points[ii].x, points[ii].y);
               } else {
                  LogicalUnitToPdf(pd, points[ii].x, points[ii].y, px, py);
                  pd->painter->LineTo(px, py);
               }
               break;
            case PT_BEZIERTO:
               if (ii + 3 <= nPoints &&
                   (types[ii + 1] & (~PT_CLOSEFIGURE)) == PT_BEZIERTO &&
                   (types[ii + 2] & (~PT_CLOSEFIGURE)) == PT_BEZIERTO) {
                  if (toDC) {
                     PolyBezierTo(toDC, &(points[ii]), 3);
                  } else {
                     double x1, y1, x2, y2;

                     LogicalUnitToPdf(pd, points[ii].x,     points[ii].y, x1, y1);
                     LogicalUnitToPdf(pd, points[ii + 1].x, points[ii + 1].y, x2, y2);
                     LogicalUnitToPdf(pd, points[ii + 2].x, points[ii + 2].y, px, py);

                     pd->painter->CubicBezierTo(x1, y1, x2, y2, px, py);
                  }
                  ii += 2;
               } else {
                  if (pd->on_error) {
                     pd->on_error(ERROR_NOT_SUPPORTED, "Path with PT_BEZIERTO, but points don't match", pd->on_error_user_data);
                  }
               }
               break;
            default:
               if (pd->on_error) {
                  pd->on_error(ERROR_NOT_SUPPORTED, "Path of unknown type encountered", pd->on_error_user_data);
               }
               break;
            }

            if (types[ii] & PT_CLOSEFIGURE) {
               if (toDC) {
                  CloseFigure(toDC);
               } else {
                  pd->painter->ClosePath();
               }
            }
         }
      }
   }

   if (points) free(points);
   if (types) free(types);

   return nPoints > 0 && points && types;
}

// because PathToRegion() discards the path in an HDC
static HRGN convertPathToRegion(struct processData *pd)
{
   if (!pd || !pd->replayDC) {
      return NULL;
   }

   HRGN rgn = NULL;
   HDC dc = CreateCompatibleDC(pd->replayDC);
   if (!dc) {
      if (pd->on_error) {
         pd->on_error(ERROR_INVALID_PARAMETER, "convertPathToRegion: Failed to create DC", pd->on_error_user_data);
      }
      return NULL;
   }

   BeginPath(dc);
   if (!applyDCPath(pd, pd->replayDC, dc)) {
      DeleteDC(dc);
      return NULL;
   }

   EndPath(dc);

   rgn = PathToRegion(dc);

   DeleteDC(dc);

   return rgn;
}

static void applyClipRegion(struct processData *pd, HRGN rgn2, int rgnMode)
{
   if (!pd->replayDC || !pd->painter) {
      return;
   }

   if (pd->currentDCState.isClipping) {
      if (pd->painter) {
         pd->painter->Restore();
      }

      pd->currentDCState.isClipping = false;
   }

   if (!pd->currentDCState.clipRgn) {
      if (rgn2 && (rgnMode == RGN_AND || rgnMode == RGN_COPY)) {
         rgnMode = RGN_COPY;
         pd->currentDCState.clipRgn = CreateRectRgn(0, 0, 1, 1);
      } else {
         bool switchY = (pd->currentDCState.iMapMode == MM_ANISOTROPIC || pd->currentDCState.iMapMode == MM_ISOTROPIC) && (pd->windowExt.cy < 0 || pd->viewportExt.cy < 0);
         int pxX, pxY, pxW = 0, pxH = 0;

         pxX = -(pd->viewportOrg.x - pd->windowOrg.x);
         pxY = -(pd->viewportOrg.y - pd->windowOrg.y);

         if (pd->currentDCState.iMapMode == MM_ANISOTROPIC || pd->currentDCState.iMapMode == MM_ISOTROPIC) {
            pxW = pd->windowExt.cx != 0 ? pd->windowExt.cx : pd->viewportExt.cx;
            pxH = pd->windowExt.cy != 0 ? pd->windowExt.cy : pd->viewportExt.cy;
         }

         if (!pxW || !pxH) {
            pxW = pd->pageSize_lu.cx;
            pxH = pd->pageSize_lu.cy;
         }

         pxW = pxW + pxX;
         pxH = pxH + pxY;

         pd->currentDCState.clipRgn = CreateRectRgn(pxX, pxY, pxW, pxH);
      }
   }

   if (rgn2) {
      if (rgnMode == RGN_COPY) {
         CombineRgn(pd->currentDCState.clipRgn, rgn2, rgn2, rgnMode);
      } else {
         CombineRgn(pd->currentDCState.clipRgn, pd->currentDCState.clipRgn, rgn2, rgnMode);
      }
   }

   DWORD bytes = GetRegionData(pd->currentDCState.clipRgn, 0, NULL);
   if (bytes > 0) {
      LPRGNDATA rgndata;

      rgndata = (LPRGNDATA) malloc(sizeof(BYTE) * bytes);
      if (rgndata) {
         memset(rgndata, 0, sizeof(BYTE) * bytes);
         if (GetRegionData(pd->currentDCState.clipRgn, bytes, rgndata) &&
             rgndata->rdh.nCount > 0) {
            const RECT *rects = (const RECT *) rgndata->Buffer;
            DWORD ii;

            pd->painter->EndPath(); //there should be none opened
            pd->currentDCState.isClipping = true;
            pd->painter->Save();

            for (ii = 0; ii < rgndata->rdh.nCount; ii++, rects++) {
               RECT r = *rects;
               double dx, dy, w, h;

               OffsetRect(&r, r.right >= r.left ? -1 : 1, r.bottom >= r.top ? -1 : 1);

               LogicalUnitToPdf(pd, r.left, r.top, dx, dy);
               LogicalUnitToPdf(pd, r.right, r.bottom, w, h);

               w -= dx;
               h -= dy;

               pd->painter->Rectangle(dx, dy, w, h);
            }

            pd->painter->Clip();
            pd->painter->EndPath();
         }

         free(rgndata);
      }
   }
}

static int CALLBACK processEnhMetaFileCB(HDC hDC, HANDLETABLE *lpHTable, const ENHMETARECORD *lpEMFR, int nObj, LPARAM lpData)
{
   struct processData *pd = (struct processData *) lpData;
   if (!pd) {
      return 0;
   }

   if (!pd->document) {
      if (pd->on_error) {
         pd->on_error(ERROR_INVALID_PARAMETER, "No document provided", pd->on_error_user_data);
      }
      return 0;
   }

   if (!pd->painter) {
      if (pd->on_error) {
         pd->on_error(ERROR_INVALID_PARAMETER, "No painter provided", pd->on_error_user_data);
      }
      return 0;
   }

   if (!lpEMFR) {
      if (pd->on_error) {
         pd->on_error(ERROR_INVALID_PARAMETER, "lpEMFR is NULL", pd->on_error_user_data);
      }
      return 0;
   }

   PdfPainter *painter = pd->painter;
   PdfDocument *document = pd->document;
   bool replayDCUpdated = false;

   writeToDebugLog(lpEMFR, nObj, NULL);

   switch (lpEMFR->iType) {
   case 0x1: {
      // const ENHMETAHEADER *dt = (const ENHMETAHEADER *) lpEMFR;
   } break;
   case EMR_ABORTPATH: {
      pd->buildingPath = false;
      painter->EndPath();
   } break;
   case EMR_ALPHABLEND: {
      const EMRALPHABLEND *dt = (const EMRALPHABLEND *) lpEMFR;
      BLENDFUNCTION blendFunc;

      memcpy(&blendFunc, &dt->dwRop, 4);

      if (dt->cbBmiSrc != 0 && dt->cbBitsSrc != 0 && dt->offBitsSrc != 0 && dt->offBmiSrc != 0) {
         painter->Save();
         maybePushWorldMatrix(pd);

         if (blendFunc.SourceConstantAlpha < 255) {
            PdfExtGState extGState(document);
            extGState.SetFillOpacity(blendFunc.SourceConstantAlpha / 255.0);

            painter->SetExtGState(&extGState);
         }

         if (blendFunc.AlphaFormat == AC_SRC_ALPHA) {
            BYTE *bitsAlpha = NULL;
            DWORD cbBitsAlpha = 0;

            PdfImage *imgSrc = prepareImage(pd, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, SRCCOPY, dt->iUsageSrc,
               (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc,
               true, false, &bitsAlpha, &cbBitsAlpha);

            if (imgSrc && bitsAlpha && cbBitsAlpha) {
               double dx, dy, dw, dh;

               PdfMemoryInputStream stream((const char *) bitsAlpha, cbBitsAlpha);
               PdfImage softMask(pd->document);

               softMask.SetImageColorSpace( ePdfColorSpace_DeviceGray );
               softMask.SetImageData(imgSrc->GetWidth(), imgSrc->GetHeight(), 8, &stream);

               imgSrc->SetImageSoftmask(&softMask);

               LogicalUnitToPdf(pd, dt->xDest, dt->yDest, dx, dy);
               LogicalUnitToPdf(pd, dt->xDest + dt->cxDest, dt->yDest + dt->cyDest, dw, dh);

               dw -= dx;
               dh -= dy;

               painter->DrawImage(dx, dy, imgSrc, dw / (double) dt->cxSrc, dh / (double) dt->cySrc);
            } else {
               if (pd->on_error) {
                  pd->on_error(ERROR_INVALID_PARAMETER, "Failed to convert image with alpha", pd->on_error_user_data);
               }
            }

            if (bitsAlpha) {
               free (bitsAlpha);
            }
            if (imgSrc) {
               delete imgSrc;
            }
         } else {
            drawImage(pd, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, SRCCOPY, dt->iUsageSrc,
               (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc, true);
         }

         maybePopWorldMatrix(pd);
         painter->Restore();
      }
   } break;
   case EMR_ANGLEARC: {
      const EMRANGLEARC *dt = (const EMRANGLEARC *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      POINT pos;
      if (!GetCurrentPositionEx (pd->replayDC, &pos)) {
         pos.x = 0;
         pos.y = 0;
      }

      HDC dc = CreateCompatibleDC(pd->replayDC);
      if (!dc) {
         if (pd->on_error) {
            pd->on_error (GetLastError(), "AngleArc: Failed to create temporary DC", pd->on_error_user_data);
         }
         break;
      }

      BeginPath(dc);
      MoveToEx(dc, pos.x, pos.y, NULL);
      AngleArc(dc, dt->ptlCenter.x, dt->ptlCenter.y, dt->nRadius, dt->eStartAngle, dt->eSweepAngle);
      EndPath(dc);

      applyDCPath(pd, dc);

      if (GetCurrentPositionEx (dc, &pos)) {
         LogicalUnitToPdf(pd, pos.x, pos.y, pd->current_posX, pd->current_posY);
      }

      DeleteDC(dc);

      drawPath(pd, true);
   } break;
   case EMR_ARC: {
      const EMRARC *dt = (const EMRARC *) lpEMFR;
      double boundLeft, boundTop, boundRight, boundBottom;
      double startX, startY, endX, endY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->rclBox.left, dt->rclBox.top, boundLeft, boundTop);
      LogicalUnitToPdf(pd, dt->rclBox.right, dt->rclBox.bottom, boundRight, boundBottom);
      LogicalUnitToPdf(pd, dt->ptlStart.x, dt->ptlStart.y, startX, startY);
      LogicalUnitToPdf(pd, dt->ptlEnd.x, dt->ptlEnd.y, endX, endY);

      drawArcPath(pd, boundLeft, boundTop, boundRight - boundLeft, boundBottom - boundTop, startX, startY, endX, endY,
         dt->rclBox.bottom < dt->rclBox.top ? pd->currentDCState.iArcDirection != AD_COUNTERCLOCKWISE : pd->currentDCState.iArcDirection == AD_COUNTERCLOCKWISE);
      drawPath(pd, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_ARCTO: {
      const EMRARCTO *dt = (const EMRARCTO *) lpEMFR;
      double boundLeft, boundTop, boundRight, boundBottom;
      double startX, startY, endX, endY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->rclBox.left, dt->rclBox.top, boundLeft, boundTop);
      LogicalUnitToPdf(pd, dt->rclBox.right, dt->rclBox.bottom, boundRight, boundBottom);
      LogicalUnitToPdf(pd, dt->ptlStart.x, dt->ptlStart.y, startX, startY);
      LogicalUnitToPdf(pd, dt->ptlEnd.x, dt->ptlEnd.y, endX, endY);

      drawArcPath(pd, boundLeft, boundTop, boundRight - boundLeft, boundBottom - boundTop, startX, startY, endX, endY,
         dt->rclBox.bottom < dt->rclBox.top ? pd->currentDCState.iArcDirection != AD_COUNTERCLOCKWISE : pd->currentDCState.iArcDirection == AD_COUNTERCLOCKWISE, true);
      drawPath(pd, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_BEGINPATH: {
      if (pd->buildingPath) {
         break;
      }
      pd->buildingPath = true;
      painter->EndPath();
   } break;
   case EMR_BITBLT: {
      const EMRBITBLT *dt = (const EMRBITBLT *) lpEMFR;
      double dx, dy, w, h;

      if (dt->cbBmiSrc == 0 && dt->cbBitsSrc == 0 && dt->offBitsSrc == 0 && dt->offBmiSrc == 0) {
         if (dt->dwRop == DSTINVERT) {
            // InvertRect
         }

         if (pd->currentDCState.brushSet && dt->cxDest != 0 && dt->cyDest != 0) {
            painter->Save();
            maybePushWorldMatrix(pd);

            LogicalUnitToPdf(pd, dt->xDest, dt->yDest, dx, dy);
            LogicalUnitToPdf(pd, dt->xDest + dt->cxDest, dt->yDest + dt->cyDest, w, h);

            w -= dx;
            h -= dy;

            if (selectBrush(pd, true)) {
               painter->Rectangle(dx, dy, w, h);
               painter->Fill();
            }

            maybePopWorldMatrix(pd);
            painter->Restore();
         }
      } else {
         painter->Save();
         maybePushWorldMatrix(pd);

         LogicalUnitToPdf(pd, dt->xDest, dt->yDest, dx, dy);
         LogicalUnitToPdf(pd, dt->xDest + dt->cxDest, dt->yDest + dt->cyDest, w, h);

         w -= dx;
         h -= dy;

         drawImage(pd, dt->xSrc, dt->ySrc, dt->cxDest, dt->cyDest, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->iUsageSrc,
            (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc, false);

         maybePopWorldMatrix(pd);
         painter->Restore();
      }
   } break;
   case EMR_CHORD: {
      const EMRCHORD *dt = (const EMRCHORD *) lpEMFR;
      double boundLeft, boundTop, boundRight, boundBottom;
      double startX, startY, endX, endY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->rclBox.left, dt->rclBox.top, boundLeft, boundTop);
      LogicalUnitToPdf(pd, dt->rclBox.right, dt->rclBox.bottom, boundRight, boundBottom);
      LogicalUnitToPdf(pd, dt->ptlStart.x, dt->ptlStart.y, startX, startY);
      LogicalUnitToPdf(pd, dt->ptlEnd.x, dt->ptlEnd.y, endX, endY);

      drawArcPath(pd, boundLeft, boundTop, boundRight - boundLeft, boundBottom - boundTop, startX, startY, endX, endY,
         dt->rclBox.bottom < dt->rclBox.top ? pd->currentDCState.iArcDirection != AD_COUNTERCLOCKWISE : pd->currentDCState.iArcDirection == AD_COUNTERCLOCKWISE);
      painter->ClosePath();

      drawPath(pd, false);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_CLOSEFIGURE: {
      if (pd->buildingPath) {
         break;
      }
      /*painter->LineTo(pd->current_posX, pd->current_posY);
      painter->ClosePath();*/
   } break;
   //case EMR_COLORCORRECTPALETTE: { } break;
   //case EMR_COLORMATCHTOTARGETW: { } break;
   case EMR_CREATEBRUSHINDIRECT: {
      const EMRCREATEBRUSHINDIRECT *dt = (const EMRCREATEBRUSHINDIRECT *) lpEMFR;
      MObjectData *objData = new MObjectData();
      if (!objData) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to allocate object data for Brush", pd->on_error_user_data);
         }
         break;
      }

      objData->type = OBJECT_DATA_TYPE_BRUSH;
      objData->bytes = malloc (lpEMFR->nSize);
      if (!objData->bytes) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to copy brush structure", pd->on_error_user_data);
         }
         break;
      }
      memcpy (objData->bytes, lpEMFR, lpEMFR->nSize);
      dt = (const EMRCREATEBRUSHINDIRECT *) objData->bytes;

      objData->v.brush = &dt->lb;
      pd->objects[dt->ihBrush] = objData;
   } break;
   //case EMR_CREATECOLORSPACE: { } break;
   //case EMR_CREATECOLORSPACEW: { } break;
   case EMR_CREATEDIBPATTERNBRUSHPT: {
      const EMRCREATEDIBPATTERNBRUSHPT *dt = (const EMRCREATEDIBPATTERNBRUSHPT *) lpEMFR;
      MObjectData *objData = new MObjectData();
      if (!objData) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to allocate object data for Brush", pd->on_error_user_data);
         }
         break;
      }

      objData->type = OBJECT_DATA_TYPE_BRUSH;
      objData->bytes = malloc (lpEMFR->nSize + dt->cbBmi + dt->cbBits);
      if (!objData->bytes) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to copy brush structure", pd->on_error_user_data);
         }
         break;
      }
      memcpy (objData->bytes, lpEMFR, lpEMFR->nSize);
      dt = (const EMRCREATEDIBPATTERNBRUSHPT *) objData->bytes;

      objData->v.brush = NULL;
      pd->objects[dt->ihBrush] = objData;
   } break;
   case EMR_CREATEMONOBRUSH: {
      const EMRCREATEMONOBRUSH *dt = (const EMRCREATEMONOBRUSH *) lpEMFR;
      MObjectData *objData = new MObjectData();
      if (!objData) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to allocate object data for Brush", pd->on_error_user_data);
         }
         break;
      }

      objData->type = OBJECT_DATA_TYPE_BRUSH;
      objData->bytes = malloc (lpEMFR->nSize + dt->cbBmi + dt->cbBits);
      if (!objData->bytes) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to copy brush structure", pd->on_error_user_data);
         }
         break;
      }
      memcpy (objData->bytes, lpEMFR, lpEMFR->nSize);
      dt = (const EMRCREATEMONOBRUSH *) objData->bytes;

      objData->v.brush = NULL;
      pd->objects[dt->ihBrush] = objData;
   } break;
   case EMR_CREATEPALETTE: {
      const EMRCREATEPALETTE *dt = (const EMRCREATEPALETTE *) lpEMFR;
      MObjectData *objData = new MObjectData();
      if (!objData) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to allocate object data for Palette", pd->on_error_user_data);
         }
         break;
      }

      objData->type = OBJECT_DATA_TYPE_PALETTE;
      pd->objects[dt->ihPal] = objData;
   } break;
   case EMR_CREATEPEN: {
      const EMRCREATEPEN *dt = (const EMRCREATEPEN *) lpEMFR;
      MObjectData *objData = new MObjectData();
      if (!objData) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to allocate object data for Pen", pd->on_error_user_data);
         }
         break;
      }

      objData->type = OBJECT_DATA_TYPE_PEN;
      objData->v.pen = dt->lopn;

      pd->objects[dt->ihPen] = objData;
   } break;
   //case EMR_DELETECOLORSPACE: { } break;
   case EMR_DELETEOBJECT: {
      const EMRDELETEOBJECT *dt = (const EMRDELETEOBJECT *) lpEMFR;
      map<DWORD, MObjectData *>::iterator it;

      it = pd->objects.find (dt->ihObject);
      if (it != pd->objects.end()) {
         MObjectData *obj = it->second;
         if (obj) {
            switch(obj->type) {
            case OBJECT_DATA_TYPE_PEN:
               break;
            case OBJECT_DATA_TYPE_EXTPEN:
               if (pd->currentDCState.setExtPen == obj->v.extPen) {
                  pd->currentDCState.setExtPen = NULL;
               }
               break;
            case OBJECT_DATA_TYPE_BRUSH:
               if (pd->currentDCState.setBrush == obj->v.brush) {
                  pd->currentDCState.setBrush = NULL;
               }
               break;
            case OBJECT_DATA_TYPE_FONT:
               break;
            case OBJECT_DATA_TYPE_PALETTE:
               break;
            }

            delete obj;
            it->second = NULL;
         }
         pd->objects.erase (it);
      } else if (pd->on_error) {
         char buff[128];
         sprintf (buff, "Object %u to delete does not exist", dt->ihObject);
         pd->on_error (ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
   } break;
   case EMR_ELLIPSE: {
      const EMRELLIPSE *dt = (const EMRELLIPSE *) lpEMFR;
      double dx, dy, w, h;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->rclBox.left, dt->rclBox.top, dx, dy);
      LogicalUnitToPdf(pd, dt->rclBox.right, dt->rclBox.bottom, w, h);

      w -= dx;
      h -= dy;

      painter->Ellipse(dx, dy, w, h);
      drawPath(pd, false);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_ENDPATH: {
      pd->buildingPath = false;
   } break;
   case EMR_EOF: {
      /* just ignore, nothing to be done here */
   } break;
   case EMR_EXCLUDECLIPRECT: {
      const EMREXCLUDECLIPRECT *dt = (const EMREXCLUDECLIPRECT *) lpEMFR;

      HRGN rgn2 = CreateRectRgn(dt->rclClip.left, dt->rclClip.top, dt->rclClip.right, dt->rclClip.bottom);

      applyClipRegion(pd, rgn2, RGN_DIFF);

      DeleteObject(rgn2);
   } break;
   case EMR_EXTCREATEFONTINDIRECTW: {
      const EMREXTCREATEFONTINDIRECTW *dt = (const EMREXTCREATEFONTINDIRECTW *) lpEMFR;
      MObjectData *objData = new MObjectData();
      if (!objData) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to allocate object data for Font", pd->on_error_user_data);
         }
         break;
      }

      objData->type = OBJECT_DATA_TYPE_FONT;
      objData->v.font.lfHeight = dt->elfw.elfLogFont.lfHeight;
      objData->v.font.lfWidth = dt->elfw.elfLogFont.lfWidth;
      objData->v.font.lfEscapement = dt->elfw.elfLogFont.lfEscapement;
      objData->v.font.lfOrientation = dt->elfw.elfLogFont.lfOrientation;
      objData->v.font.lfWeight = dt->elfw.elfLogFont.lfWeight;
      objData->v.font.lfItalic = dt->elfw.elfLogFont.lfItalic;
      objData->v.font.lfUnderline = dt->elfw.elfLogFont.lfUnderline;
      objData->v.font.lfStrikeOut = dt->elfw.elfLogFont.lfStrikeOut;
      objData->v.font.lfCharSet = dt->elfw.elfLogFont.lfCharSet;
      objData->v.font.lfOutPrecision = dt->elfw.elfLogFont.lfOutPrecision;
      objData->v.font.lfClipPrecision = dt->elfw.elfLogFont.lfClipPrecision;
      objData->v.font.lfQuality = dt->elfw.elfLogFont.lfQuality;
      objData->v.font.lfPitchAndFamily = dt->elfw.elfLogFont.lfPitchAndFamily;
      objData->v.font.lfHeight = dt->elfw.elfLogFont.lfHeight;
      objData->v.font.lfFaceName[0] = 0;

      DWORD blen = sizeof(objData->v.font.lfFaceName);
      if (!UnicodeToAnsi((LPWSTR) dt->elfw.elfLogFont.lfFaceName, -1, objData->v.font.lfFaceName, &blen)) {
         if (pd->on_error) {
            pd->on_error(ERROR_INVALID_PARAMETER, "Failed to convert font name to ANSI", pd->on_error_user_data);
         }
      }

      if (objData->v.font.lfCharSet == DEFAULT_CHARSET) {
         if (stricmp (objData->v.font.lfFaceName, "Wingdings") == 0 ||
             stricmp (objData->v.font.lfFaceName, "Webdings") == 0) {
            objData->v.font.lfCharSet = SYMBOL_CHARSET;
         }
      } else if (objData->v.font.lfCharSet != SYMBOL_CHARSET) {
         if (stricmp (objData->v.font.lfFaceName, "Wingdings") == 0 ||
             stricmp (objData->v.font.lfFaceName, "Webdings") == 0) {
            strcpy (objData->v.font.lfFaceName, "Arial");
         }
      }

      pd->objects[dt->ihFont] = objData;
   } break;
   case EMR_EXTCREATEPEN: {
      const EMREXTCREATEPEN *dt = (const EMREXTCREATEPEN *) lpEMFR;
      MObjectData *objData = new MObjectData();
      if (!objData) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to allocate object data for ExtPen", pd->on_error_user_data);
         }
         break;
      }

      objData->type = OBJECT_DATA_TYPE_EXTPEN;
      objData->bytes = malloc (lpEMFR->nSize);
      if (!objData->bytes) {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to copy ExtPen structure", pd->on_error_user_data);
         }
         break;
      }

      memcpy (objData->bytes, lpEMFR, lpEMFR->nSize);
      dt = (const EMREXTCREATEPEN *) objData->bytes;

      objData->v.extPen = (EXTLOGPEN32 *)(&dt->elp);

      pd->objects[dt->ihPen] = objData;
   } break;
   //case EMR_EXTFLOODFILL: { } break;
   case EMR_EXTSELECTCLIPRGN: {
      const EMREXTSELECTCLIPRGN *dt = (const EMREXTSELECTCLIPRGN *) lpEMFR;

      if (dt->cbRgnData == 0) {
         if (pd->currentDCState.isClipping) {
            painter->Restore();
            pd->currentDCState.isClipping = false;
         }
         if (pd->currentDCState.clipRgn) {
            DeleteObject(pd->currentDCState.clipRgn);
         }
         pd->currentDCState.clipRgn = NULL;
         if (!pd->savedStates.empty()) {
            HRGN lastClipRgn = pd->savedStates.rbegin()->clipRgn;
            if (lastClipRgn) {
               pd->currentDCState.clipRgn = CreateRectRgn(0, 0, 1, 1);
               CombineRgn(pd->currentDCState.clipRgn, lastClipRgn, lastClipRgn, RGN_COPY);
            }
         }
      } else {
         const RGNDATA *rgndata = (const RGNDATA *) dt->RgnData;
         const RECT *rects = (const RECT *) rgndata->Buffer;
         DWORD ii;
         HRGN rgn2 = NULL;

         for (ii = 0; ii < rgndata->rdh.nCount; ii++, rects++) {
            const RECT &r = *rects;

            if (!rgn2) {
               rgn2 = CreateRectRgn(r.left, r.top, r.right, r.bottom);
            } else {
               HRGN rrgn = CreateRectRgn(r.left, r.top, r.right, r.bottom);
               CombineRgn(rgn2, rgn2, rrgn, RGN_OR);
            }
         }

         if (rgn2) {
            applyClipRegion(pd, rgn2, dt->iMode);

            DeleteObject(rgn2);
         }
      }
   } break;
   case EMR_EXTTEXTOUTA:
   case EMR_EXTTEXTOUTW: {
      const EMREXTTEXTOUTW *dt = (const EMREXTTEXTOUTW *) lpEMFR;

      drawText(pd, lpEMFR, dt->emrtext, (pd->currentDCState.iMapMode == MM_ANISOTROPIC || pd->currentDCState.iMapMode == MM_ISOTROPIC) && dt->iGraphicsMode == GM_COMPATIBLE && dt->eyScale < 0.0);
   } break;
   case EMR_FILLPATH: {
      const EMRFILLPATH *dt = (const EMRFILLPATH *) lpEMFR;

      maybePushWorldMatrix(pd);
      if (applyDCPath(pd, pd->replayDC)) {
         painter->Save();

         if (selectBrush(pd)) {
            painter->Fill(pd->currentDCState.iPolyFillMode == ALTERNATE);
         }

         painter->Restore();
      }
      maybePopWorldMatrix(pd);
   } break;
   case EMR_FILLRGN: {
      const EMRFILLRGN *dt = (const EMRFILLRGN *) lpEMFR;
      const RGNDATA *rgndata = (const RGNDATA *) dt->RgnData;
      const RECT *rects = (const RECT *) rgndata->Buffer;
      DWORD ii;
      bool canFill = false;

      storeState(pd);

      if (pd->objects.find (dt->ihBrush) != pd->objects.end()) {
         if (pd->objects[dt->ihBrush]->type == OBJECT_DATA_TYPE_BRUSH) {
            pd->currentDCState.brushSet = true;
            pd->currentDCState.setBrush = pd->objects[dt->ihBrush]->v.brush;
            pd->currentDCState.setBrushObjectIndex = dt->ihBrush;
            pd->currentDCState.setBrushName = &pd->objects[dt->ihBrush]->brushName;
            canFill = selectBrush(pd);
         } else if (pd->on_error) {
            pd->on_error(ERROR_INVALID_PARAMETER, "FillRgn:Specified object is not brush", pd->on_error_user_data);
         }
      } else if (pd->on_error) {
         pd->on_error(ERROR_INVALID_PARAMETER, "FillRgn: Cannot find specified brush object", pd->on_error_user_data);
      }

      maybePushWorldMatrix(pd);

      for (ii = 0; canFill && ii < rgndata->rdh.nCount; ii++, rects++) {
         const RECT &r = *rects;
         double dx, dy, w, h;

         LogicalUnitToPdf(pd, r.left, r.top, dx, dy);
         LogicalUnitToPdf(pd, r.right, r.bottom, w, h);

         w -= dx;
         h -= dy;

         painter->Rectangle(dx, dy, w, h);
      }

      if (canFill) {
         painter->Fill();
      }

      maybePopWorldMatrix(pd);

      restoreState(pd);
   } break;
   case EMR_FLATTENPATH: {
      // const EMRFLATTENPATH *dt = (const EMRFLATTENPATH *) lpEMFR;
      // applied to pd->replayDC only
   } break;
   case EMR_FRAMERGN: {
      const EMRFRAMERGN *dt = (const EMRFRAMERGN *) lpEMFR;
      const RGNDATA *rgndata = (const RGNDATA *) dt->RgnData;
      const RECT *rects = (const RECT *) rgndata->Buffer;
      DWORD ii;
      bool canFill = false;

      storeState(pd);
      maybePushWorldMatrix(pd);

      if (pd->objects.find (dt->ihBrush) != pd->objects.end()) {
         if (pd->objects[dt->ihBrush]->type == OBJECT_DATA_TYPE_BRUSH) {
            pd->currentDCState.brushSet = true;
            pd->currentDCState.setBrush = pd->objects[dt->ihBrush]->v.brush;
            pd->currentDCState.setBrushObjectIndex = dt->ihBrush;
            pd->currentDCState.setBrushName = &pd->objects[dt->ihBrush]->brushName;
            canFill = selectBrush(pd);
         } else if (pd->on_error) {
            pd->on_error(ERROR_INVALID_PARAMETER, "FrameRgn:Specified object is not brush", pd->on_error_user_data);
         }
      } else if (pd->on_error) {
         pd->on_error(ERROR_INVALID_PARAMETER, "FrameRgn: Cannot find specified brush object", pd->on_error_user_data);
      }

      map<int, vector<const RECT *> > rleft, rtop, rright, rbottom;

      for (ii = 0; canFill && ii < rgndata->rdh.nCount; ii++, rects++) {
         rleft[rects->left].push_back(rects);
         rtop[rects->top].push_back(rects);
         rright[rects->right].push_back(rects);
         rbottom[rects->bottom].push_back(rects);
      }

      sortRegionRects(rleft, sortRECTbyTop);
      sortRegionRects(rtop, sortRECTbyLeft);
      sortRegionRects(rright, sortRECTbyTop);
      sortRegionRects(rbottom, sortRECTbyLeft);

      rects = (const RECT *) rgndata->Buffer;
      for (ii = 0; canFill && ii < rgndata->rdh.nCount; ii++, rects++) {
         vector<RECT> borders;

         borders = evalRegionBorders(rects, dt->szlStroke, rleft, rtop, rright, rbottom);

         vector<RECT>::const_iterator it, end = borders.end();
         for (it = borders.begin(); it != end; it++) {
            double dx, dy, w, h;

            LogicalUnitToPdf(pd, it->left, it->top, dx, dy);
            LogicalUnitToPdf(pd, it->right, it->bottom, w, h);

            w -= dx;
            h -= dy;

            painter->Rectangle(dx, dy, w, h);
         }
      }

      if (canFill && ii != 0) {
         painter->Fill();
      }

      maybePopWorldMatrix(pd);
      restoreState(pd);
   } break;
   case EMR_GDICOMMENT: {
      /* just ignore, nothing to be done here */
   } break;
   //case EMR_GLSBOUNDEDRECORD: { } break;
   //case EMR_GLSRECORD: { } break;
   case EMR_GRADIENTFILL: {
      const EMRGRADIENTFILL *dt = (const EMRGRADIENTFILL *) lpEMFR;
      if (dt->ulMode == GRADIENT_FILL_RECT_H ||
          dt->ulMode == GRADIENT_FILL_RECT_V ||
          dt->ulMode == GRADIENT_FILL_TRIANGLE) {
         DWORD ii;

         if (dt->ulMode == GRADIENT_FILL_RECT_H ||
             dt->ulMode == GRADIENT_FILL_RECT_V) {
            const GRADIENT_RECT *rect = (const GRADIENT_RECT *) STRUCT_OFFSET (dt->Ver, sizeof(TRIVERTEX) * dt->nVer);
            for (ii = 0; ii < dt->nTri; ii++, rect++) {
               if (rect->UpperLeft < dt->nVer && rect->LowerRight < dt->nVer) {
                  TRIVERTEX ul = dt->Ver[rect->UpperLeft], lr = dt->Ver[rect->LowerRight];
                  double dx, dy, w, h, rf, gf, bf, rt, gt, bt;

                  LogicalUnitToPdf(pd, ul.x, ul.y, dx, dy);
                  LogicalUnitToPdf(pd, lr.x, lr.y, w, h);

                  w -= dx;
                  h -= dy;

                  rf = ul.Red / ((double) 0xFF00); if (rf > 1.0) rf = 1.0;
                  gf = ul.Green / ((double) 0xFF00); if (gf > 1.0) gf = 1.0;
                  bf = ul.Blue / ((double) 0xFF00); if (bf > 1.0) bf = 1.0;

                  rt = lr.Red / ((double) 0xFF00); if (rt > 1.0) rt = 1.0;
                  gt = lr.Green / ((double) 0xFF00); if (gt > 1.0) gt = 1.0;
                  bt = lr.Blue / ((double) 0xFF00); if (bt > 1.0) bt = 1.0;

                  PdfAxialShadingPattern pattern(dx, dy, dt->ulMode == GRADIENT_FILL_RECT_H ? dx + w : dx, dt->ulMode == GRADIENT_FILL_RECT_V ? dy + h : dy,
                     PdfColor(rf, gf, bf), PdfColor(rt, gt, bt), document);

                  painter->Save();
                  painter->SetShadingPattern(pattern);
                  painter->Rectangle(dx, dy, w, h);
                  painter->Fill();
                  painter->Restore();
               }
            }
         } else {
            const GRADIENT_TRIANGLE *trig = (const GRADIENT_TRIANGLE *) STRUCT_OFFSET (dt->Ver, sizeof(TRIVERTEX) * dt->nVer);
            for (ii = 0; ii < dt->nTri; ii++, trig++) {
               if (trig->Vertex1 < dt->nVer && trig->Vertex2 < dt->nVer && trig->Vertex3 < dt->nVer) {
                  TRIVERTEX v1 = dt->Ver[trig->Vertex1], v2 = dt->Ver[trig->Vertex2], v3 = dt->Ver[trig->Vertex3];
                  double dx1, dy1, dx2, dy2, dx3, dy3, r1, g1, b1, r2, g2, b2, r3, g3, b3;

                  LogicalUnitToPdf(pd, v1.x, v1.y, dx1, dy1);
                  LogicalUnitToPdf(pd, v2.x, v2.y, dx2, dy2);
                  LogicalUnitToPdf(pd, v3.x, v3.y, dx3, dy3);

                  r1 = v1.Red / ((double) 0xFF00); if (r1 > 1.0) r1 = 1.0;
                  g1 = v1.Green / ((double) 0xFF00); if (g1 > 1.0) g1 = 1.0;
                  b1 = v1.Blue / ((double) 0xFF00); if (b1 > 1.0) b1 = 1.0;

                  r2 = v2.Red / ((double) 0xFF00); if (r2 > 1.0) r2 = 1.0;
                  g2 = v2.Green / ((double) 0xFF00); if (g2 > 1.0) g2 = 1.0;
                  b2 = v2.Blue / ((double) 0xFF00); if (b2 > 1.0) b2 = 1.0;

                  r3 = v3.Red / ((double) 0xFF00); if (r3 > 1.0) r3 = 1.0;
                  g3 = v3.Green / ((double) 0xFF00); if (g3 > 1.0) g3 = 1.0;
                  b3 = v3.Blue / ((double) 0xFF00); if (b3 > 1.0) b3 = 1.0;

                  PdfTriangleShadingPattern pattern(
                     dx1, dy1, PdfColor(r1, g1, b1),
                     dx2, dy2, PdfColor(r2, g2, b2),
                     dx3, dy3, PdfColor(r3, g3, b3), document);

                  painter->Save();
                  painter->SetShadingPattern(pattern);
                  painter->MoveTo(dx1, dy1);
                  painter->LineTo(dx2, dy2);
                  painter->LineTo(dx3, dy3);
                  painter->LineTo(dx1, dy1);
                  painter->ClosePath();
                  painter->Fill();
                  painter->Restore();
               }
            }
         }
      } else if (pd->on_error) {
         char buff[128];
         sprintf (buff, "Unknown GradientFill mode 0x%x", dt->ulMode);
         pd->on_error (ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
   } break;
   case EMR_INTERSECTCLIPRECT: {
      const EMRINTERSECTCLIPRECT *dt = (const EMRINTERSECTCLIPRECT *) lpEMFR;

      HRGN rgn2 = CreateRectRgn(dt->rclClip.left, dt->rclClip.top, dt->rclClip.right, dt->rclClip.bottom);

      applyClipRegion(pd, rgn2, RGN_AND);

      DeleteObject(rgn2);
   } break;
   /*case EMR_INVERTRGN: {
      const EMRINVERTRGN *dt = (const EMRINVERTRGN *) lpEMFR;
      const RGNDATA *rgndata = (const RGNDATA *) dt->RgnData;
      const RECT *rects = (const RECT *) rgndata->Buffer;
      DWORD ii;

      storeState(pd);

      maybePushWorldMatrix(pd);

      PdfExtGState extGState(document);
      extGState.SetBlendMode(ePdfBlendMode_ColorDodge);

      painter->SetExtGState(&extGState);
      painter->SetColor(1.0, 1.0, 1.0);

      for (ii = 0; ii < rgndata->rdh.nCount; ii++, rects++) {
         const RECT &r = *rects;
         double dx, dy, w, h;

         LogicalUnitToPdf(pd, r.left, r.top, dx, dy);
         LogicalUnitToPdf(pd, r.right, r.bottom, w, h);

         w -= dx;
         h -= dy;

         painter->Rectangle(dx, dy, w, h);
      }

      painter->Fill();

      maybePopWorldMatrix(pd);

      restoreState(pd);
   } break;*/
   case EMR_LINETO: {
      const EMRLINETO *dt = (const EMRLINETO *) lpEMFR;
      double pdfX, pdfY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->ptl.x, dt->ptl.y, pdfX, pdfY);
      if (pd->currentDCState.penSet || pd->currentDCState.extPenSet) {
         bool canStroke = false;

         painter->Save();
         canStroke = selectPen(pd) || selectExtPen(pd);
         painter->MoveTo(pd->current_posX, pd->current_posY);

         painter->LineTo(pdfX, pdfY);
         if (canStroke) {
            drawPath (pd, true);
            painter->MoveTo(pdfX, pdfY);
         }
         painter->Restore();
      } else {
         painter->MoveTo(pdfX, pdfY);
      }

      maybePopWorldMatrix(pd);

      pd->current_posX = pdfX;
      pd->current_posY = pdfY;
   } break;
   case EMR_MASKBLT: {
      const EMRMASKBLT *dt = (const EMRMASKBLT *) lpEMFR;

      if (dt->cbBmiSrc != 0 && dt->cbBitsSrc != 0 && dt->offBitsSrc != 0 && dt->offBmiSrc != 0 &&
          dt->cbBmiMask != 0 && dt->cbBitsMask != 0 && dt->offBitsMask != 0 && dt->offBmiMask != 0) {
         drawMaskedImage(pd,
            dt->xMask, dt->yMask, dt->cxDest, dt->cyDest, dt->iUsageMask,
            (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiMask), (BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsMask), dt->cbBitsMask,
            dt->xSrc, dt->ySrc, dt->cxDest, dt->cyDest, dt->iUsageSrc,
            (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc,
            dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop & 0xFFFFFF);
      }
   } break;
   case EMR_MODIFYWORLDTRANSFORM: {
      const EMRMODIFYWORLDTRANSFORM *dt = (const EMRMODIFYWORLDTRANSFORM *) lpEMFR;
      DWorldMatrix dMatrix;

      dMatrix.eM11 = dt->xform.eM11;
      dMatrix.eM12 = dt->xform.eM12;
      dMatrix.eM21 = dt->xform.eM21;
      dMatrix.eM22 = dt->xform.eM22;
      dMatrix.eDx = dt->xform.eDx;
      dMatrix.eDy = dt->xform.eDy;

      if (!pd->currentDCState.worldMatrixValid && dt->iMode != MWT_IDENTITY) {
         pd->currentDCState.worldMatrix.eM11 = 1.0;
         pd->currentDCState.worldMatrix.eM12 = 0.0;
         pd->currentDCState.worldMatrix.eM21 = 0.0;
         pd->currentDCState.worldMatrix.eM22 = 1.0;
         pd->currentDCState.worldMatrix.eDx = 0.0;
         pd->currentDCState.worldMatrix.eDy = 0.0;
      }

      switch (dt->iMode) {
      case MWT_IDENTITY:
         pd->currentDCState.worldMatrixValid = false;
         break;
      case MWT_LEFTMULTIPLY:
         multiplyMatrixes (pd->currentDCState.worldMatrix, dMatrix, pd->currentDCState.worldMatrix);
         pd->currentDCState.worldMatrixValid = true;
         break;
      case MWT_RIGHTMULTIPLY:
         multiplyMatrixes (pd->currentDCState.worldMatrix, pd->currentDCState.worldMatrix, dMatrix);
         pd->currentDCState.worldMatrixValid = true;
         break;
      }

      if (pd->currentDCState.worldMatrixValid &&
          double_equal (pd->currentDCState.worldMatrix.eM11, 1.0) && double_equal (pd->currentDCState.worldMatrix.eM12, 0.0) &&
          double_equal (pd->currentDCState.worldMatrix.eM21, 0.0) && double_equal (pd->currentDCState.worldMatrix.eM22, 1.0) &&
          double_equal (pd->currentDCState.worldMatrix.eDx,  0.0) && double_equal (pd->currentDCState.worldMatrix.eDy,  0.0)) {
         pd->currentDCState.worldMatrixValid = false;
      }
   } break;
   case EMR_MOVETOEX: {
      const EMRMOVETOEX *dt = (const EMRMOVETOEX *) lpEMFR;
      double pdfX, pdfY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->ptl.x, dt->ptl.y, pdfX, pdfY);
      painter->MoveTo(pdfX, pdfY);

      maybePopWorldMatrix(pd);

      pd->current_posX = pdfX;
      pd->current_posY = pdfY;
   } break;
   case EMR_OFFSETCLIPRGN: {
      const EMROFFSETCLIPRGN *dt = (const EMROFFSETCLIPRGN *) lpEMFR;

      if (pd->currentDCState.clipRgn) {
         HRGN rgn2 = CreateRectRgn(0, 0, 1, 1);

         CombineRgn(rgn2, pd->currentDCState.clipRgn, pd->currentDCState.clipRgn, RGN_COPY);
         OffsetRgn(rgn2, dt->ptlOffset.x, dt->ptlOffset.y);
         applyClipRegion(pd, rgn2, RGN_COPY);

         DeleteObject(rgn2);
      }
   } break;
   case EMR_PAINTRGN: {
      const EMRPAINTRGN *dt = (const EMRPAINTRGN *) lpEMFR;
      const RGNDATA *rgndata = (const RGNDATA *) dt->RgnData;
      const RECT *rects = (const RECT *) rgndata->Buffer;
      DWORD ii;

      storeState(pd);

      maybePushWorldMatrix(pd);

      for (ii = 0; ii < rgndata->rdh.nCount; ii++, rects++) {
         const RECT &r = *rects;
         double dx, dy, w, h;

         LogicalUnitToPdf(pd, r.left, r.top, dx, dy);
         LogicalUnitToPdf(pd, r.right, r.bottom, w, h);

         w -= dx;
         h -= dy;

         painter->Rectangle(dx, dy, w, h);
      }

      if (rgndata->rdh.nCount > 0) {
         if (selectBrush(pd)) {
            painter->Fill();
         }
      }

      maybePopWorldMatrix(pd);

      restoreState(pd);
   } break;
   case EMR_PIE: {
      const EMRPIE *dt = (const EMRPIE *) lpEMFR;
      double boundLeft, boundTop, boundRight, boundBottom;
      double startX, startY, endX, endY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->rclBox.left, dt->rclBox.top, boundLeft, boundTop);
      LogicalUnitToPdf(pd, dt->rclBox.right, dt->rclBox.bottom, boundRight, boundBottom);
      LogicalUnitToPdf(pd, dt->ptlStart.x, dt->ptlStart.y, startX, startY);
      LogicalUnitToPdf(pd, dt->ptlEnd.x, dt->ptlEnd.y, endX, endY);

      drawArcPath(pd, boundLeft, boundTop, boundRight - boundLeft, boundBottom - boundTop, startX, startY, endX, endY,
         dt->rclBox.bottom < dt->rclBox.top ? pd->currentDCState.iArcDirection != AD_COUNTERCLOCKWISE : pd->currentDCState.iArcDirection == AD_COUNTERCLOCKWISE);
      painter->LineTo(boundLeft + (boundRight - boundLeft) / 2, boundTop + (boundBottom - boundTop) / 2);
      painter->ClosePath();

      drawPath(pd, false);

      maybePopWorldMatrix(pd);
   } break;
   //case EMR_PIXELFORMAT: { } break;
   case EMR_PLGBLT: {
      const EMRPLGBLT *dt = (const EMRPLGBLT *) lpEMFR;

      if (dt->cbBmiSrc != 0 && dt->cbBitsSrc != 0 && dt->offBitsSrc != 0 && dt->offBmiSrc != 0 &&
          dt->cbBmiMask != 0 && dt->cbBitsMask != 0 && dt->offBitsMask != 0 && dt->offBmiMask != 0) {

         double ltx, lty, rtx, rty, lbx, lby;
         LONG cxDest, cyDest, xDelta, yDelta;

         cxDest = fabs((double)(dt->aptlDest[1].x - dt->aptlDest[0].x));
         yDelta = fabs((double)(dt->aptlDest[1].y - dt->aptlDest[0].y));
         xDelta = fabs((double)(dt->aptlDest[2].x - dt->aptlDest[0].x));
         cyDest = fabs((double)(dt->aptlDest[2].y - dt->aptlDest[0].y));

         const BITMAPINFO *bmiSrc = (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc);
         if (cyDest != 0 && yDelta != 0 && bmiSrc && bmiSrc->bmiHeader.biWidth != 0 && bmiSrc->bmiHeader.biHeight != 0) {
            painter->Save();

            LogicalUnitToPdf(pd, dt->aptlDest[0].x, dt->aptlDest[0].y, ltx, lty);
            LogicalUnitToPdf(pd, dt->aptlDest[1].x, dt->aptlDest[1].y, rtx, rty);
            LogicalUnitToPdf(pd, dt->aptlDest[2].x, dt->aptlDest[2].y, lbx, lby);

            double sval, cval, scaleX, scaleY;
            sval = -sin(atan((double) cxDest / (double) yDelta));
            cval = cos(atan((double) xDelta / (double) cyDest));
            scaleX = (double) cxDest / (double) bmiSrc->bmiHeader.biWidth;
            scaleY = (double) cyDest / (double) (bmiSrc->bmiHeader.biHeight * (bmiSrc->bmiHeader.biHeight < 0 ? -1 : +1));

            painter->SetTransformationMatrix(cval * scaleX, sval / scaleY, sval / scaleX, cval * scaleY, ltx, lty);

            drawMaskedImage(pd,
               dt->xMask, dt->yMask, cxDest, cyDest, dt->iUsageMask,
               (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiMask), (BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsMask), dt->cbBitsMask,
               dt->xSrc, dt->ySrc, cxDest, cyDest, dt->iUsageSrc,
               (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc,
               dt->aptlDest[0].x, dt->aptlDest[0].y, cxDest, cyDest, SRCCOPY, true);

            painter->Restore();

            /*{
               double dx, dy;

               painter->Save();
               LogicalUnitToPdf(pd, dt->aptlDest[0].x, dt->aptlDest[0].y, dx, dy);
               painter->MoveTo(dx, dy);

               LogicalUnitToPdf(pd, dt->aptlDest[1].x, dt->aptlDest[1].y, dx, dy);
               painter->LineTo(dx, dy);

               LogicalUnitToPdf(pd, dt->aptlDest[1].x + xDelta, dt->aptlDest[2].y + yDelta, dx, dy);
               painter->LineTo(dx, dy);

               LogicalUnitToPdf(pd, dt->aptlDest[2].x, dt->aptlDest[2].y, dx, dy);
               painter->LineTo(dx, dy);

               painter->ClosePath();

               painter->SetStrokeWidth(0.1);
               painter->SetStrokingColor(1.0, 0.0, 0.0);
               painter->Stroke();
               painter->Restore();
            }*/
         }
      }
   } break;
   case EMR_POLYBEZIER: {
      const EMRPOLYBEZIER *dt = (const EMRPOLYBEZIER *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      if (dt->cptl > 0) {
         double x1, y1, x2, y2, x3, y3;
         DWORD ii;

         maybePushWorldMatrix(pd);

         LogicalUnitToPdf(pd, dt->aptl[0].x, dt->aptl[0].y, x1, y1);

         for (ii = 1; ii + 2 < dt->cptl; ii += 3) {
            painter->MoveTo(x1, y1);

            LogicalUnitToPdf(pd, dt->aptl[ii].x, dt->aptl[ii].y, x1, y1);
            LogicalUnitToPdf(pd, dt->aptl[ii + 1].x, dt->aptl[ii + 1].y, x2, y2);
            LogicalUnitToPdf(pd, dt->aptl[ii + 2].x, dt->aptl[ii + 2].y, x3, y3);

            painter->CubicBezierTo(x1, y1, x2, y2, x3, y3);

            x1 = x3;
            y1 = y3;
         }

         drawPath(pd, true);

         maybePopWorldMatrix(pd);
      }
   } break;
   case EMR_POLYBEZIER16: {
      const EMRPOLYBEZIER16 *dt = (const EMRPOLYBEZIER16 *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      if (dt->cpts > 0) {
         double x1, y1, x2, y2, x3, y3;
         DWORD ii;

         maybePushWorldMatrix(pd);

         LogicalUnitToPdf(pd, dt->apts[0].x, dt->apts[0].y, x1, y1);

         for (ii = 1; ii + 2 < dt->cpts; ii += 3) {
            painter->MoveTo(x1, y1);

            LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, x1, y1);
            LogicalUnitToPdf(pd, dt->apts[ii + 1].x, dt->apts[ii + 1].y, x2, y2);
            LogicalUnitToPdf(pd, dt->apts[ii + 2].x, dt->apts[ii + 2].y, x3, y3);

            painter->CubicBezierTo(x1, y1, x2, y2, x3, y3);

            x1 = x3;
            y1 = y3;
         }

         drawPath(pd, true);

         maybePopWorldMatrix(pd);
      }
   } break;
   case EMR_POLYBEZIERTO: {
      const EMRPOLYBEZIERTO *dt = (const EMRPOLYBEZIERTO *) lpEMFR;
      double x1, y1, x2, y2, x3, y3;
      DWORD ii;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      x1 = pd->current_posX;
      y1 = pd->current_posY;

      for (ii = 0; ii + 2 < dt->cptl; ii += 3) {
         painter->MoveTo(x1, y1);

         LogicalUnitToPdf(pd, dt->aptl[ii].x, dt->aptl[ii].y, x1, y1);
         LogicalUnitToPdf(pd, dt->aptl[ii + 1].x, dt->aptl[ii + 1].y, x2, y2);
         LogicalUnitToPdf(pd, dt->aptl[ii + 2].x, dt->aptl[ii + 2].y, x3, y3);

         painter->CubicBezierTo(x1, y1, x2, y2, x3, y3);

         x1 = x3;
         y1 = y3;
         pd->current_posX = x3;
         pd->current_posY = y3;
      }

      drawPath(pd, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYBEZIERTO16: {
      const EMRPOLYBEZIERTO16 *dt = (const EMRPOLYBEZIERTO16 *) lpEMFR;
      double x1, y1, x2, y2, x3, y3;
      DWORD ii;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      x1 = pd->current_posX;
      y1 = pd->current_posY;

      for (ii = 0; ii + 2 < dt->cpts; ii += 3) {
         painter->MoveTo(x1, y1);

         LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, x1, y1);
         LogicalUnitToPdf(pd, dt->apts[ii + 1].x, dt->apts[ii + 1].y, x2, y2);
         LogicalUnitToPdf(pd, dt->apts[ii + 2].x, dt->apts[ii + 2].y, x3, y3);

         painter->CubicBezierTo(x1, y1, x2, y2, x3, y3);

         x1 = x3;
         y1 = y3;
         pd->current_posX = x3;
         pd->current_posY = y3;
      }

      drawPath(pd, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYDRAW: {
      const EMRPOLYDRAW *dt = (const EMRPOLYDRAW *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      double px, py;
      DWORD ii;
      const BYTE *types = (const BYTE *) STRUCT_OFFSET(dt->aptl, dt->cptl * sizeof(dt->aptl[0]));

      for (ii = 0; ii < dt->cptl; ii++) {
         switch(types[ii] & (~PT_CLOSEFIGURE)) {
         case PT_MOVETO:
            LogicalUnitToPdf(pd, dt->aptl[ii].x, dt->aptl[ii].y, px, py);
            painter->MoveTo(px, py);
            break;
         case PT_LINETO:
            LogicalUnitToPdf(pd, dt->aptl[ii].x, dt->aptl[ii].y, px, py);
            painter->LineTo(px, py);
            break;
         case PT_BEZIERTO:
            if (ii + 3 <= dt->cptl &&
                (dt->abTypes[ii + 1] & (~PT_CLOSEFIGURE)) == PT_BEZIERTO &&
                (dt->abTypes[ii + 2] & (~PT_CLOSEFIGURE)) == PT_BEZIERTO) {
               double x1, y1, x2, y2;

               LogicalUnitToPdf(pd, dt->aptl[ii].x,     dt->aptl[ii].y, x1, y1);
               LogicalUnitToPdf(pd, dt->aptl[ii + 1].x, dt->aptl[ii + 1].y, x2, y2);
               LogicalUnitToPdf(pd, dt->aptl[ii + 2].x, dt->aptl[ii + 2].y, px, py);

               painter->CubicBezierTo(x1, y1, x2, y2, px, py);
               ii += 2;
            } else {
               if (pd->on_error) {
                  pd->on_error(ERROR_NOT_SUPPORTED, "PolyDraw: Path with PT_BEZIERTO, but points don't match", pd->on_error_user_data);
               }
            }
            break;
         default:
            if (pd->on_error) {
               pd->on_error(ERROR_NOT_SUPPORTED, "PolyDraw: Path of unknown type encountered", pd->on_error_user_data);
            }
            break;
         }

         if (types[ii] & PT_CLOSEFIGURE) {
            painter->ClosePath();
         }
      }

      drawPath(pd, true);
   } break;
   case EMR_POLYDRAW16: {
      const EMRPOLYDRAW16 *dt = (const EMRPOLYDRAW16 *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      double px, py;
      DWORD ii;
      const BYTE *types = (const BYTE *) STRUCT_OFFSET(dt->apts, dt->cpts * sizeof(dt->apts[0]));

      for (ii = 0; ii < dt->cpts; ii++) {
         switch(types[ii] & (~PT_CLOSEFIGURE)) {
         case PT_MOVETO:
            LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, px, py);
            painter->MoveTo(px, py);
            break;
         case PT_LINETO:
            LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, px, py);
            painter->LineTo(px, py);
            break;
         case PT_BEZIERTO:
            if (ii + 3 <= dt->cpts &&
                (types[ii + 1] & (~PT_CLOSEFIGURE)) == PT_BEZIERTO &&
                (types[ii + 2] & (~PT_CLOSEFIGURE)) == PT_BEZIERTO) {
               double x1, y1, x2, y2;

               LogicalUnitToPdf(pd, dt->apts[ii].x,     dt->apts[ii].y, x1, y1);
               LogicalUnitToPdf(pd, dt->apts[ii + 1].x, dt->apts[ii + 1].y, x2, y2);
               LogicalUnitToPdf(pd, dt->apts[ii + 2].x, dt->apts[ii + 2].y, px, py);

               painter->CubicBezierTo(x1, y1, x2, y2, px, py);
               ii += 2;
            } else {
               if (pd->on_error) {
                  pd->on_error(ERROR_NOT_SUPPORTED, "PolyDraw16: Path with PT_BEZIERTO, but points don't match", pd->on_error_user_data);
               }
            }
            break;
         default:
            if (pd->on_error) {
               pd->on_error(ERROR_NOT_SUPPORTED, "PolyDraw16: Path of unknown type encountered", pd->on_error_user_data);
            }
            break;
         }

         if (types[ii] & PT_CLOSEFIGURE) {
            painter->ClosePath();
         }
      }

      drawPath(pd, true);
   } break;
   case EMR_POLYGON: {
      const EMRPOLYGON *dt = (const EMRPOLYGON *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      if (dt->cptl > 0) {
         double dX, dY;

         maybePushWorldMatrix(pd);

         LogicalUnitToPdf(pd, dt->aptl[0].x, dt->aptl[0].y, dX, dY);
         painter->MoveTo(dX, dY);

         DWORD ii;
         for (ii = 1; ii < dt->cptl; ii++) {
            LogicalUnitToPdf(pd, dt->aptl[ii].x, dt->aptl[ii].y, dX, dY);
            painter->LineTo(dX, dY);
         }

         LogicalUnitToPdf(pd, dt->aptl[0].x, dt->aptl[0].y, dX, dY);
         painter->LineTo(dX, dY);

         drawPath(pd, false, true);

         maybePopWorldMatrix(pd);
      }
   } break;
   case EMR_POLYGON16: {
      const EMRPOLYGON16 *dt = (const EMRPOLYGON16 *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      if (dt->cpts > 0) {
         double dX, dY;

         maybePushWorldMatrix(pd);

         LogicalUnitToPdf(pd, dt->apts[0].x, dt->apts[0].y, dX, dY);
         painter->MoveTo(dX, dY);

         DWORD ii;
         for (ii = 1; ii < dt->cpts; ii++) {
            LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, dX, dY);
            painter->LineTo(dX, dY);
         }

         LogicalUnitToPdf(pd, dt->apts[0].x, dt->apts[0].y, dX, dY);
         painter->LineTo(dX, dY);

         drawPath(pd, false, true);

         maybePopWorldMatrix(pd);
      }
   } break;
   case EMR_POLYLINE: {
      const EMRPOLYLINE16 *dt = (const EMRPOLYLINE16 *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      if (dt->cpts > 0) {
         DWORD ii;
         double dX, dY;

         maybePushWorldMatrix(pd);

         LogicalUnitToPdf(pd, dt->apts[0].x, dt->apts[0].y, dX, dY);
         painter->MoveTo(dX, dY);

         for (ii = 1; ii < dt->cpts; ii++) {
            LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, dX, dY);
            painter->LineTo(dX, dY);
         }
         drawPath(pd, true);

         maybePopWorldMatrix(pd);
      }
   } break;
   case EMR_POLYLINE16: {
      const EMRPOLYLINE16 *dt = (const EMRPOLYLINE16 *) lpEMFR;

      if (pd->buildingPath) {
         break;
      }

      if (dt->cpts > 0) {
         DWORD ii;
         double dX, dY;

         maybePushWorldMatrix(pd);

         LogicalUnitToPdf(pd, dt->apts[0].x, dt->apts[0].y, dX, dY);
         painter->MoveTo(dX, dY);

         for (ii = 1; ii < dt->cpts; ii++) {
            LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, dX, dY);
            painter->LineTo(dX, dY);
         }
         drawPath(pd, true);

         maybePopWorldMatrix(pd);
      }
   } break;
   case EMR_POLYLINETO: {
      const EMRPOLYLINETO *dt = (const EMRPOLYLINETO *) lpEMFR;
      DWORD ii;
      double dX, dY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      dX = pd->current_posX;
      dY = pd->current_posY;
      painter->MoveTo(dX, dY);

      for (ii = 0; ii < dt->cptl; ii++) {
         LogicalUnitToPdf(pd, dt->aptl[ii].x, dt->aptl[ii].y, dX, dY);
         painter->LineTo(dX, dY);

         pd->current_posX = dX;
         pd->current_posY = dY;
      }
      drawPath(pd, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYLINETO16: {
      const EMRPOLYLINETO16 *dt = (const EMRPOLYLINETO16 *) lpEMFR;
      DWORD ii;
      double dX, dY;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      dX = pd->current_posX;
      dY = pd->current_posY;
      painter->MoveTo(dX, dY);

      for (ii = 0; ii < dt->cpts; ii++) {
         LogicalUnitToPdf(pd, dt->apts[ii].x, dt->apts[ii].y, dX, dY);
         painter->LineTo(dX, dY);

         pd->current_posX = dX;
         pd->current_posY = dY;
      }
      drawPath(pd, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYPOLYGON: {
      const EMRPOLYPOLYGON *dt = (const EMRPOLYPOLYGON *) lpEMFR;
      DWORD ii, jj, pointIdx = 0;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      for (jj = 0; jj < dt->nPolys; jj++) {
         if (dt->aPolyCounts[jj] > 0) {
            double dX, dY, fromX, fromY;

            LogicalUnitToPdf(pd, dt->aptl[pointIdx].x, dt->aptl[pointIdx].y, dX, dY);
            painter->MoveTo(dX, dY);
            fromX = dX;
            fromY = dY;

            for (ii = 1; ii < dt->aPolyCounts[jj]; ii++, pointIdx++) {
               LogicalUnitToPdf(pd, dt->aptl[pointIdx].x, dt->aptl[pointIdx].y, dX, dY);
               painter->LineTo(dX, dY);
            }
            painter->LineTo(fromX, fromY);

            drawPath(pd, false, true);
         }
      }

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYPOLYGON16: {
      const EMRPOLYPOLYGON16 *dt = (const EMRPOLYPOLYGON16 *) lpEMFR;
      DWORD ii, jj, pointIdx = 1;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      for (jj = 0; jj < dt->nPolys; jj++) {
         if (dt->aPolyCounts[jj] > 0) {
            double dX, dY, fromX, fromY;

            LogicalUnitToPdf(pd, dt->apts[pointIdx].x, dt->apts[pointIdx].y, dX, dY);
            painter->MoveTo(dX, dY);
            fromX = dX;
            fromY = dY;

            pointIdx++;
            for (ii = 1; ii < dt->aPolyCounts[jj]; ii++, pointIdx++) {
               LogicalUnitToPdf(pd, dt->apts[pointIdx].x, dt->apts[pointIdx].y, dX, dY);
               painter->LineTo(dX, dY);
            }
            painter->LineTo(fromX, fromY);
         }
      }

      drawPath(pd, false, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYPOLYLINE: {
      const EMRPOLYPOLYLINE *dt = (const EMRPOLYPOLYLINE *) lpEMFR;
      DWORD ii, jj, pointIdx = 0;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      for (jj = 0; jj < dt->nPolys; jj++) {
         if (dt->aPolyCounts[jj] > 0) {
            double dX, dY;

            LogicalUnitToPdf(pd, dt->aptl[pointIdx].x, dt->aptl[pointIdx].y, dX, dY);
            painter->MoveTo(dX, dY);

            for (ii = 1; ii < dt->aPolyCounts[jj]; ii++, pointIdx++) {
               LogicalUnitToPdf(pd, dt->aptl[pointIdx].x, dt->aptl[pointIdx].y, dX, dY);
               painter->LineTo(dX, dY);
            }

            drawPath(pd, true);
         }
      }

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYPOLYLINE16: {
      const EMRPOLYPOLYLINE16 *dt = (const EMRPOLYPOLYLINE16 *) lpEMFR;
      DWORD ii, jj, pointIdx = 1;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      for (jj = 0; jj < dt->nPolys; jj++) {
         if (dt->aPolyCounts[jj] > 0) {
            double dX, dY;

            LogicalUnitToPdf(pd, dt->apts[pointIdx].x, dt->apts[pointIdx].y, dX, dY);
            painter->MoveTo(dX, dY);

            pointIdx++;
            for (ii = 1; ii < dt->aPolyCounts[jj]; ii++, pointIdx++) {
               LogicalUnitToPdf(pd, dt->apts[pointIdx].x, dt->apts[pointIdx].y, dX, dY);
               painter->LineTo(dX, dY);
            }
         }
      }

      drawPath(pd, true);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_POLYTEXTOUTA:
   case EMR_POLYTEXTOUTW: {
      const EMRPOLYTEXTOUTW *dt = (const EMRPOLYTEXTOUTW *) lpEMFR;
      LONG ii;

      for (ii = 0; ii < dt->cStrings; ii++) {
         drawText(pd, lpEMFR, dt->aemrtext[ii], (pd->currentDCState.iMapMode == MM_ANISOTROPIC || pd->currentDCState.iMapMode == MM_ISOTROPIC) && dt->iGraphicsMode == GM_COMPATIBLE && dt->eyScale < 0.0);
      }
   } break;
   case EMR_REALIZEPALETTE: {
      // ignored, called after SelectPalette
   } break;
   case EMR_RECTANGLE: {
      const EMRRECTANGLE *dt = (const EMRRECTANGLE *) lpEMFR;
      double dx, dy, w, h;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->rclBox.left, dt->rclBox.top, dx, dy);
      LogicalUnitToPdf(pd, dt->rclBox.right, dt->rclBox.bottom, w, h);

      w -= dx;
      h -= dy;

      painter->Rectangle(dx, dy, w, h);
      drawPath(pd, false);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_RESIZEPALETTE: {
      // const EMRRESIZEPALETTE *dt = (const EMRRESIZEPALETTE *) lpEMFR;
      // ignored
   } break;
   case EMR_RESTOREDC: {
      const EMRRESTOREDC *dt = (const EMRRESTOREDC *) lpEMFR;
      LONG level = dt->iRelative;
      if (level < 0) {
         while (level < 0 && !pd->savedStates.empty()) {
            restoreState(pd);
            level++;
         }
      } else {
         while (level > (LONG) pd->savedStates.size()) {
            restoreState(pd);
         }
      }
   } break;
   case EMR_ROUNDRECT: {
      const EMRROUNDRECT *dt = (const EMRROUNDRECT *) lpEMFR;
      double dx, dy, w, h, rx, ry;

      if (pd->buildingPath) {
         break;
      }

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->rclBox.left, dt->rclBox.top, dx, dy);
      LogicalUnitToPdf(pd, dt->rclBox.right, dt->rclBox.bottom, w, h);
      LogicalUnitToPdf(pd, dt->szlCorner.cx, dt->szlCorner.cy, rx, ry, false);

      w -= dx;
      h -= dy;

      rx = fabs(rx);
      ry = fabs(ry);

      // -ry because height is negative, because top-at-bottom for PDF
      painter->Rectangle(dx, dy, w, h, (rx / 2.0) * (w < 0.0 ? -1.0 : 1.0), (ry / 2.0) * (h < 0.0 ? -1.0 : 1.0));
      drawPath(pd, false);

      maybePopWorldMatrix(pd);
   } break;
   case EMR_SAVEDC: {
      storeState(pd);
   } break;
   case EMR_SCALEVIEWPORTEXTEX: {
      const EMRSCALEVIEWPORTEXTEX *dt = (const EMRSCALEVIEWPORTEXTEX *) lpEMFR;

      if (dt->xDenom && dt->yDenom) {
         pd->viewportExt.cx = pd->viewportExt.cx * dt->xNum / dt->xDenom;
         pd->viewportExt.cy = pd->viewportExt.cy * dt->yNum / dt->yDenom;
      }
   } break;
   case EMR_SCALEWINDOWEXTEX: {
      const EMRSCALEWINDOWEXTEX *dt = (const EMRSCALEWINDOWEXTEX *) lpEMFR;

      if (dt->xDenom && dt->yDenom) {
         pd->windowExt.cx = pd->windowExt.cx * dt->xNum / dt->xDenom;
         pd->windowExt.cy = pd->windowExt.cy * dt->yNum / dt->yDenom;
      }
   } break;
   case EMR_SELECTCLIPPATH: {
      const EMRSELECTCLIPPATH *dt = (const EMRSELECTCLIPPATH *) lpEMFR;
      HRGN rgn2 = convertPathToRegion(pd);
      if (rgn2) {
         applyClipRegion(pd, rgn2, dt->iMode);
         DeleteObject(rgn2);
      }
   } break;
   case EMR_SELECTOBJECT: {
      const EMRSELECTOBJECT *dt = (const EMRSELECTOBJECT *) lpEMFR;

      if ((dt->ihObject & ENHMETA_STOCK_OBJECT) == ENHMETA_STOCK_OBJECT) {
         DWORD ihObj = dt->ihObject & ~ENHMETA_STOCK_OBJECT;
         switch (ihObj) {
         case BLACK_BRUSH:
         case DKGRAY_BRUSH:
         //case DC_BRUSH:
         case GRAY_BRUSH:
         case LTGRAY_BRUSH:
         case WHITE_BRUSH: {
            map<DWORD, LOGBRUSH32 *>::iterator it = pd->stdBrushes.find(ihObj);
            if (it != pd->stdBrushes.end()) {
               pd->currentDCState.setBrush = it->second;
               pd->currentDCState.setBrushObjectIndex = -1;
               pd->currentDCState.setBrushName = NULL;
               pd->stdBrushes[ihObj] = it->second;
            } else {
               LOGBRUSH32 *lb = (LOGBRUSH32 *) malloc (sizeof (LOGBRUSH32));
               if (!lb) {
                  if (pd->on_error) {
                     pd->on_error (ERROR_NOT_ENOUGH_MEMORY, "Failed to copy standard brush structure", pd->on_error_user_data);
                  }
                  break;
               }

               if (!GetObjectA(GetStockObject(ihObj), sizeof(LOGBRUSH32), lb)) {
                  pd->currentDCState.brushSet = false;
                  if (pd->on_error) {
                     pd->on_error (GetLastError(), "Failed to get brush object", pd->on_error_user_data);
                  }
                  free (lb);
               } else {
                  pd->currentDCState.brushSet = true;
                  pd->currentDCState.setBrush = lb;
                  pd->currentDCState.setBrushObjectIndex = -1;
                  pd->currentDCState.setBrushName = NULL;
                  pd->stdBrushes[ihObj] = lb;
               }
            }
         } break;
         case NULL_BRUSH: { // case HOLLOW_BRUSH
            pd->currentDCState.brushSet = false;
         } break;

         case NULL_PEN: {
            LOGPEN lp;

            lp.lopnColor = RGB(0,0,0);
            lp.lopnWidth.x = 0;
            lp.lopnWidth.y = 0;
            lp.lopnStyle = PS_NULL;

            pd->currentDCState.setPen = lp;
            pd->currentDCState.penSet = false;
            pd->currentDCState.extPenSet = false;
         } break;
         case BLACK_PEN:
         //case DC_PEN:
         case  WHITE_PEN: {
            LOGPEN lp;

            pd->currentDCState.extPenSet = false;

            if (!GetObjectA(GetStockObject(ihObj), sizeof(lp), &lp)) {
               pd->currentDCState.penSet = false;
               if (pd->on_error) {
                  pd->on_error (GetLastError(), "Failed to get pen object", pd->on_error_user_data);
               }
            } else {
               pd->currentDCState.setPen = lp;
               pd->currentDCState.penSet = true;
            }
         } break;

         case ANSI_FIXED_FONT:
         case ANSI_VAR_FONT:
         case DEVICE_DEFAULT_FONT:
         case DEFAULT_GUI_FONT:
         case OEM_FIXED_FONT:
         case SYSTEM_FONT:
         case SYSTEM_FIXED_FONT: {
            LOGFONTA lf;

            if (!GetObjectA(GetStockObject(ihObj), sizeof(lf), &lf)) {
               pd->currentDCState.fontAngle = 0;
               pd->currentDCState.fontSet = false;
               if (pd->on_error) {
                  pd->on_error (GetLastError(), "Failed to get system font object", pd->on_error_user_data);
               }
            } else {
               pd->currentDCState.setFont = lf;
            }
         } break;

         case DEFAULT_PALETTE: {
         } break;

         default: {
            if (pd->on_error) {
               char buff[128];
               sprintf (buff, "Stock object %u (%x) to select not recognized", ihObj, ihObj);
               pd->on_error (ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
            }
         } break;
         }
      } else  {
         if (pd->objects.find (dt->ihObject) != pd->objects.end()) {
            switch (pd->objects[dt->ihObject]->type) {
            case OBJECT_DATA_TYPE_PEN: {
               pd->currentDCState.penSet = true;
               pd->currentDCState.extPenSet = false;
               pd->currentDCState.setPen = pd->objects[dt->ihObject]->v.pen;
            } break;
            case OBJECT_DATA_TYPE_EXTPEN: {
               pd->currentDCState.penSet = false;
               pd->currentDCState.extPenSet = true;
               pd->currentDCState.setExtPen = pd->objects[dt->ihObject]->v.extPen;
               pd->currentDCState.setExtPenBrushName = &pd->objects[dt->ihObject]->brushName;
            } break;
            case OBJECT_DATA_TYPE_BRUSH: {
               pd->currentDCState.brushSet = true;
               pd->currentDCState.setBrush = pd->objects[dt->ihObject]->v.brush;
               pd->currentDCState.setBrushObjectIndex = dt->ihObject;
               pd->currentDCState.setBrushName = &pd->objects[dt->ihObject]->brushName;
            } break;
            case OBJECT_DATA_TYPE_FONT: {
               pd->currentDCState.fontSet = true;
               pd->currentDCState.setFont = pd->objects[dt->ihObject]->v.font;
            } break;
            case OBJECT_DATA_TYPE_PALETTE: {
               // nothing to do
            } break;
            }
         } else if (pd->on_error) {
            char buff[128];
            sprintf (buff, "Object %u (%x) to select does not exist", dt->ihObject, dt->ihObject);
            pd->on_error (ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
         }
      }
      } break;
   case EMR_SELECTPALETTE: {
      // const EMRSELECTPALETTE *dt = (const EMRSELECTPALETTE *) lpEMFR;
      // ignored
   } break;
   case EMR_SETARCDIRECTION: {
      const EMRSETARCDIRECTION *dt = (const EMRSETARCDIRECTION *) lpEMFR;
      pd->currentDCState.iArcDirection = dt->iArcDirection;
   } break;
   case EMR_SETBKCOLOR: {
      const EMRSETBKCOLOR *dt = (const EMRSETBKCOLOR *) lpEMFR;

      pd->currentDCState.bkColor = dt->crColor;
      pd->currentDCState.bkColorSet = true;
   } break;
   case EMR_SETBKMODE: {
      const EMRSETBKMODE *dt = (const EMRSETBKMODE *) lpEMFR;
      pd->currentDCState.iBkMode = dt->iMode;
   } break;
   case EMR_SETBRUSHORGEX: {
      const EMRSETBRUSHORGEX *dt = (const EMRSETBRUSHORGEX *) lpEMFR;
      pd->currentDCState.brushOrg.x = dt->ptlOrigin.x;
      pd->currentDCState.brushOrg.y = dt->ptlOrigin.y;
   } break;
   //case EMR_SETCOLORADJUSTMENT: { } break;
   //case EMR_SETCOLORSPACE: { } break;
   case EMR_SETDIBITSTODEVICE: {
      const EMRSETDIBITSTODEVICE *dt = (const EMRSETDIBITSTODEVICE *) lpEMFR;

      maybePushWorldMatrix(pd);

      if (dt->cbBmiSrc > 0 && dt->cbBitsSrc > 0 && dt->offBmiSrc > 0 && dt->offBitsSrc > 0 &&
          dt->cySrc > 0 && dt->ySrc < dt->cySrc) {
         LONG ySrc = dt->ySrc, cySrc = dt->cySrc, yDest = dt->yDest;

         if (dt->iStartScan >= ySrc) {
            cySrc = cySrc - (dt->iStartScan - ySrc);
            ySrc = 0;
         } else if (dt->iStartScan > 0) {
            yDest = yDest + dt->iStartScan;
            cySrc = cySrc - dt->iStartScan;
            ySrc = ySrc - dt->iStartScan;
         }

         if (cySrc > dt->cScans) {
            yDest = yDest + (cySrc - dt->cScans) + ySrc;
            cySrc = cySrc - (cySrc - dt->cScans) - ySrc;
         }

         drawImage(pd, dt->xSrc, ySrc, dt->cxSrc, cySrc, dt->xDest, yDest, dt->cxSrc, cySrc, SRCCOPY, dt->iUsageSrc,
            (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc, false);
      }

      maybePopWorldMatrix(pd);
   } break;
   case EMR_SETICMMODE: {
      const EMRSETICMMODE *dt = (const EMRSETICMMODE *) lpEMFR;
      if (dt->iMode != ICM_OFF && pd->on_error) {
         char buff[128];

         sprintf (buff, "Drawing 0x%x (%s) not supported, skipping it", lpEMFR->iType, get_emfr_type_as_string (lpEMFR->iType));

         pd->on_error (ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
   } break;
   //case EMR_SETICMPROFILEA: { } break;
   //case EMR_SETICMPROFILEW: { } break;
   //case EMR_SETLAYOUT: { } break;
   case EMR_SETMAPMODE: {
      const EMRSETMAPMODE *dt = (const EMRSETMAPMODE *) lpEMFR;
      pd->currentDCState.iMapMode = dt->iMode;
   }break;
   case EMR_SETMAPPERFLAGS: {
      // const EMRSETMAPPERFLAGS *dt = (const EMRSETMAPPERFLAGS *) lpEMFR;
      // ignored, applied to replayDC only
   } break;
   case EMR_SETMETARGN: {
      // const EMRSETMETARGN *dt= (const EMRSETMETARGN *) lpEMFR;

      painter->Save();
      pd->currentDCState.metaRgnCount++;
   } break;
   case EMR_SETMITERLIMIT: {
      const EMRSETMITERLIMIT *dt = (const EMRSETMITERLIMIT *) lpEMFR;
      // maybe just ignored?
      if (dt->eMiterLimit > 1e-6) {
         painter->SetMiterLimit(dt->eMiterLimit);
      }
   } break;
   //case EMR_SETPALETTEENTRIES: { } break;
   case EMR_SETPIXELV: {
      const EMRSETPIXELV *dt = (const EMRSETPIXELV *) lpEMFR;
      double dx, dy, w, h;

      maybePushWorldMatrix(pd);

      LogicalUnitToPdf(pd, dt->ptlPixel.x, dt->ptlPixel.y, dx, dy);
      LogicalUnitToPdf(pd, dt->ptlPixel.x + 1, dt->ptlPixel.y + 1, w, h);

      w -= dx;
      h -= dy;

      painter->Rectangle(dx, dy, w, h);
      painter->SetColor(GetRValue(dt->crColor)/255.0, GetGValue(dt->crColor)/255.0, GetBValue(dt->crColor)/255.0);
      painter->Fill();

      maybePopWorldMatrix(pd);
   } break;
   case EMR_SETPOLYFILLMODE: {
      const EMRSETPOLYFILLMODE *dt = (const EMRSETPOLYFILLMODE *) lpEMFR;
      pd->currentDCState.iPolyFillMode = dt->iMode;
   } break;
   case EMR_SETROP2: {
      const EMRSETROP2 *dt = (const EMRSETROP2 *) lpEMFR;
      if (dt->iMode == R2_COPYPEN) {
      } else {
         if (pd->on_error) {
            char buff[128];
            sprintf(buff, "SetROP2 mode %s (0x%x) not supported",
               dt->iMode == R2_BLACK         ? "R2_BLACK" :
               dt->iMode == R2_COPYPEN         ? "R2_COPYPEN" :
               dt->iMode == R2_MASKNOTPEN      ? "R2_MASKNOTPEN" :
               dt->iMode == R2_MASKPEN         ? "R2_MASKPEN" :
               dt->iMode == R2_MASKPENNOT      ? "R2_MASKPENNOT" :
               dt->iMode == R2_MERGENOTPEN   ? "R2_MERGENOTPEN" :
               dt->iMode == R2_MERGEPEN      ? "R2_MERGEPEN" :
               dt->iMode == R2_MERGEPENNOT   ? "R2_MERGEPENNOT" :
               dt->iMode == R2_NOP            ? "R2_NOP" :
               dt->iMode == R2_NOT            ? "R2_NOT" :
               dt->iMode == R2_NOTCOPYPEN      ? "R2_NOTCOPYPEN" :
               dt->iMode == R2_NOTMASKPEN      ? "R2_NOTMASKPEN" :
               dt->iMode == R2_NOTMERGEPEN   ? "R2_NOTMERGEPEN" :
               dt->iMode == R2_NOTXORPEN      ? "R2_NOTXORPEN" :
               dt->iMode == R2_WHITE         ? "R2_WHITE" :
               dt->iMode == R2_XORPEN         ? "R2_XORPEN" :
               "???", dt->iMode);
            pd->on_error (ERROR_NOT_SUPPORTED, "Other than R2_COPYPEN SetROP2 mode is not supported", pd->on_error_user_data);
         }
      }
   } break;
   case EMR_SETSTRETCHBLTMODE: {
      const EMRSETSTRETCHBLTMODE *dt = (const EMRSETSTRETCHBLTMODE *) lpEMFR;
      pd->currentDCState.iStretchMode = dt->iMode;
   } break;
   case EMR_SETTEXTALIGN: {
      const EMRSETTEXTALIGN *dt = (const EMRSETTEXTALIGN *) lpEMFR;
      pd->currentDCState.textAlign = dt->iMode;
   } break;
   case EMR_SETTEXTCOLOR: {
      const EMRSETTEXTCOLOR *dt = (const EMRSETTEXTCOLOR *) lpEMFR;
      pd->currentDCState.textColor = dt->crColor;
   } break;
   case EMR_SETVIEWPORTEXTEX: {
      const EMRSETVIEWPORTEXTEX *dt = (const EMRSETVIEWPORTEXTEX *) lpEMFR;
      pd->viewportExt.cx = dt->szlExtent.cx;
      pd->viewportExt.cy = dt->szlExtent.cy;
   } break;
   case EMR_SETVIEWPORTORGEX: {
      const EMRSETVIEWPORTORGEX *dt = (const EMRSETVIEWPORTORGEX *) lpEMFR;
      pd->viewportOrg.x = dt->ptlOrigin.x;
      pd->viewportOrg.y = dt->ptlOrigin.y;
   } break;
   case EMR_SETWINDOWEXTEX: {
      const EMRSETWINDOWEXTEX *dt = (const EMRSETWINDOWEXTEX *) lpEMFR;
      pd->windowExt.cx = dt->szlExtent.cx;
      pd->windowExt.cy = dt->szlExtent.cy;
   } break;
   case EMR_SETWINDOWORGEX: {
      const EMRSETWINDOWORGEX *dt = (const EMRSETWINDOWORGEX *) lpEMFR;
      pd->windowOrg.x = dt->ptlOrigin.x;
      pd->windowOrg.y = dt->ptlOrigin.y;
   } break;
   case EMR_SETWORLDTRANSFORM: {
      const EMRSETWORLDTRANSFORM *dt = (const EMRSETWORLDTRANSFORM *) lpEMFR;
      DWorldMatrix dMatrix;

      dMatrix.eM11 = dt->xform.eM11;
      dMatrix.eM12 = dt->xform.eM12;
      dMatrix.eM21 = dt->xform.eM21;
      dMatrix.eM22 = dt->xform.eM22;
      dMatrix.eDx = dt->xform.eDx;
      dMatrix.eDy = dt->xform.eDy;

      if (double_equal (dMatrix.eM11, 1.0) && double_equal (dMatrix.eM12, 0.0) &&
          double_equal (dMatrix.eM21, 0.0) && double_equal (dMatrix.eM22, 1.0) &&
          double_equal (dMatrix.eDx,  0.0) && double_equal (dMatrix.eDy,  0.0)) {
         pd->currentDCState.worldMatrixValid = false;
      } else {
         pd->currentDCState.worldMatrix = dMatrix;
         pd->currentDCState.worldMatrixValid = true;
      }
   } break;
   case EMR_STRETCHBLT: {
      const EMRSTRETCHBLT *dt = (const EMRSTRETCHBLT *) lpEMFR;

      maybePushWorldMatrix(pd);

      if (pd->currentDCState.iStretchMode == COLORONCOLOR) {
         if (dt->cbBmiSrc > 0 && dt->cbBitsSrc > 0 && dt->offBmiSrc > 0 && dt->offBitsSrc > 0) {
            drawImage(pd, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->iUsageSrc,
               (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc, false);
         }
      } else {
         if (pd->on_error) {
            pd->on_error (ERROR_NOT_SUPPORTED, "StretchBlt: Unsupported stretch mode", pd->on_error_user_data);
         }
      }

      maybePopWorldMatrix(pd);
   } break;
   case EMR_STRETCHDIBITS: {
      const EMRSTRETCHDIBITS *dt = (const EMRSTRETCHDIBITS *) lpEMFR;

      maybePushWorldMatrix(pd);

      if (dt->cbBmiSrc > 0 && dt->cbBitsSrc > 0 && dt->offBmiSrc > 0 && dt->offBitsSrc > 0) {
         drawImage(pd, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, dt->dwRop, dt->iUsageSrc,
            (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc, false);
      }

      maybePopWorldMatrix(pd);
   } break;
   case EMR_STROKEANDFILLPATH: {
      const EMRSTROKEANDFILLPATH *dt = (const EMRSTROKEANDFILLPATH *) lpEMFR;

      maybePushWorldMatrix(pd);
      if (applyDCPath(pd, pd->replayDC)) {
         drawPath(pd, false, true);
      }
      maybePopWorldMatrix(pd);
   } break;
   case EMR_STROKEPATH: {
      const EMRSTROKEPATH *dt = (const EMRSTROKEPATH *) lpEMFR;

      maybePushWorldMatrix(pd);
      if (applyDCPath(pd, pd->replayDC)) {
         drawPath(pd, true);
      }
      maybePopWorldMatrix(pd);
   } break;
   case EMR_WIDENPATH: {
      // const EMRWIDENPATH *dt = (const EMRWIDENPATH *) lpEMFR;
      // applied to pd->replayDC only
   } break;
   case EMR_TRANSPARENTBLT: {
      const EMRTRANSPARENTBLT *dt = (const EMRTRANSPARENTBLT *) lpEMFR;

      if (dt->cbBmiSrc != 0 && dt->cbBitsSrc != 0 && dt->offBitsSrc != 0 && dt->offBmiSrc != 0) {
         painter->Save();
         maybePushWorldMatrix(pd);

         drawImage(pd, dt->xSrc, dt->ySrc, dt->cxSrc, dt->cySrc, dt->xDest, dt->yDest, dt->cxDest, dt->cyDest, SRCCOPY, dt->iUsageSrc,
            (const BITMAPINFO *) STRUCT_OFFSET(lpEMFR, dt->offBmiSrc), (const BYTE *) STRUCT_OFFSET(lpEMFR, dt->offBitsSrc), dt->cbBitsSrc, true,
            &(dt->dwRop));

         maybePopWorldMatrix(pd);
         painter->Restore();
      } else if (pd->on_error) {
         pd->on_error (ERROR_NOT_SUPPORTED, "TransparentBlt: Unsupported image format", pd->on_error_user_data);
      }
   } break;
   default:
      if (pd->on_error) {
         char buff[128];

         sprintf (buff, "Drawing 0x%x (%s) not supported, skipping it", lpEMFR->iType, get_emfr_type_as_string (lpEMFR->iType));

         pd->on_error (ERROR_NOT_SUPPORTED, buff, pd->on_error_user_data);
      }
      break;
   }

   if (pd->replayDC && !replayDCUpdated) {
      if (!PlayEnhMetaFileRecord(pd->replayDC, lpHTable, lpEMFR, nObj)) {
         #ifdef _DEBUG
         if (pd->on_error &&
             lpEMFR->iType != EMR_SETPIXELV &&
             lpEMFR->iType != EMR_STROKEANDFILLPATH &&
             lpEMFR->iType != EMR_STROKEPATH &&
             lpEMFR->iType != EMR_FILLPATH) {
            char buff[128];
            sprintf(buff, "Failed to write %s record to a DC", get_emfr_type_as_string(lpEMFR->iType));
            pd->on_error (ERROR_WRITE_FAULT, buff, pd->on_error_user_data);
         }
         #endif
      } else if ((pd->currentDCState.textAlign & TA_UPDATECP) != 0 && pd->replayDC && !pd->buildingPath) {
         if (lpEMFR->iType == EMR_EXTTEXTOUTA ||
             lpEMFR->iType == EMR_EXTTEXTOUTW ||
             lpEMFR->iType == EMR_POLYTEXTOUTA ||
             lpEMFR->iType == EMR_POLYTEXTOUTW) {
            POINT pos;
            if (GetCurrentPositionEx (pd->replayDC, &pos)) {
               LogicalUnitToPdf(pd, pos.x, pos.y, pd->current_posX, pd->current_posY);
            }
         }
      }
   }

   return 1;
}
//---------------------------------------------------------------------------
