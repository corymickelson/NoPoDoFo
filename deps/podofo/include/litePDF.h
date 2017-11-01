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

#ifndef LITEPDF_H
#define LITEPDF_H

#ifdef  __cplusplus
#define LITEPDF_BEGIN_DECLS  extern "C" {
#define LITEPDF_END_DECLS    }
#else
#define LITEPDF_BEGIN_DECLS
#define LITEPDF_END_DECLS
#endif

#ifdef LITEPDF_DLL_BUILD
#define LITEPDF_PUBLIC /* __declspec(dllexport) */
#else
#define LITEPDF_PUBLIC __declspec(dllimport)
#endif

/** @mainpage litePDF
litePDF is a library (DLL), which allows creating new and editing of existing PDF documents with simple API.
Page content is drawn with standard GDI functions through a device context (HDC or TCanvas, in case of Delphi or C++ Builder).<br>
<br>
Main features of litePDF:
<ul>
<li>create new PDF documents in memory only, or with direct write to a disk</li>
<li>load of existing PDF documents</li>
<li>modify or delete of existing pages</li>
<li>copy pages from other documents</li>
<li>incremental update of PDF documents</li>
<li>encryption of PDF documents</li>
<li>digitally sign PDF documents</li>
<li>draw with GDI functions both directly to a page, and to a resource/template (XObject)</li>
<li>draw of created resources into a page</li>
<li>font embedding, complete or subset</li>
<li>font substitution</li>
<li>JPEG compression for images</li>
<li>attach files into PDF documents</li>
<li>low-level PDF operations</li>
</ul>
<br>
litePDF uses an Open Source project <a href="http://podofo.sf.net">PoDoFo</a> for manipulation of PDF documents,
and offers direct PdfDocument pointer to the PoDoFo interface, thus the library users can do anything what the PoDoFo
offers.
*/

/** @page units Units
Since litePDF 1.2.0.0 the functions which used millimeters as their measure
units can use also inches, or fractions of the millimeters and inches.
The reason for the fraction is to not use architectural dependent types
in the API, where the @a double type is. The unit value is rounded down
to unsigned integers in the API. The default unit is set to millimeters
(@ref LITEPDF_UNIT_MM) for backward compatibility.<br>
<br>
Call @ref litePDF_GetUnit to determine which unit is currently used. To change
the unit to be used call @ref litePDF_SetUnit. For example, to create a page
of size 8.5 inches x 11.0 inches, set the unit to @ref LITEPDF_UNIT_10TH_INCH
and then call the @ref litePDF_AddPage with the width 85 and height 110
(10-times larger, because the current unit is 1/10th of an inch).<br>
<br>
All functions use the unit set for the current context, except of
the @ref litePDF_DrawResource, which has an explicit argument for the unit
value. That's for simplicity, to not need to change the units before
the function is called and then set it back after the call.
*/

/** @file litePDF.h
    @brief C interface
   
    This document describes a direct C interface for litePDF.dll API.
*/

LITEPDF_BEGIN_DECLS

typedef void (__stdcall * litePDFErrorCB)(unsigned int code,
                                          const char *msg,
                                          void *user_data);
/**<
   Error callback.
   @param code Error code, same as GetLastError().
   @param msg Detailed error message.
   @param user_data User data provided in @ref litePDF_CreateMemDocument.
*/

void * __stdcall LITEPDF_PUBLIC litePDF_CreateContext(litePDFErrorCB on_error,
                                                      void *on_error_user_data);
/**<
   Creates a new litePDF context. This is used in other litePDF functions.
   The returned context should be freed with @ref litePDF_FreeContext
   when no longer needed.

   @param on_error Callback called on errors. This is used to report litePDF errors and
      is used during whole life time of the returned context.
   @param on_error_user_data User data passed into on_error callback.
   @return Newly created litePDF context. This should be freed with @ref litePDF_FreeContext.
*/

void __stdcall LITEPDF_PUBLIC litePDF_FreeContext(void *pctx);
/**<
   Frees litePDF context previously created with @ref litePDF_CreateContext.
   If it contains any unsaved data, then these are discarded.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
*/

#define LITEPDF_UNIT_UNKNOWN     (0) /**< Unknown unit; usually used to indicate an error */
#define LITEPDF_UNIT_MM          (1) /**< Millimeters unit */
#define LITEPDF_UNIT_10TH_MM     (2) /**< 1/10th of a millimeter unit; 5 mm is value 50 */
#define LITEPDF_UNIT_100TH_MM    (3) /**< 1/100th of a millimeter unit; 5 mm is value 500 */
#define LITEPDF_UNIT_1000TH_MM   (4) /**< 1/1000th of a millimeter unit; 5 mm is value 5000 */
#define LITEPDF_UNIT_INCH        (5) /**< Inch unit */
#define LITEPDF_UNIT_10TH_INCH   (6) /**< 1/10th of an inch unit; 5" is value 50 */
#define LITEPDF_UNIT_100TH_INCH  (7) /**< 1/100th of an inch unit; 5" is value 500 */
#define LITEPDF_UNIT_1000TH_INCH (8) /**< 1/1000th of an inch unit; 5" is value 5000 */

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetUnit(void *pctx,
                                              unsigned int unitValue);
/**<
   Sets a unit to use in functions which expect non-pixel size and position values.
   It can be one of LITEPDF_UNIT values. The default is @ref LITEPDF_UNIT_MM.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param unitValue One of LITEPDF_UNIT values, to set as a unit for the litePDF context.

   @see @ref units, litePDF_GetUnit
*/

unsigned int __stdcall LITEPDF_PUBLIC litePDF_GetUnit(void *pctx);
/**<
   Gets the currently set unit, which is used in functions which expect
   non-pixel size and position values. It can be one of LITEPDF_UNIT values.
   The default is @ref LITEPDF_UNIT_MM.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @return One of LITEPDF_UNIT values, which is set as the current unit for the litePDF context.

   @see @ref units, litePDF_SetUnit
*/

#define LITEPDF_ENCRYPT_PERMISSION_NONE         0x0         /**< Nothing from the rest is allowed */
#define LITEPDF_ENCRYPT_PERMISSION_PRINT        0x00000004  /**< Allow printing the document */
#define LITEPDF_ENCRYPT_PERMISSION_EDIT         0x00000008  /**< Allow modifying the document besides annotations, form fields or changing pages */
#define LITEPDF_ENCRYPT_PERMISSION_COPY         0x00000010  /**< Allow text and graphic extraction */
#define LITEPDF_ENCRYPT_PERMISSION_EDITNOTES    0x00000020  /**< Add or modify text annotations or form fields (if ePdfPermissions_Edit is set also allow to create interactive form fields including signature) */
#define LITEPDF_ENCRYPT_PERMISSION_FILLANDSIGN  0x00000100  /**< Fill in existing form or signature fields */
#define LITEPDF_ENCRYPT_PERMISSION_ACCESSIBLE   0x00000200  /**< Extract text and graphics to support user with disabilities */
#define LITEPDF_ENCRYPT_PERMISSION_DOCASSEMBLY  0x00000400  /**< Assemble the document: insert, create, rotate delete pages or add bookmarks */
#define LITEPDF_ENCRYPT_PERMISSION_HIGHPRINT    0x00000800  /**< Print a high resolution version of the document */

#define LITEPDF_ENCRYPT_ALGORITHM_RC4V1         1           /**< RC4 Version 1 encryption using a 40bit key */
#define LITEPDF_ENCRYPT_ALGORITHM_RC4V2         2           /**< RC4 Version 2 encryption using a 128bit key */
#define LITEPDF_ENCRYPT_ALGORITHM_AESV2         4           /**< AES encryption with a 128 bit key (PDF1.6) */
#define LITEPDF_ENCRYPT_ALGORITHM_AESV3         8           /**< AES encryption with a 256 bit key (PDF1.7 extension 3) */

BOOL __stdcall LITEPDF_PUBLIC litePDF_PrepareEncryption(void *pctx,
                                                        const char *userPassword,
                                                        const char *ownerPassword,
                                                        unsigned int permissions,
                                                        unsigned int algorithm);
/**<
   Prepares encryption for newly created documents. The litePDF
   context should be empty. The encryption is used only with
   @ref litePDF_CreateFileDocument and @ref litePDF_CreateMemDocument, other
   functions ignore it. Use NULL or an empty @a ownerPassword
   to unset any previously set encryption properties.
   Loading an encrypted document lefts it encrypted on save too.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param userPassword User's password, can be an empty string,
      then the user doesn't have to write a password.
   @param ownerPassword Owner's password. Can be NULL or an empty string, to unset
      encryption properties.
   @param permissions Bit-or of LITEPDF_ENCRYPT_PERMISSION flags, to set user's
      permissions for the document.
   @param algorithm One of LITEPDF_ENCRYPT_ALGORITHM constants, an algorithm
      to be used to encrypt the document.
   @return Whether was able to prepare encryption within the litePDF context.

   @see litePDF_CreateFileDocument, litePDF_CreateMemDocument
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateFileDocument(void *pctx,
                                                         const char *fileName);
/**<
   Makes the litePDF context hold a new PDF, which writes directly to a file.
   The passed-in litePDF context should not have opened any other PDF data.
   Call @ref litePDF_Close, to close the file, and possibly save changes to it.
   Most of the operations require memory-based PDF, which can be created
   with @ref litePDF_CreateMemDocument.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param fileName File name to write the PDF result to.
   @return Whether succeeded.

   @note PoDoFo doesn't support creation of file-based documents with AES encryption,
      thus use for it memory-based documents instead (@ref litePDF_CreateMemDocument).

   @see litePDF_PrepareEncryption, litePDF_CreateFileDocumentW, litePDF_CreateMemDocument, litePDF_LoadFromFile
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateFileDocumentW(void *pctx,
                                                          const wchar_t *fileName);
/**<
   This is the same as @ref litePDF_CreateFileDocument, the only difference is that
   the @a fileName is a wide string.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateMemDocument(void *pctx);
/**<
   Makes the litePDF context hold a memory-based PDF. Such PDF can be
   saved with @ref litePDF_SaveToFile or  @ref litePDF_SaveToData.
   The PDF should be closed with @ref litePDF_Close.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @return Whether succeeded.

   @see litePDF_PrepareEncryption, litePDF_CreateFileDocument, litePDF_LoadFromFile
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_LoadFromFile(void *pctx,
                                                   const char *fileName,
                                                   const char *password,
                                                   BOOL loadCompletely,
                                                   BOOL forUpdate);
/**<
   Makes the litePDF context hold a memory-based PDF, which is loaded
   from a disk file. This should be closed with @ref litePDF_Close.
   The @a loadCompletely parameter is used to determine whether the file
   should be loaded into memory completely, or when the file can be read
   on demand. Using complete load requires more memory, but the disk
   file can be changed freely in the background, while incomplete load
   requires left the file without changes while being opened.
   The @a forUpdate parameter specifies whether the file is being opened
   for incremental update. In that case saving such document will result
   in the changes being appended to the end of the original document,
   instead of resaving whole document.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param fileName File name to load the PDF from.
   @param password Password to use for encrypted documents.
   @param loadCompletely Set to TRUE when the file should be loaded completely
      into memory, or FALSE to keep the disk file in use while working with it.
   @param forUpdate Set to TRUE to open the file for incremental update,
      or set to FALSE otherwise.
   @return Whether succeeded.

   @see litePDF_LoadFromFileW
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_LoadFromFileW(void *pctx,
                                                    const wchar_t *fileName,
                                                    const char *password,
                                                    BOOL loadCompletely,
                                                    BOOL forUpdate);
/**<
   This is the same as @ref litePDF_LoadFromFile, the only difference is that
   the @a fileName is a wide string.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_LoadFromData(void *pctx,
                                                   const BYTE *data,
                                                   unsigned int dataLength,
                                                   const char *password,
                                                   BOOL forUpdate);
/**<
   Makes the litePDF context hold a memory-based PDF, which is loaded
   with a PDF data. This should be closed with @ref litePDF_Close.
   The @a forUpdate parameter specifies whether the file is being opened
   for incremental update. In that case saving such document will result
   in the changes being appended to the end of the original document,
   instead of resaving whole document.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param data PDF data to load.
   @param dataLength Length of PDF data.
   @param password Password to use for encrypted documents.
   @param forUpdate Set to TRUE to open the file for incremental update,
      or set to FALSE otherwise.
   @return Whether succeeded.

   @see litePDF_CreateMemDocument, litePDF_SaveToFile, litePDF_SaveToData
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFile(void *pctx,
                                                 const char *fileName);
/**<
   Saves memory-based PDF into a file. The context should hold PDF created only
   with @ref litePDF_CreateMemDocument, @ref litePDF_LoadFromFile or @ref litePDF_LoadFromData.
   Passing any other context results in an error.

   In case the PDF document had been loaded with @ref litePDF_LoadFromFile,
   @ref litePDF_LoadFromFileW or @ref litePDF_LoadFromData with its @a forUpdate
   parameter being TRUE, the resulting document will contain the changes as
   an incremental update (appended at the end of the original document), otherwise
   the whole document is completely rewritten.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param fileName File name to which save the memory-based PDF.
   @return Whether succeeded.

   @note The only valid operation after this is either close the document
      with @ref litePDF_Close, or free the context with @ref litePDF_FreeContext.

   @see litePDF_SaveToFileW, litePDF_SaveToData, litePDF_SaveToFileWithSign, litePDF_Close
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileW(void *pctx,
                                                  const wchar_t *fileName);
/**<
   This is the same as @ref litePDF_SaveToFile, the only difference is that
   the @a fileName is a wide string.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToData(void *pctx,
                                                 BYTE *data,
                                                 unsigned int *dataLength);
/**<
   Saves memory-based PDF into a data. The context should hold PDF created only
   with @ref litePDF_CreateMemDocument, @ref litePDF_LoadFromFile or @ref litePDF_LoadFromData.
   Passing any other context results in an error.

   In case the PDF document had been loaded with @ref litePDF_LoadFromFile,
   @ref litePDF_LoadFromFileW or @ref litePDF_LoadFromData with its @a forUpdate
   parameter being TRUE, the resulting document will contain the changes as
   an incremental update (appended at the end of the original document), otherwise
   the whole document is completely rewritten.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param data [out] Actual data to store the PDF content to. It can be NULL, in which case
      the @a dataLength is populated with large-enough value to hold the whole data.
   @param dataLength [in/out] Tells how many bytes can be stored in @a data. If @a data
      is NULL, then it is set to large-enough value. Passing non-NULL @a data with no enough
      large buffer results in a failure with no change on @a dataLength.
   @return Whether succeeded.

   @note The only valid operation after this is either call of @ref litePDF_SaveToData again,
      to get information about necessary buffer size or data itself, close the document
      with @ref litePDF_Close, or free the context with @ref litePDF_FreeContext.

   @see litePDF_SaveToFile, litePDF_SaveToDataWithSign, litePDF_Close
*/

void __stdcall LITEPDF_PUBLIC litePDF_Close(void *pctx);
/**<
   Closes PDF data in a litePDF context, thus the context doesn't hold anything afterward,
   aka it's like a context just created with @ref litePDF_CreateContext. Do not forget to
   free the context itself with @ref litePDF_FreeContext, when no longer needed. The function
   does nothing, if the context doesn't hold any data. In case of any drawing in progress,
   the drawing is discarded, same as any unsaved changes to the memory-based PDF documents.
   It also unsets any encryption properties, previously set by @ref litePDF_PrepareEncryption.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.

   @see litePDF_AddPage, litePDF_InsertPage, litePDF_UpdatePage, litePDF_FinishPage
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetPageCount(void *pctx,
                                                   unsigned int *pageCount);
/**<
   Returns count of pages in a PDF opened in the litePDF context.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pageCount [out] Count of pages.
   @return Whether succeeded.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetPageSize(void *pctx,
                                                  unsigned int pageIndex,
                                                  unsigned int *width_u,
                                                  unsigned int *height_u);
/**<
   Gets size of an existing page, in the current unit.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pageIndex Page index for which get the page size; counts from 0.
   @param width_u [out] Width of the page in the current unit.
   @param height_u [out] Height of the page in the current unit.
   @return Whether succeeded.

   @see litePDF_GetUnit
*/

#define LITEPDF_DRAW_FLAG_NONE                       (0)      /**< None draw flags */
#define LITEPDF_DRAW_FLAG_EMBED_FONTS_NONE           (1 << 4) /**< Do not embed any fonts into resulting PDF.
   @note: Fonts' subset is embeded by default, if none of the @ref LITEPDF_DRAW_FLAG_EMBED_FONTS_NONE, @ref LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE, @ref LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET,
      flags is defined; the @ref LITEPDF_DRAW_FLAG_EMBED_FONTS_NONE is to override default font embedding. The reason for this default embedding is
      due to PDF readers not showing correct fonts when they are not part of the PDF file.
   @see LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE, LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET */
#define LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE       (1 << 0) /**< Embed complete fonts into resulting PDF; @see LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET, LITEPDF_DRAW_FLAG_EMBED_FONTS_NONE */
#define LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET         (1 << 1) /**< Embed only subset of the fonts, aka used letters; this flag is used before @ref LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE; @see LITEPDF_DRAW_FLAG_EMBED_FONTS_NONE */
#define LITEPDF_DRAW_FLAG_SUBSTITUTE_FONTS           (1 << 2) /**< Substitute fonts with base PDF fonts, if possible */
#define LITEPDF_DRAW_FLAG_COMPRESS_IMAGES_WITH_JPEG  (1 << 3) /**< Compress images with JPEG compression, to get smaller PDF document; this is used only for RGB images */

HDC __stdcall LITEPDF_PUBLIC litePDF_AddPage(void *pctx,
                                             unsigned int width_u,
                                             unsigned int height_u,
                                             unsigned int width_px,
                                             unsigned int height_px,
                                             unsigned int drawFlags);
/**<
   Begins drawing into a new page into the PDF context of the given size.
   Newly created page is added as the last page of the PDF document.
   This cannot be called when other drawing is in progress.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param width_u Width of the new page in the current unit.
   @param height_u Height of the new page in the current unit.
   @param width_px Width of the new page in pixels.
   @param height_px Height of the new page in pixels.
   @param drawFlags Flags for drawing functions. This is a bit-or of LITEPDF_DRAW_FLAG values
      and influences only @ref litePDF_AddPage, @ref litePDF_InsertPage, @ref litePDF_UpdatePage
      and @ref litePDF_AddResource functions.
   @return Device context into which can be drawn with standard GDI functions.
      Finish the drawing, and the page addition, with @ref litePDF_FinishPage.

   @note Larger pixel page size produces more precise font mapping.

   @see litePDF_GetUnit, litePDF_InsertPage, litePDF_UpdatePage, litePDF_FinishPage, litePDF_AddResource
*/

HDC __stdcall LITEPDF_PUBLIC litePDF_InsertPage(void *pctx,
                                                unsigned int pageIndex,
                                                unsigned int width_u,
                                                unsigned int height_u,
                                                unsigned int width_px,
                                                unsigned int height_px,
                                                unsigned int drawFlags);
/**<
   Begins drawing into a new page into the  PDF context of the given size.
   Newly created page is inserted at the given position of the PDF document.
   This cannot be called when other drawing is in progress.
   If the index is out of bounds, then the page is added ad the end, like with
   @ref litePDF_AddPage.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pageIndex Page index where to insert the page; counts from 0.
   @param width_u Width of the new page in the current unit.
   @param height_u Height of the new page in the current unit.
   @param width_px Width of the new page in pixels.
   @param height_px Height of the new page in pixels.
   @param drawFlags Flags for drawing functions. This is a bit-or of LITEPDF_DRAW_FLAG values
      and influences only @ref litePDF_AddPage, @ref litePDF_InsertPage, @ref litePDF_UpdatePage
      and @ref litePDF_AddResource functions.
   @return Device context into which can be drawn with standard GDI functions.
      Finish the drawing, and the page insertion, with @ref litePDF_FinishPage.

   @note Larger pixel page size produces more precise font mapping.

   @see litePDF_GetUnit, litePDF_GetPageCount, litePDF_AddPage, litePDF_UpdatePage,
      litePDF_FinishPage, litePDF_AddResource
*/

HDC __stdcall LITEPDF_PUBLIC litePDF_UpdatePage(void *pctx,
                                                unsigned int pageIndex,
                                                unsigned int width_px,
                                                unsigned int height_px,
                                                unsigned int drawFlags);
/**<
   Begins drawing into an already existing page. The page size in the current unit can be
   found by @ref litePDF_GetPageSize. The function fails, and returns NULL,
   if the @a pageIndex is out of bounds.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pageIndex Page index which to update; counts from 0.
   @param width_px Width of the new page in pixels.
   @param height_px Height of the new page in pixels.
   @param drawFlags Flags for drawing functions. This is a bit-or of LITEPDF_DRAW_FLAG values
      and influences only @ref litePDF_AddPage, @ref litePDF_InsertPage, @ref litePDF_UpdatePage
      and @ref litePDF_AddResource functions.
   @return Device context into which can be drawn with standard GDI functions.
      Finish the drawing, and the page update, with @ref litePDF_FinishPage.

   @see litePDF_GetPageCount, litePDF_AddPage, litePDF_InsertPage,
      litePDF_FinishPage, litePDF_AddResource
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_FinishPage(void *pctx,
                                                 HDC hDC);
/**<
   Tells litePDF that drawing into the page is finished and that it can
   be processed into PDF. The @a hDC is not valid after this call any more.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param hDC Device context previously returned by @ref litePDF_AddPage,
      @ref litePDF_InsertPage or @ref litePDF_UpdatePage.
   @return Whether succeeded.
*/

HDC __stdcall LITEPDF_PUBLIC litePDF_AddResource(void *pctx,
                                                 unsigned int width_u,
                                                 unsigned int height_u,
                                                 unsigned int width_px,
                                                 unsigned int height_px,
                                                 unsigned int drawFlags);
/**<
   Begins drawing into a new resource into the PDF context of the given size.
   This cannot be called when other drawing is in progress.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param width_u Width of the new page in the current unit.
   @param height_u Height of the new page in the current unit.
   @param width_px Width of the new page in pixels.
   @param height_px Height of the new page in pixels.
   @param drawFlags Flags for drawing functions. This is a bit-or of LITEPDF_DRAW_FLAG values
      and influences only @ref litePDF_AddPage, @ref litePDF_InsertPage, @ref litePDF_UpdatePage
      and @ref litePDF_AddResource functions.
   @return Device context into which can be drawn with standard GDI functions.
      Finish the drawing, and the resource addition, with @ref litePDF_FinishResource.

   @note Larger pixel resource size produces more precise font mapping.

   @see litePDF_GetUnit, litePDF_AddPage, litePDF_InsertPage, litePDF_UpdatePage,
      litePDF_FinishResource, litePDF_DrawResource
*/

unsigned int __stdcall LITEPDF_PUBLIC litePDF_FinishResource(void *pctx,
                                                             HDC hDC);
/**<
   Tells litePDF that drawing into the resource is finished and that it can
   be processed into PDF. The @a hDC is not valid after this call any more.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param hDC Device context previously returned by @ref litePDF_AddResource.
   @return Newly created resource ID, or 0 on error.

   @see litePDF_AddResource, litePDF_AddPageFromAsResource, litePDF_DrawResource
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_DeletePage(void *pctx,
                                                 unsigned int pageIndex);
/**<
   Deletes page at given index. It doesn't delete page resources, because these can
   be used by other pages.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pageIndex Page index which to update; counts from 0.
   @return Whether succeeded.

   @see litePDF_GetPageCount, litePDF_PageToResource
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_AddPagesFrom(void *pctx,
                                                   void *pctx_from,
                                                   unsigned int pageIndex,
                                                   unsigned int pageCount);
/**<
   Adds existing pages as the last pages from another PDF. Both contexts should
   hold memory-based documents.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext,
      to which add the page.
   @param pctx_from litePDF context, previously created with @ref litePDF_CreateContext,
      from which add the pages.
   @param pageIndex Page index which to add from @a pctx_from; counts from 0.
   @param pageCount How many pages to add; 0 means whole document.
   @return Whether succeeded.

   @note The two contexts cannot be the same.

   @see litePDF_GetPageCount, litePDF_InsertPageFrom, litePDF_PageToResource
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_InsertPageFrom(void *pctx,
                                                     unsigned int pageIndexTo,
                                                     void *pctx_from,
                                                     unsigned int pageIndexFrom);
/**<
   Inserts an existing page at the given index from another PDF. Both contexts should
   hold memory-based documents.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext,
      to which add the page.
   @param pageIndexTo Page index where to add the page; counts from 0. Adds page
      at the end, if out of bounds.
   @param pctx_from litePDF context, previously created with @ref litePDF_CreateContext,
      from which add the page.
   @param pageIndexFrom Page index which to add from @a pctx_from; counts from 0.
   @return Whether succeeded.

   @note The two contexts cannot be the same.

   @see litePDF_GetPageCount, litePDF_AddPagesFrom, litePDF_PageToResource
*/

unsigned int __stdcall LITEPDF_PUBLIC litePDF_AddPageFromAsResource(void *pctx,
                                                                    void *pctx_from,
                                                                    unsigned int pageIndex,
                                                                    BOOL useTrimBox);
/**<
   Adds an existing page as a resource of a given PDF. This resource can be
   referenced multiple times by its identifier. Both contexts should
   hold memory-based documents.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext,
      to which add the page resource.
   @param pctx_from litePDF context, previously created with @ref litePDF_CreateContext,
      from which add the page.
   @param pageIndex Page index which to add from @a pctx_from; counts from 0.
   @param useTrimBox If TRUE, try to use trimbox for size of the resource (XObject)
   @return Resource identifier, or 0 on error.

   @note The two contexts cannot be the same.

   @see litePDF_GetPageCount, litePDF_AddPagesFrom, litePDF_PageToResource, litePDF_GetResourceSize,
      litePDF_DrawResource
*/

unsigned int __stdcall LITEPDF_PUBLIC litePDF_PageToResource(void *pctx,
                                                             unsigned int pageIndex);
/**<
   Creates a resource, which will reference an existing page.
   The page itself is not deleted after call of this.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pageIndex Page index for which create the resource reference; counts from 0.
   @return Resource identifier, or 0 on error.
   
   @see litePDF_GetPageCount, litePDF_AddPagesFrom, litePDF_AddPageFromAsResource, litePDF_GetResourceSize,
      litePDF_DrawResource
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetResourceSize(void *pctx,
                                                      unsigned int resourceID,
                                                      unsigned int *width_u,
                                                      unsigned int *height_u);
/**<
   Gets size of an existing resource, in the current unit. The resource ID
   was returned from @ref litePDF_AddPageFromAsResource or @ref litePDF_FinishResource.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param resourceID Resource ID for which get the size.
   @param width_u [out] Width of the resource, in the current unit.
   @param height_u [out] Height of the resource, in the current unit.
   @return Whether succeeded.

   @see litePDF_GetUnit, litePDF_AddPageFromAsResource, litePDF_DrawResource
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_DrawResource(void *pctx,
                                                   unsigned int resourceID,
                                                   unsigned int pageIndex,
                                                   unsigned int unitValue,
                                                   int x,
                                                   int y,
                                                   int scaleX,
                                                   int scaleY);
/**<
   Draws an existing resource at the given position. The resource ID 
   was returned from @ref litePDF_AddPageFromAsResource, @ref litePDF_PageToResource
   or @ref litePDF_FinishResource. The @a unitValue is used for both the position
   and the scale. In case of the scale, it defines only the ratio to the base unit.
   For example, if the @a unitValue is either @ref LITEPDF_UNIT_1000TH_MM or
   @ref LITEPDF_UNIT_1000TH_INCH, then the ratio for the @a scaleX and @a scaleY
   is used 1/1000 (where 1000 means the same size as the resource is in this case).

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param resourceID Resource ID to draw.
   @param pageIndex Page index to which draw; counts from 0.
   @param unitValue A unit to use for the @a x and @a y, and a ratio for the @a scaleX and @a scaleY.
   @param x Where to draw on the page, X axes, in the given @a unitValue unit,
      with left-top corner being [0,0].
   @param y Where to draw on the page, Y axes, in the given @a unitValue unit,
      with left-top corner being [0,0].
   @param scaleX Scale factor of the page for the X axes, using the @a unitValue ratio.
   @param scaleY Scale factor of the page for the Y axes, using the @a unitValue ratio.
   @return Whether succeeded.

   @see litePDF_GetPageCount, litePDF_AddPageFromAsResource, litePDF_PageToResource, litePDF_FinishResource,
      litePDF_GetResourceSize, litePDF_DrawResourceWithMatrix
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_DrawResourceWithMatrix(void *pctx,
                                                             unsigned int resourceID,
                                                             unsigned int pageIndex,
                                                             int a,
                                                             int b,
                                                             int c,
                                                             int d,
                                                             int e,
                                                             int f);
/**<
   Draws an existing resource with given transformation matrix. All
   the transformation values are passed into PDF directly, without any
   conversion. The resource ID was returned from @ref litePDF_AddPageFromAsResource
   or @ref litePDF_FinishResource. The constructed transformation matrix
   is a standard 3x3 matrix:<BR>
   <CODE>   | a b 0 |</CODE><BR>
   <CODE>   | c d 0 |</CODE><BR>
   <CODE>   | e f 1 |</CODE>

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param resourceID Resource ID to draw.
   @param pageIndex Page index to which draw; counts from 0.
   @param a Transformation matrix [ a b c d e f ] parameter 'a', in 1/1000th of PDF units.
   @param b Transformation matrix [ a b c d e f ] parameter 'b', in 1/1000th of PDF units.
   @param c Transformation matrix [ a b c d e f ] parameter 'c', in 1/1000th of PDF units.
   @param d Transformation matrix [ a b c d e f ] parameter 'd', in 1/1000th of PDF units.
   @param e Transformation matrix [ a b c d e f ] parameter 'e', in 1/1000th of PDF units.
   @param f Transformation matrix [ a b c d e f ] parameter 'f', in 1/1000th of PDF units.
   @return Whether succeeded.

   @see litePDF_GetPageCount, litePDF_AddPageFromAsResource, litePDF_FinishResource,
      litePDF_GetResourceSize, litePDF_DrawResource
*/

#define LITEPDF_DOCUMENT_INFO_AUTHOR            "Author"       /**< an Author of the document */
#define LITEPDF_DOCUMENT_INFO_CREATOR           "Creator"      /**< a Creator of the document */
#define LITEPDF_DOCUMENT_INFO_KEYWORDS          "Keywords"     /**< the Keywords of the document */
#define LITEPDF_DOCUMENT_INFO_SUBJECT           "Subject"      /**< a Subject of the document */
#define LITEPDF_DOCUMENT_INFO_TITLE             "Title"        /**< a Title of the document */
#define LITEPDF_DOCUMENT_INFO_PRODUCER          "Producer"     /**< a Producer of the document; this key is read-only */
#define LITEPDF_DOCUMENT_INFO_TRAPPED           "Trapped"      /**< a trapping state of the document */
#define LITEPDF_DOCUMENT_INFO_CREATION_DATE     "CreationDate" /**< a date of the creation of the document */
#define LITEPDF_DOCUMENT_INFO_MODIFICATION_DATE "ModDate"      /**< a date of the last modification of the document */

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetDocumentInfo(void *pctx,
                                                      const char *name,
                                                      const wchar_t *value);
/**<
   Sets information about the document. The name can be one
   of the LITEPDF_DOCUMENT_INFO predefined constants.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param name Document info property name to set.
   @param value Null-terminated Unicode value to set.
   @return Whether succeeded.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetDocumentInfoExists(void *pctx,
                                                            const char *name,
                                                            BOOL *pExists);
/**<
   Checks whether information about the document of the given name exists.
   The name can be one of the LITEPDF_DOCUMENT_INFO predefined constants.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param name Document info property name to test.
   @param pExists [out] Where to store whether the document information is set.
   @return Whether succeeded.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetDocumentInfo(void *pctx,
                                                      const char *name,
                                                      wchar_t *value,
                                                      unsigned int *valueLength);
/**<
   Gets information about the document. The name can be one
   of the LITEPDF_DOCUMENT_INFO predefined constants.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param name Document info property name to get.
   @param value [out] Null-terminated Unicode value. It can be NULL, in which case
      the @a valueLength is populated with large-enough value to hold the whole string.
   @param valueLength [in/out] Tells how many characters can be stored in @a value. If @a value
      is NULL, then it is set to large-enough value (not including the Null-termination character).
      Passing non-NULL @a value with no enough large buffer results in a failure with no change
      on @a valueLength. The @a valueLength has stored count of characters written, not including
      the Null-termination character on success.
   @return Whether succeeded.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetDocumentIsSigned(void *pctx,
                                                          BOOL *pIsSigned);
/**<
   Checks whether currently opened document is already signed. Signing already
   signed document can cause breakage of previous signatures, thus it's good
   to test whether the loaded document is signed, before signing it.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pIsSigned [out] Where to store the result.
   @return Whether succeeded.

   @see litePDF_GetSignatureCount, litePDF_SaveToFileWithSign, litePDF_SaveToDataWithSign
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureCount(void *pctx,
                                                        unsigned int *pCount);
/**<
   Provides how many signature fields the currently opened document contains.
   It returns the count of the all fields, not only those already signed.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pCount [out] Where to store the result.
   @return Whether succeeded.

   @note The litePDF caches the list of the existing signature fields for performance
      reasons and it rebuilds it whenever this function is called or when the
      @ref litePDF_CreateSignature is called, thus if there are made any changes
      directly with the PoDoFo API after the cache had been created, then make sure
      you call this function again to avoid a use-after-free or an outdated information
      being used. The litePDF will try to keep the cache up to date as needed, but
      it cannot cover every case, especially not the one when the PoDoFo API is used.

   @see litePDF_GetDocumentIsSigned, litePDF_GetSignatureHasData, litePDF_GetSignatureData
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureName(void *pctx,
                                                       unsigned int index,
                                                       char *name,
                                                       unsigned int *nameLength);
/**<
   Gets the signature field name at the given @a index.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature field name to get; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param name [out] Null-terminated ASCII name. It can be NULL, in which case
      the @a nameLength is populated with large-enough value to hold the whole string.
   @param nameLength [in/out] Tells how many characters can be stored in @a name. If @a name
      is NULL, then it is set to large-enough value (not including the Null-termination character).
      Passing non-NULL @a name with no enough large buffer results in a failure with no change
      on @a nameLength. The @a nameLength has stored count of characters written, not including
      the Null-termination character, on success.
   @return Whether succeeded.
*/

#define LITEPDF_ANNOTATION_FLAG_NONE           0x0000 /**< Default annotation flags */
#define LITEPDF_ANNOTATION_FLAG_INVISIBLE      0x0001 /**< Do not show nonstandard annotation if there is no annotation handler available */
#define LITEPDF_ANNOTATION_FLAG_HIDDEN         0x0002 /**< Do not allow show, print or interact with the annotation */
#define LITEPDF_ANNOTATION_FLAG_PRINT          0x0004 /**< Print the annotation */
#define LITEPDF_ANNOTATION_FLAG_NOZOOM         0x0008 /**< Do not scale the annotation's appearance to match the magnification of the page */
#define LITEPDF_ANNOTATION_FLAG_NOROTATE       0x0010 /**< Do not rotate the annotation's appearance to match the rotation of the page */
#define LITEPDF_ANNOTATION_FLAG_NOVIEW         0x0020 /**< Do not display the annotation on the screen or allow it to interact with the user */
#define LITEPDF_ANNOTATION_FLAG_READONLY       0x0040 /**< Do not allow the annotation to interact with the user */
#define LITEPDF_ANNOTATION_FLAG_LOCKED         0x0080 /**< Do not allow the annotation to be deleted or its properties (including position and size) to be modified by the user */
#define LITEPDF_ANNOTATION_FLAG_TOGGLENOVIEW   0x0100 /**< Invert the interpretation of the NoView flag for certain events */
#define LITEPDF_ANNOTATION_FLAG_LOCKEDCONTENTS 0x0200 /**< Do not allow the contents of the annotation to be modified by the user */

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateSignature(void *pctx,
                                                      const char *name,
                                                      unsigned int annotationPageIndex,
                                                      int annotationX_u,
                                                      int annotationY_u,
                                                      int annotationWidth_u,
                                                      int annotationHeight_u,
                                                      unsigned int annotationFlags,
                                                      unsigned int *pAddedIndex);
/**<
   Creates a new signature field named @a name. The field is created completely empty.
   Use @ref litePDF_SetSignatureDate, @ref litePDF_SetSignatureReason,
   @ref litePDF_SetSignatureLocation, @ref litePDF_SetSignatureCreator,
   @ref litePDF_SetSignatureAppearance and such to populated it with required values.
   Finally, to sign the signature field use @ref litePDF_SaveToFileWithSign family
   functions.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param name Signature field name to use. This should be unique.
   @param annotationPageIndex Page index where to place the signature annotation.
   @param annotationX_u X-origin of the annotation on the page, in the current unit.
   @param annotationY_u Y-origin of the annotation on the page, in the current unit.
   @param annotationWidth_u Width of the annotation on the page, in the current unit.
   @param annotationHeight_u Height of the annotation on the page, in the current unit.
   @param annotationFlags Bit-or of LITEPDF_ANNOTATION_FLAG_ flags.
   @param pAddedIndex [out] Optional parameter, if not NULL, then it's populated with
      the index of the added signature field.
   @return Whether succeeded. The call can also fail when a signature field of the same
      name already exists.

   @see litePDF_GetSignatureCount, litePDF_GetSignatureName
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureHasData(void *pctx,
                                                          unsigned int index,
                                                          BOOL *pHasData);
/**<
   Checks whether the given signature field contains any data, which
   means whether the signature field is signed.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature data to get; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param pHasData [out] Set to TRUE, when the given signature contains any data.
   @return Whether succeeded.

   @see litePDF_GetSignatureData
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureData(void *pctx,
                                                       unsigned int index,
                                                       BYTE *data,
                                                       unsigned int *dataLength);
/**<
   Gathers raw signature data for the given signature in the currently opened document.
   Use @ref litePDF_GetSignatureHasData to check whether the given signature field
   is signed or not.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature data to get; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param data [out] Actual data to store the signature content to. It can be NULL, in which case
      the @a dataLength is populated with large-enough value to hold the whole data.
   @param dataLength [in/out] Tells how many bytes can be stored in @a data. If @a data
      is NULL, then it is set to large-enough value. Passing non-NULL @a data with no enough
      large buffer results in a failure with no change on @a dataLength.
   @return Whether succeeded.

   @see litePDF_GetDocumentIsSigned, litePDF_GetSignatureCount, litePDF_GetSignatureRanges
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureRanges(void *pctx,
                                                         unsigned int index,
                                                         unsigned __int64 *pRangesArray,
                                                         unsigned int *pRangesArrayLength);
/**<
   Gathers signature ranges, that is the actual offsets into the opened file
   which had been used to create the signature data (@ref litePDF_GetSignatureData).
   The array is a pair of numbers, where the first number is an offset into the file
   from its beginning and the second number is the number of bytes being used for
   the signature from this offset.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature ranges to get; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param pRangesArray [out] Actual array to store the signature ranges to. It can be NULL,
      in which case the @a pRangesArrayLength is populated with large-enough value to hold
      the whole array.
   @param pRangesArrayLength [in/out] Tells how many items can be stored in @a pRangesArray.
      If @a pRangesArray is NULL, then it is set to large-enough value. Passing non-NULL
      @a pRangesArray with no enough large array results in a failure with no change
      on @a pRangesArrayLength.
   @return Whether succeeded.

   @note This function works only for the signatures which use this kind of signature method.

   @see litePDF_GetDocumentIsSigned, litePDF_GetSignatureCount, litePDF_GetSignatureData
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureDate(void *pctx,
                                                       unsigned int index,
                                                       __int64 dateOfSign);
/**<
   Sets signature field date of sign.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param dateOfSign Date of sign, like Unix time_t, when the signature was created; less than
      or equal to 0 means today. The value can be clamp on 32-bit systems.
   @return Whether succeeded.

   @see litePDF_GetSignatureDate, litePDF_GetSignatureCount
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureDate(void *pctx,
                                                       unsigned int index,
                                                       __int64 *pDateOfSign);
/**<
   Gets signature field date of sign.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param pDateOfSign [out] A variable where to store the date of sign. It's like Unix time_t,
      as set by the signature field creator. The value can be clamp on 32-bit systems.
   @return Whether succeeded.

   @see litePDF_SetSignatureDate, litePDF_GetSignatureCount
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureReason(void *pctx,
                                                         unsigned int index,
                                                         const wchar_t *reason);
/**<
   Sets signature reason.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param reason The value to set.
   @return Whether succeeded.

   @see litePDF_GetSignatureReason, litePDF_GetSignatureCount
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureReason(void *pctx,
                                                         unsigned int index,
                                                         wchar_t *value,
                                                         unsigned int *valueLength);
/**<
   Gets signature reason.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param value [out] Null-terminated Unicode value. It can be NULL, in which case
      the @a valueLength is populated with large-enough value to hold the whole string.
   @param valueLength [in/out] Tells how many characters can be stored in @a value. If @a value
      is NULL, then it is set to large-enough value (not including the Null-termination character).
      Passing non-NULL @a value with no enough large buffer results in a failure with no change
      on @a valueLength. The @a valueLength has stored count of characters written, not including
      the Null-termination character on success.
   @return Whether succeeded.

   @see litePDF_SetSignatureReason, litePDF_GetSignatureCount
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureLocation(void *pctx,
                                                           unsigned int index,
                                                           const wchar_t *location);
/**<
   Sets signature location, aka where the signature had been made. This can be left unset.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param location The value to set.
   @return Whether succeeded.

   @see litePDF_GetSignatureLocation, litePDF_GetSignatureCount
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureLocation(void *pctx,
                                                           unsigned int index,
                                                           wchar_t *value,
                                                           unsigned int *valueLength);
/**<
   Gets signature location.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param value [out] Null-terminated Unicode value. It can be NULL, in which case
      the @a valueLength is populated with large-enough value to hold the whole string.
   @param valueLength [in/out] Tells how many characters can be stored in @a value. If @a value
      is NULL, then it is set to large-enough value (not including the Null-termination character).
      Passing non-NULL @a value with no enough large buffer results in a failure with no change
      on @a valueLength. The @a valueLength has stored count of characters written, not including
      the Null-termination character on success.
   @return Whether succeeded.

   @see litePDF_SetSignatureLocation, litePDF_GetSignatureCount
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureCreator(void *pctx,
                                                          unsigned int index,
                                                          const char *creator);
/**<
   Sets signature creator. This can be left unset.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param creator The value to set.
   @return Whether succeeded.

   @see litePDF_GetSignatureCreator, litePDF_GetSignatureCount
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureCreator(void *pctx,
                                                          unsigned int index,
                                                          char *value,
                                                          unsigned int *valueLength);
/**<
   Gets signature creator.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param value [out] Null-terminated ASCII value. It can be NULL, in which case
      the @a valueLength is populated with large-enough value to hold the whole string.
   @param valueLength [in/out] Tells how many characters can be stored in @a value. If @a value
      is NULL, then it is set to large-enough value (not including the Null-termination character).
      Passing non-NULL @a value with no enough large buffer results in a failure with no change
      on @a valueLength. The @a valueLength has stored count of characters written, not including
      the Null-termination character on success.
   @return Whether succeeded.

   @see litePDF_SetSignatureCreator, litePDF_GetSignatureCount
*/

#define LITEPDF_APPEARANCE_NORMAL   (0) /**< Normal appearance */
#define LITEPDF_APPEARANCE_ROLLOVER (1) /**< Rollover appearance; the default is the normal appearance */
#define LITEPDF_APPEARANCE_DOWN     (2) /**< Down appearance; the default is the normal appearance */

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureAppearance(void *pctx,
                                                             unsigned int index,
                                                             unsigned int appearanceType,
                                                             unsigned int resourceID,
                                                             int offsetX_u,
                                                             int offsetY_u);
/**<
   Sets the signature appearance.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param appearanceType One of the @ref LITEPDF_APPEARANCE_NORMAL, @ref LITEPDF_APPEARANCE_ROLLOVER
      and @ref LITEPDF_APPEARANCE_DOWN contacts. At least the @ref LITEPDF_APPEARANCE_NORMAL type
      should be set, if the appearance of the signature is requested.
   @param resourceID An existing resource ID of the annotation content, as shown to the user.
   @param offsetX_u X-offset of the resource inside the annotation of the signature, in the current unit.
   @param offsetY_u Y-offset of the resource inside the annotation of the signature, in the current unit.
   @return Whether succeeded.

   @note The resoruce position offset is from [left, top] corner of the annotation rectangle.

   @see litePDF_GetUnit, litePDF_AddResource, litePDF_GetSignatureCount, litePDF_CreateSignature
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureSize(void *pctx,
                                                       unsigned int requestBytes);
/**<
   Sets how many bytes the signature may require. The default value is 2048 bytes
   and it is automatically adjusted when the @ref litePDF_SaveToFileWithSign or
   @ref litePDF_SaveToDataWithSign are used. The manual signing functions
   require this value to be set before signing, if the final hash with the certificate
   exceeds the default size.

   This value is remembered in general, not for any signature in particular.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param requestBytes How many bytes the signature will require.
   @return Whether succeeded.

   @see litePDF_SaveToFileWithSignManual, litePDF_SaveToFileWithSignManualW, litePDF_SaveToDataWithSignManual
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_AddSignerPFX(void *pctx,
                                                   const BYTE *pfxData,
                                                   unsigned int pfxDataLength,
                                                   const char *pfxPassword);
/**<
   Adds a signer to be used when digitally signing the document with
   @ref litePDF_SaveToFileWithSign or @ref litePDF_SaveToDataWithSign.
   The passed-in certificate is in the PFX format and should include
   the private key.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pfxData A certificate with private key in the PFX format.
   @param pfxDataLength A length of the @a pfxData.
   @param pfxPassword A password to use to open the PFX certificate; can be NULL.
   @return Whether succeeded.

   @see litePDF_AddSignerPEM
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_AddSignerPEM(void *pctx,
                                                   const BYTE *pemData,
                                                   unsigned int pemDataLength,
                                                   const BYTE *pkeyData,
                                                   unsigned int pkeyDataLength,
                                                   const char *pkeyPassword);
/**<
   Adds a signer to be used when digitally signing the document with
   @ref litePDF_SaveToFileWithSign or @ref litePDF_SaveToDataWithSign.
   The passed-in certificate and private key are in the PEM format.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param pemData A certificate in the PEM format.
   @param pemDataLength A length of the @a pemData.
   @param pkeyData A private key for the certificate, in the PEM format.
   @param pkeyDataLength A length of the @a pkeyData.
   @param pkeyPassword A password to use to open the private key; can be NULL.
   @return Whether succeeded.

   @see litePDF_AddSignerPFX
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSign(void *pctx,
                                                         const char *fileName,
                                                         unsigned int signatureIndex);
/**<
   Digitally signs a PDF document opened at the litePDF context. The caller is
   responsible to set at least one signer with either @ref litePDF_AddSignerPFX
   or @ref litePDF_AddSignerPEM first. An alternative @ref litePDF_SaveToFileWithSignManual
   is provided when it's required to compute the signature hash manually by the caller.

   In case the document had been loaded with @ref litePDF_LoadFromFile,
   @ref litePDF_LoadFromFileW or @ref litePDF_LoadFromData with its @a forUpdate
   parameter being TRUE, the resulting document will contain the changes as
   an incremental update (appended at the end of the original document), otherwise
   the whole document is completely rewritten.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param fileName A file name where to save signed PDF document.
   @param signatureIndex Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @return Whether succeeded.

   @note The only valid operation after this is either close the document
      with @ref litePDF_Close, or free the context with @ref litePDF_FreeContext.

   @note Signing already signed document can cause breakage of previous signatures, thus
      check whether the loaded document is already signed with @ref litePDF_GetDocumentIsSigned.
      Load the document with its @a forUpdate parameter set to TRUE, to sign an existing document.

   @see litePDF_SaveToFileWithSignW, litePDF_SaveToDataWithSign
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSignW(void *pctx,
                                                          const wchar_t *fileName,
                                                          unsigned int signatureIndex);
/**<
   This is the same as @ref litePDF_SaveToFileWithSign, the only difference is that
   the @a fileName is a wide string.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToDataWithSign(void *pctx,
                                                         unsigned int signatureIndex,
                                                         BYTE *data,
                                                         unsigned int *dataLength);
/**<
   Digitally signs a PDF document opened at the litePDF context. The caller is
   responsible to set at least one signer with either @ref litePDF_AddSignerPFX
   or @ref litePDF_AddSignerPEM first. An alternative @ref litePDF_SaveToDataWithSignManual
   is provided when it's required to compute the signature hash manually by the caller.

   In case the document had been loaded with @ref litePDF_LoadFromFile,
   @ref litePDF_LoadFromFileW or @ref litePDF_LoadFromData with its @a forUpdate
   parameter being TRUE, the resulting document will contain the changes as
   an incremental update (appended at the end of the original document), otherwise
   the whole document is completely rewritten.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param signatureIndex Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param data [out] Actual data to store the PDF content to. It can be NULL, in which case
      the @a dataLength is populated with large-enough value to hold the whole data.
   @param dataLength [in/out] Tells how many bytes can be stored in @a data. If @a data
      is NULL, then it is set to large-enough value. Passing non-NULL @a data with no enough
      large buffer results in a failure with no change on @a dataLength.
   @return Whether succeeded.

   @note The only valid operation after this is either call of @ref litePDF_SaveToDataWithSign again,
      to get information about necessary buffer size or data itself, close the document
      with @ref litePDF_Close, or free the context with @ref litePDF_FreeContext.

   @note Signing already signed document can cause breakage of previous signatures, thus
      check whether the loaded document is already signed with @ref litePDF_GetDocumentIsSigned.
      Load the document with its @a forUpdate parameter set to TRUE, to sign an existing document.

   @see litePDF_SaveToFileWithSign
*/

typedef void (__stdcall *litePDF_appendSignatureDataFunc)(const char *bytes,
                                                          unsigned int bytes_len,
                                                          void *user_data);
/**<
   The function is used within @ref litePDF_SaveToFileWithSignManual and @ref litePDF_SaveToDataWithSignManual.
   It is called called when more data should be added to hash computation.
*/

typedef void (__stdcall *litePDF_finishSignatureFunc)(char *signature,
                                                      unsigned int *signature_len,
                                                      void *user_data);
/**<
   The function is used within @ref litePDF_SaveToFileWithSignManual and @ref litePDF_SaveToDataWithSignManual.
   It is called when all the data are processed and the signature value is required.
   The @a signature_len contains size of the @a signature buffer. The callback is
   responsible to populate @a signature and @a signature_len with correct values.
   Set @a signature_len to zero on any error. Note the callback is called only once.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSignManual(void *pctx,
                                                               const char *fileName,
                                                               unsigned int signatureIndex,
                                                               litePDF_appendSignatureDataFunc appendSignatureData,
                                                               void *append_user_data,
                                                               litePDF_finishSignatureFunc finishSignature,
                                                               void *finish_user_data);
/**<
   Digitally signs a PDF document opened at the litePDF context. The caller is
   responsible for a detached hash computations and related certificate management.

   In case the document had been loaded with @ref litePDF_LoadFromFile,
   @ref litePDF_LoadFromFileW or @ref litePDF_LoadFromData with its @a forUpdate
   parameter being TRUE, the resulting document will contain the changes as
   an incremental update (appended at the end of the original document), otherwise
   the whole document is completely rewritten.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param fileName A file name where to save signed PDF document.
   @param signatureIndex Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param appendSignatureData Called when more data should be added to hash computation.
      The function cannot be NULL, even when called the second time, to get actual data.
   @param append_user_data User data value for the @a appendSignatureData callback.
   @param finishSignature Called when all the data are processed, and the signature
      value is required. The @a signature_len contains size of the @a signature buffer.
      The callback is responsible to populate @a signature and @a signature_len with
      correct values. Set @a signature_len to zero on any error.
      Note the callback is called only once.
      The function cannot be NULL, even when called the second time, to get actual data.
   @param finish_user_data User data value for the @a finishSignature callback.
   @return Whether succeeded.

   @note The only valid operation after this is either close the document
      with @ref litePDF_Close, or free the context with @ref litePDF_FreeContext.

   @note Signing already signed document can cause breakage of previous signatures, thus
      check whether the loaded document is already signed with @ref litePDF_GetDocumentIsSigned.
      Load the document with its @a forUpdate parameter set to TRUE, to sign an existing document.

   @see litePDF_SaveToFileWithSign, litePDF_SaveToFileWithSignManualW, litePDF_SaveToDataWithSignManual
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSignManualW(void *pctx,
                                                                const wchar_t *fileName,
                                                                unsigned int signatureIndex,
                                                                litePDF_appendSignatureDataFunc appendSignatureData,
                                                                void *append_user_data,
                                                                litePDF_finishSignatureFunc finishSignature,
                                                                void *finish_user_data);
/**<
   This is the same as @ref litePDF_SaveToFileWithSignManual, the only difference is that
   the @a fileName is a wide string.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToDataWithSignManual(void *pctx,
                                                               unsigned int signatureIndex,
                                                               litePDF_appendSignatureDataFunc appendSignatureData,
                                                               void *append_user_data,
                                                               litePDF_finishSignatureFunc finishSignature,
                                                               void *finish_user_data,
                                                               BYTE *data,
                                                               unsigned int *dataLength);
/**<
   Digitally signs a PDF document opened at the litePDF context. The caller is
   responsible for a detached hash computations and related certificate management.

   In case the document had been loaded with @ref litePDF_LoadFromFile,
   @ref litePDF_LoadFromFileW or @ref litePDF_LoadFromData with its @a forUpdate
   parameter being TRUE, the resulting document will contain the changes as
   an incremental update (appended at the end of the original document), otherwise
   the whole document is completely rewritten.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param signatureIndex Which signature to use; counts from 0. This might be less
      than @ref litePDF_GetSignatureCount.
   @param appendSignatureData Called when more data should be added to hash computation.
      The function cannot be NULL, even when called the second time, to get actual data.
   @param append_user_data User data value for the @a appendSignatureData callback.
   @param finishSignature Called when all the data are processed, and the signature
      value is required. The @a signature_len contains size of the @a signature buffer.
      The callback is responsible to populate @a signature and @a signature_len with
      correct values. Set @a signature_len to zero on any error.
      Note the callback is called only once.
      The function cannot be NULL, even when called the second time, to get actual data.
   @param finish_user_data User data value for the @a finishSignature callback.
   @param data [out] Actual data to store the PDF content to. It can be NULL, in which case
      the @a dataLength is populated with large-enough value to hold the whole data.
   @param dataLength [in/out] Tells how many bytes can be stored in @a data. If @a data
      is NULL, then it is set to large-enough value. Passing non-NULL @a data with no enough
      large buffer results in a failure with no change on @a dataLength.
   @return Whether succeeded.

   @note The only valid operation after this is either call of @ref litePDF_SaveToDataWithSignManual again,
      to get information about necessary buffer size or data itself, close the document
      with @ref litePDF_Close, or free the context with @ref litePDF_FreeContext.

   @note Signing already signed document can cause breakage of previous signatures, thus
      check whether the loaded document is already signed with @ref litePDF_GetDocumentIsSigned.
      Load the document with its @a forUpdate parameter set to TRUE, to sign an existing document.

   @see litePDF_SaveToFileWithSignManual, litePDF_SaveToFileWithSign
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedFile(void *pctx,
                                                const char *fileName);
/**<
   Embeds a file into a PDF document.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param fileName File name of the file to be attached.
   @return Whether succeeded.

   @note Files can be embed only to memory-based documents.

   @note The path is stripped from the @a fileName. The @a fileName is used as a key,
      aka it's not possible to embed two files of the same name into a PDF document.

   @see litePDF_EmbedFileW, litePDF_EmbedData, litePDF_CreateMemDocument
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedFileW(void *pctx,
                                                 const wchar_t *fileName);
/**<
   This is the same as @ref litePDF_EmbedFile, the only difference is that
   the @a fileName is a wide string.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedData(void *pctx,
                                                const char *fileName,
                                                const BYTE *data,
                                                unsigned int dataLength);
/**<
   Embeds a data (file) into a PDF document.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param fileName File name to be used for the data identification.
   @param data Actual data to be attached.
   @param dataLength Length of the data.
   @return Whether succeeded.

   @note Data can be embed only to memory-based documents.

   @note The path is stripped from the @a fileName. The @a fileName is used as a key,
      aka it's not possible to embed two files of the same name into a PDF document.

   @see litePDF_EmbedDataW, litePDF_EmbedFile, litePDF_CreateMemDocument
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedDataW(void *pctx,
                                                 const wchar_t *fileName,
                                                 const BYTE *data,
                                                 unsigned int dataLength);
/**<
   This is the same as @ref litePDF_EmbedData, the only difference is that
   the @a fileName is a wide string.
*/


INT __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileCount(void *pctx);
/**<
   Gets count of embedded files stored in a PDF document.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @return Count of found embedded files, or -1 on error.

   @see litePDF_EmbedFile, litePDF_EmbedData, litePDF_GetEmbeddedFileName,
      litePDF_GetEmbeddedFileData
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileName(void *pctx,
                                                          unsigned int index,
                                                          char *fileName,
                                                          unsigned int *fileNameLength);
/**<
   Gets embedded file's name, as stored in a PDF document.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Index of the embedded file; returns failure, if out of range.
   @param fileName [out] Buffer to store the embedded file's name to. It can be NULL,
      in which case the @a fileNameLength is populated with large-enough value to hold
      the whole string.
   @param fileNameLength [in/out] Tells how many characters can be stored in @a fileName.
      If @a fileName is NULL, then it is set to large-enough value. Passing
      non-NULL @a fileName with no enough large buffer results in a failure with
      no change on @a fileNameLength. The set length is without the Null-terminate character.
   @return Whether succeeded.

   @see litePDF_GetEmbeddedFileNameW, litePDF_EmbedFile, litePDF_EmbedData, litePDF_GetEmbeddedFileCount,
      litePDF_GetEmbeddedFileData
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileNameW(void *pctx,
                                                           unsigned int index,
                                                           wchar_t *fileName,
                                                           unsigned int *fileNameLength);
/**<
   This is the same as @ref litePDF_GetEmbeddedFileName, the only difference is that
   the @a fileName is a wide string.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileData(void *pctx,
                                                          unsigned int index,
                                                          BYTE *data,
                                                          unsigned int *dataLength);
/**<
   Gets embedded file's data, as stored in a PDF document. There are no data returned,
   if the file was not embed.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext.
   @param index Index of the embedded file; returns failure, if out of range.
   @param data [out] Actual embedded file's data, as stored in the PDF. It can be NULL, in which case
      the @a dataLength is populated with large-enough value to hold the whole data.
   @param dataLength [in/out] Tells how many bytes can be stored in @a data. If @a data
      is NULL, then it is set to large-enough value. Passing non-NULL @a data with no enough
      large buffer results in a failure with no change on @a dataLength.
   @return Whether succeeded.

   @see litePDF_EmbedFile, litePDF_EmbedData, litePDF_GetEmbeddedFileCount,
      litePDF_GetEmbeddedFileName
*/

void * __stdcall LITEPDF_PUBLIC litePDF_GetPoDoFoDocument(void *pctx);
/**<
   Gets a pointer to PoDoFo::PdfDocument document, which is currently opened.
   The returned pointer is owned by litePDF, do not free it. It is valid until
   the document is closed.

   @return Pointer to currently opened PoDoFo::PdfDocument.

   @see litePDF_Close
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_DrawDebugPage(void *pctx,
                                                    const char *filename);
/**<
   Draws saved debugPage as a new page into the PDF file. There should not be
   running any drawing when calling this function (like no page can be opened
   for drawing).

   @param pctx litePDF context, previously created with @ref litePDF_CreateContext.
   @param filename File name with full path for litePDF debug page.
*/

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateLinkAnnotation(void *pctx,
                                                           unsigned int annotationPageIndex,
                                                           int annotationX_u,
                                                           int annotationY_u,
                                                           int annotationWidth_u,
                                                           int annotationHeight_u,
                                                           unsigned int annotationFlags,
                                                           unsigned int annotationResourceID,
                                                           unsigned int destinationPageIndex,
                                                           unsigned int destinationX_u,
                                                           unsigned int destinationY_u,
                                                           const wchar_t *destinationDescription);
/**<
   Creates a link annotation at the given page and position, which will target the given
   destination page and the position in it. The context should hold a memory-based document.
   Note, the link annotation can be created only when the document is not drawing, to
   have all the document pages available.

   @param pctx a litePDF context, previously created with @ref litePDF_CreateContext,
      to which add the link annotation.
   @param annotationPageIndex Page index where to place the link annotation.
   @param annotationX_u X-origin of the annotation on the page, in the current unit.
   @param annotationY_u Y-origin of the annotation on the page, in the current unit.
   @param annotationWidth_u Width of the annotation on the page, in the current unit.
   @param annotationHeight_u Height of the annotation on the page, in the current unit.
   @param annotationFlags Bit-or of LITEPDF_ANNOTATION_FLAG_ flags.
   @param annotationResourceID Optional resource ID of the annotation content, as shown
      to the user. 0 means do not add additional visualization on the page, but the annotation
      can be still clicked.
   @param destinationPageIndex Page index where the link points to.
   @param destinationX_u X-origin of the destination on the page, in the current unit.
   @param destinationY_u Y-origin of the destination on the page, in the current unit.
   @param destinationDescription Optional destination description, which can be used
      for accessibility reasons by the viewer.
   @return Whether succeeded.

   @see litePDF_GetUnit, litePDF_GetPageCount, litePDF_AddResource, litePDF_CreateBookmarkRoot
*/

#define LITEPDF_BOOKMARK_FLAG_NONE      0x0000 /**< Default bookmark flags */
#define LITEPDF_BOOKMARK_FLAG_ITALIC    0x0001 /**< Show bookmark title as an italic text */
#define LITEPDF_BOOKMARK_FLAG_BOLD      0x0002 /**< Show bookmark title as a bold text */

unsigned int __stdcall LITEPDF_PUBLIC litePDF_CreateBookmarkRoot(void *pctx,
                                                                 const wchar_t *title,
                                                                 unsigned int flags,
                                                                 unsigned char titleColor_red,
                                                                 unsigned char titleColor_green,
                                                                 unsigned char titleColor_blue,
                                                                 unsigned int destinationPageIndex,
                                                                 unsigned int destinationX_u,
                                                                 unsigned int destinationY_u);
/**<
   Creates a new root (top-level) bookmark, which will target the given destination
   page and the position in it. The context should hold a memory-based document.
   Note, the bookmarks can be created only when the document is not drawing, to
   have all the document pages available.

   @param pctx A litePDF context, previously created with @ref litePDF_CreateContext,
      to which add the link annotation.
   @param title Title of the bookmark.
   @param flags Bit-or of LITEPDF_BOOKMARK_FLAG_ flags.
   @param titleColor_red RGB red value of the title text color.
   @param titleColor_green RGB green value of the title text color.
   @param titleColor_blue RGB blue value of the title text color.
   @param destinationPageIndex Page index where the link points to.
   @param destinationX_u X-origin of the destination on the page, in the current unit.
   @param destinationY_u Y-origin of the destination on the page, in the current unit.
   @return Created bookmark ID or 0, when the bookmark could not be created.

   @see litePDF_GetUnit, litePDF_CreateBookmarkChild, litePDF_CreateBookmarkSibling, litePDF_CreateLinkAnnotation
*/

unsigned int __stdcall LITEPDF_PUBLIC litePDF_CreateBookmarkChild(void *pctx,
                                                                  unsigned int parentBookmarkID,
                                                                  const wchar_t *title,
                                                                  unsigned int flags,
                                                                  unsigned char titleColor_red,
                                                                  unsigned char titleColor_green,
                                                                  unsigned char titleColor_blue,
                                                                  unsigned int destinationPageIndex,
                                                                  unsigned int destinationX_u,
                                                                  unsigned int destinationY_u);
/**<
   Creates a new child bookmark, which will target the given destination
   page and the position in it. The context should hold a memory-based document.
   Note, the bookmarks can be created only when the document is not drawing, to
   have all the document pages available.

   @param pctx A litePDF context, previously created with @ref litePDF_CreateContext,
      to which add the link annotation.
   @param parentBookmarkID Bookmark ID of the parent bookmark. The child will be
      created under this bookmark.
   @param title Title of the bookmark.
   @param flags Bit-or of LITEPDF_BOOKMARK_FLAG_ flags.
   @param titleColor_red RGB red value of the title text color.
   @param titleColor_green RGB green value of the title text color.
   @param titleColor_blue RGB blue value of the title text color.
   @param destinationPageIndex Page index where the link points to.
   @param destinationX_u X-origin of the destination on the page, in the current unit.
   @param destinationY_u Y-origin of the destination on the page, in the current unit.
   @return Created bookmark ID or 0, when the bookmark could not be created.

   @see litePDF_GetUnit, litePDF_CreateBookmarkRoot, litePDF_CreateBookmarkSibling, litePDF_CreateLinkAnnotation
*/

unsigned int __stdcall LITEPDF_PUBLIC litePDF_CreateBookmarkSibling(void *pctx,
                                                                    unsigned int previousBookmarkID,
                                                                    const wchar_t *title,
                                                                    unsigned int flags,
                                                                    unsigned char titleColor_red,
                                                                    unsigned char titleColor_green,
                                                                    unsigned char titleColor_blue,
                                                                    unsigned int destinationPageIndex,
                                                                    unsigned int destinationX_u,
                                                                    unsigned int destinationY_u);
/**<
   Creates a new sibling (next) bookmark, which will target the given destination
   page and the position in it. The context should hold a memory-based document.
   Note, the bookmarks can be created only when the document is not drawing, to
   have all the document pages available.

   @param pctx A litePDF context, previously created with @ref litePDF_CreateContext,
      to which add the link annotation.
   @param previousBookmarkID Bookmark ID of the previous bookmark. The sibling will be
      created as the next of this bookmark.
   @param title Title of the bookmark.
   @param flags Bit-or of LITEPDF_BOOKMARK_FLAG_ flags.
   @param titleColor_red RGB red value of the title text color.
   @param titleColor_green RGB green value of the title text color.
   @param titleColor_blue RGB blue value of the title text color.
   @param destinationPageIndex Page index where the link points to.
   @param destinationX_u X-origin of the destination on the page, in the current unit.
   @param destinationY_u Y-origin of the destination on the page, in the current unit.
   @return Created bookmark ID or 0, when the bookmark could not be created.

   @see litePDF_GetUnit, litePDF_CreateBookmarkRoot, litePDF_CreateBookmarkChild, litePDF_CreateLinkAnnotation
*/

LITEPDF_END_DECLS

#endif /* LITEPDF_H */
