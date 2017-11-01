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

// Silence some annoying warnings from Visual Studio
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

#include "litePDF.h"
#include "encodings.h"
#include "meta2pdf.h"
#include "podofo/podofo.h"
#include "litePDF-version.h"

#include <string>
#include <vector>
#include <set>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/pkcs12.h>
#include <openssl/x509.h>

using namespace PoDoFo;

#define DeletePtr(x) do { if ((x)) { delete (x); (x) = NULL; } } while (0)
#define FreePtr(x) do { if ((x)) { free(x); (x) = NULL; } } while (0)

#define LITEPDF_RETURN_VAL_IF_FAIL(what, where, ret) do { \
   if (!(what)) { \
      if (ctx->on_error) { \
         std::string buff = std::string((where)) + ": Assertion '" + (# what) + "' failed"; \
         ctx->on_error(ERROR_INVALID_PARAMETER, buff.c_str(), ctx->on_error_user_data); \
      } \
      return (ret); \
   } \
   } while (false)

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   switch (fdwReason) {
   case DLL_PROCESS_ATTACH:
		OpenSSL_add_all_algorithms();
		ERR_load_crypto_strings();
      ERR_load_PEM_strings();
      ERR_load_ASN1_strings();
      ERR_load_EVP_strings();
      break;
   case DLL_PROCESS_DETACH:
      ERR_free_strings();
      break;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

class MResourceInfo
{
 public:
   PdfName identifier;
   PdfReference objectReference;
   PdfRect pageSize;

   MResourceInfo(const PdfXObject *xObject)
   {
      identifier = xObject->GetIdentifier();
      objectReference = xObject->GetObjectReference();
      pageSize = xObject->GetPageSize();
   }
};

typedef enum _MDrawType {
   DrawTypeNone,
   DrawTypeAddPage,
   DrawTypeInsertPage,
   DrawTypeUpdatePage,
   DrawTypeToResource
} MDrawType;

typedef struct _MSignerData {
   X509 *cert;
   EVP_PKEY *pkey;
   unsigned int data_length;
} MSignerData;

typedef struct _litePDFContext
{
   unsigned int unit; /* LITEPDF_UNIT_... */

   litePDFErrorCB on_error;
   void *on_error_user_data;

   MEncodingsCache *encodingsCache;
   PdfEncrypt *encrypt;

   PdfStreamedDocument *streamed_document;
   PdfMemDocument *mem_document;
   bool loadedForUpdate;
   bool drawnData;
   PdfRefCountedBuffer *savedData; /* for SaveToData function */
   PdfRefCountedBuffer *signedSavedData; /* for SaveToDataWithSign function */
   bool documentSaved; /* whether already saved the document */

   struct _currentDraw {
      MDrawType type;
      HDC hdc;
      DBLSIZE mm;
      SIZE px;
      unsigned int atPageIndex;
      unsigned int flags;
   } currentDraw;

   std::vector<MResourceInfo> resources;
   std::vector<PdfOutlineItem *> outlines;

   bool sign_annotsTraversed;
   std::vector<PdfObject *> sign_annots;

   unsigned int sign_requestBytes;
   std::vector<MSignerData> sign_signers;
} litePDFContext;
//---------------------------------------------------------------------------

static void setLitePDFAsProducer(PdfDocument *document)
{
   if (!document) {
      return;
   }

   PdfInfo *info = document->GetInfo();
   if (!info) {
      return;
   }

   info->SetProducer("litePDF " LITEPDF_VERSION_PRETTY " - http://litePDF.sf.net (with help of PoDoFo - http://podofo.sf.net)");
}
//---------------------------------------------------------------------------

void * __stdcall LITEPDF_PUBLIC litePDF_CreateContext(litePDFErrorCB on_error,
                                                      void *on_error_user_data)
{
   litePDFContext *ctx;

   ctx = new litePDFContext();
   if (!ctx) {
      if (on_error) {
         on_error (ERROR_NOT_ENOUGH_MEMORY, "litePDF_CreateContext: Cannot create context, out of memory", on_error_user_data);
      }
      return NULL;
   }

   ctx->unit = LITEPDF_UNIT_MM;
   ctx->on_error = on_error;
   ctx->on_error_user_data = on_error_user_data;
   ctx->encodingsCache = NULL;
   ctx->encrypt = NULL;
   ctx->streamed_document = NULL;
   ctx->mem_document = NULL;
   ctx->loadedForUpdate = false;
   ctx->savedData = NULL;
   ctx->signedSavedData = NULL;
   ctx->drawnData = false;
   ctx->documentSaved = false;
   ctx->currentDraw.type = DrawTypeNone;
   ctx->currentDraw.hdc = NULL;
   ctx->currentDraw.mm.cx = 0.0;
   ctx->currentDraw.mm.cy = 0.0;
   ctx->currentDraw.px.cx = 0;
   ctx->currentDraw.px.cy = 0;
   ctx->currentDraw.atPageIndex = 0;
   ctx->resources.clear();
   ctx->outlines.clear();
   ctx->sign_annotsTraversed = false;
   ctx->sign_annots.clear();
   ctx->sign_requestBytes = 0;
   ctx->sign_signers.clear();

   PdfError::EnableLogging(false);
   PdfError::EnableDebug(false);

   return ctx;
}
//---------------------------------------------------------------------------

static void resetContextMembers(litePDFContext *ctx)
{
   if (!ctx) {
      return;
   }

   if (ctx->currentDraw.hdc) {
      AbortMeta2PdfDC(ctx->currentDraw.hdc);
      ctx->currentDraw.hdc = NULL;
   }

   if (ctx->streamed_document) {
      ctx->streamed_document->Close();
   }

   if (!ctx->sign_signers.empty()) {
      std::vector<MSignerData>::iterator it, end = ctx->sign_signers.end();
      for (it = ctx->sign_signers.begin(); it != end; it++) {
         X509 *cert = it->cert;
         EVP_PKEY *pkey = it->pkey;

         if (cert) {
            X509_free(cert);
            it->cert = NULL;
         }

         if (pkey) {
            EVP_PKEY_free(pkey);
            it->pkey = NULL;
         }
      }
   }

   // always delete documents first, then other data which can be used by them too
   DeletePtr(ctx->streamed_document);
   DeletePtr(ctx->mem_document);
   DeletePtr(ctx->encodingsCache);
   DeletePtr(ctx->encrypt);
   DeletePtr(ctx->savedData);
   DeletePtr(ctx->signedSavedData);

   ctx->loadedForUpdate = false;
   ctx->drawnData = false;
   ctx->documentSaved = false;
   ctx->currentDraw.type = DrawTypeNone;
   ctx->currentDraw.hdc = NULL;
   ctx->currentDraw.mm.cx = 0.0;
   ctx->currentDraw.mm.cy = 0.0;
   ctx->currentDraw.px.cx = 0;
   ctx->currentDraw.px.cy = 0;
   ctx->currentDraw.atPageIndex = 0;

   ctx->resources.clear();
   ctx->outlines.clear();
   ctx->sign_annotsTraversed = false;
   ctx->sign_annots.clear();
   ctx->sign_requestBytes = 0;
   ctx->sign_signers.clear();
}
//---------------------------------------------------------------------------

void __stdcall LITEPDF_PUBLIC litePDF_FreeContext(void *pctx)
{
   if (!pctx) {
      return;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   resetContextMembers(ctx);

   DeletePtr (ctx);
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetUnit(void *pctx,
                                              unsigned int unitValue)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (unitValue <= LITEPDF_UNIT_UNKNOWN || unitValue > LITEPDF_UNIT_1000TH_INCH) {
      if (ctx->on_error) {
         char buff[1024];

         sprintf(buff, "litePDF_SetUnit: Unknown unit value 0x%x", unitValue);

         ctx->on_error(ERROR_INVALID_PARAMETER, buff, ctx->on_error_user_data);
      }

      return FALSE;
   }

   ctx->unit = unitValue;

   return TRUE;
}
//---------------------------------------------------------------------------

unsigned int __stdcall LITEPDF_PUBLIC litePDF_GetUnit(void *pctx)
{
   if (!pctx) {
      return LITEPDF_UNIT_UNKNOWN;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   return ctx->unit;
}
//---------------------------------------------------------------------------

static double unitToMM(unsigned int unit,
                       double value)
{
   switch (unit) {
   case LITEPDF_UNIT_MM:
      break;
   case LITEPDF_UNIT_10TH_MM:
      value = value / 10.0;
      break;
   case LITEPDF_UNIT_100TH_MM:
      value = value / 100.0;
      break;
   case LITEPDF_UNIT_1000TH_MM:
      value = value / 1000.0;
      break;
   case LITEPDF_UNIT_INCH:
      value = value * 25.4;
      break;
   case LITEPDF_UNIT_10TH_INCH:
      value = (value / 10.0) * 25.4;
      break;
   case LITEPDF_UNIT_100TH_INCH:
      value = (value / 100.0) * 25.4;
      break;
   case LITEPDF_UNIT_1000TH_INCH:
      value = (value / 1000.0) * 25.4;
      break;
   }

   return value;
}
//---------------------------------------------------------------------------

static double mmToUnit(unsigned int unit,
                       double value)
{
   switch (unit) {
   case LITEPDF_UNIT_MM:
      break;
   case LITEPDF_UNIT_10TH_MM:
      value = value * 10.0;
      break;
   case LITEPDF_UNIT_100TH_MM:
      value = value * 100.0;
      break;
   case LITEPDF_UNIT_1000TH_MM:
      value = value * 1000.0;
      break;
   case LITEPDF_UNIT_INCH:
      value = value / 25.4;
      break;
   case LITEPDF_UNIT_10TH_INCH:
      value = (value * 10.0) / 25.4;
      break;
   case LITEPDF_UNIT_100TH_INCH:
      value = (value * 100.0) / 25.4;
      break;
   case LITEPDF_UNIT_1000TH_INCH:
      value = (value * 1000.0) / 25.4;
      break;
   }

   return value;
}
//---------------------------------------------------------------------------

static double getUnitRatio(unsigned int unit)
{
   double ratio = 1.0;

   switch (unit) {
   case LITEPDF_UNIT_MM:
   case LITEPDF_UNIT_INCH:
      break;
   case LITEPDF_UNIT_10TH_MM:
   case LITEPDF_UNIT_10TH_INCH:
      ratio = 10.0;
      break;
   case LITEPDF_UNIT_100TH_MM:
   case LITEPDF_UNIT_100TH_INCH:
      ratio = 100.0;
      break;
   case LITEPDF_UNIT_1000TH_MM:
   case LITEPDF_UNIT_1000TH_INCH:
      ratio = 1000.0;
      break;
   }

   return ratio;
}
//---------------------------------------------------------------------------

static void handleException(litePDFContext *ctx,
                            const char *where,
                            const PdfError &error)
{
   if (!ctx || !ctx->on_error) {
      return;
   }

   std::string buff = std::string(where) + ": " + error.ErrorName(error.GetError());
   unsigned int code = ERROR_EXTENDED_ERROR;

   switch (error.GetError()) {
   case ePdfError_InvalidHandle:
      code = ERROR_INVALID_HANDLE;
      break;
   case ePdfError_FileNotFound:
      code = ERROR_FILE_NOT_FOUND;
      break;
   case ePdfError_InvalidDeviceOperation:
      code = ERROR_DISK_OPERATION_FAILED;
      break;
   case ePdfError_UnexpectedEOF:
      code = ERROR_HANDLE_EOF;
      break;
   case ePdfError_OutOfMemory:
      code = ERROR_OUTOFMEMORY;
      break;
   case ePdfError_ValueOutOfRange:
      code = ERROR_INVALID_DATA;
      break;
   case ePdfError_InvalidPassword:
      code = ERROR_WRONG_PASSWORD;
      break;
   case ePdfError_ChangeOnImmutable:
      code = ERROR_INVALID_FUNCTION;
      buff = std::string(where) + ": The function can be called only with memory-based documents";
      break;
   }

   const char *delim = "\n   ";
   const TDequeErrorInfo &callStack = error.GetCallstack();
   TCIDequeErrorInfo it, end = callStack.end();

   if (callStack.size() == 1) {
      delim = ": ";
   }

   for (it = callStack.begin(); it != end; it++) {
      if (!(*it).GetInformation().empty()) {
         buff += delim;
         buff += (*it).GetInformation().c_str();
      }

      if (!(*it).GetInformationW().empty()) {
         std::wstring wstr = (*it).GetInformationW();
         int wrote = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
         char *cbuff = (char *) malloc (sizeof(char) * (1 + wrote));
         if (cbuff) {
            wrote = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), cbuff, wrote, NULL, NULL);
            cbuff[wrote] = 0;
            buff += delim;
            buff += cbuff;
            free(cbuff);
         }
      }
   }

   ctx->on_error (code, buff.c_str(), ctx->on_error_user_data);
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_PrepareEncryption(void *pctx,
                                                        const char *userPassword,
                                                        const char *ownerPassword,
                                                        unsigned int permissions,
                                                        unsigned int algorithm)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   DeletePtr (ctx->encrypt);

   if (!ownerPassword || !*ownerPassword) {
      return TRUE;
   }

   PdfEncrypt::EPdfEncryptAlgorithm eAlgorithm;
   PdfEncrypt::EPdfKeyLength eKeyLength;

   switch(algorithm) {
   case LITEPDF_ENCRYPT_ALGORITHM_RC4V1:
      eAlgorithm = PdfEncrypt::ePdfEncryptAlgorithm_RC4V1;
      eKeyLength = PdfEncrypt::ePdfKeyLength_40;
      break;
   case LITEPDF_ENCRYPT_ALGORITHM_RC4V2:
      eAlgorithm = PdfEncrypt::ePdfEncryptAlgorithm_RC4V2;
      eKeyLength = PdfEncrypt::ePdfKeyLength_128;
      break;
   case LITEPDF_ENCRYPT_ALGORITHM_AESV2:
      eAlgorithm = PdfEncrypt::ePdfEncryptAlgorithm_AESV2;
      eKeyLength = PdfEncrypt::ePdfKeyLength_128;
      break;
   case LITEPDF_ENCRYPT_ALGORITHM_AESV3:
      eAlgorithm = PdfEncrypt::ePdfEncryptAlgorithm_AESV3;
      eKeyLength = PdfEncrypt::ePdfKeyLength_256;
      break;
   default:
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_PARAMETER, "litePDF_PrepareEncryption: Unknown encryption algorithm", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      ctx->encrypt = PdfEncrypt::CreatePdfEncrypt(userPassword ? userPassword : "", ownerPassword, permissions, eAlgorithm, eKeyLength);
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_PrepareEncryption", error);
      return FALSE;
   }

   return ctx->encrypt != NULL;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateFileDocument(void *pctx,
                                                         const char *fileName)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_CreateFileDocument", FALSE);

   if (ctx->streamed_document || ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_ALREADY_INITIALIZED, "litePDF_CreateFileDocument: Context has an opened document already", ctx->on_error_user_data);
      }

      return FALSE;
   }

   if (ctx->encrypt && (
       ctx->encrypt->GetEncryptAlgorithm() == PdfEncrypt::ePdfEncryptAlgorithm_AESV2 ||
       ctx->encrypt->GetEncryptAlgorithm() == PdfEncrypt::ePdfEncryptAlgorithm_AESV3)) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_NOT_SUPPORTED, "litePDF_CreateFileDocument: Cannot create file-based document with AES encryption, use memory-based document instead", ctx->on_error_user_data);
      }

      return FALSE;
   }

   try {
      ctx->streamed_document = new PdfStreamedDocument(fileName, ePdfVersion_1_7, ctx->encrypt);
      setLitePDFAsProducer(ctx->streamed_document);
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_CreateFileDocument", error);
      return FALSE;
   }

   if (!ctx->streamed_document) {
      if (ctx->on_error) {
         ctx->on_error (ERROR_NOT_ENOUGH_MEMORY, "litePDF_CreateFileDocument: Cannot create PDF document, out of memory", ctx->on_error_user_data);
      }
      return FALSE;
   }

   DeletePtr (ctx->encodingsCache);
   ctx->encodingsCache = new MEncodingsCache(ctx->streamed_document);

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateFileDocumentW(void *pctx,
                                                          const wchar_t *fileName)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_CreateFileDocumentW", FALSE);

   if (ctx->streamed_document || ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_ALREADY_INITIALIZED, "litePDF_CreateFileDocumentW: Context has an opened document already", ctx->on_error_user_data);
      }

      return FALSE;
   }

   if (ctx->encrypt && (
       ctx->encrypt->GetEncryptAlgorithm() == PdfEncrypt::ePdfEncryptAlgorithm_AESV2 ||
       ctx->encrypt->GetEncryptAlgorithm() == PdfEncrypt::ePdfEncryptAlgorithm_AESV3)) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_NOT_SUPPORTED, "litePDF_CreateFileDocumentW: Cannot create file-based document with AES encryption, use memory-based document instead", ctx->on_error_user_data);
      }

      return FALSE;
   }

   try {
      ctx->streamed_document = new PdfStreamedDocument(fileName, ePdfVersion_1_7, ctx->encrypt);
      setLitePDFAsProducer(ctx->streamed_document);
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_CreateFileDocumentW", error);
      return FALSE;
   }

   if (!ctx->streamed_document) {
      if (ctx->on_error) {
         ctx->on_error (ERROR_NOT_ENOUGH_MEMORY, "litePDF_CreateFileDocumentW: Cannot create PDF document, out of memory", ctx->on_error_user_data);
      }
      return FALSE;
   }

   DeletePtr (ctx->encodingsCache);
   ctx->encodingsCache = new MEncodingsCache(ctx->streamed_document);

   return TRUE;
}
//---------------------------------------------------------------------------

static bool prepareMemDocument(litePDFContext *ctx,
                               const char *where)
{
   if (!ctx) {
      return false;
   }

   if (ctx->streamed_document || ctx->mem_document) {
      if (ctx->on_error) {
         std::string buff = std::string(where) + ": Context has an opened document already";
         ctx->on_error(ERROR_ALREADY_INITIALIZED, buff.c_str(), ctx->on_error_user_data);
      }

      return false;
   }

   try {
      ctx->mem_document = new PdfMemDocument();
   } catch(const PdfError &error) {
      handleException(ctx, where, error);
      return false;
   }

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         std::string buff = std::string(where) + ": Cannot create PDF document, out of memory";
         ctx->on_error (ERROR_NOT_ENOUGH_MEMORY, buff.c_str(), ctx->on_error_user_data);
      }
      return false;
   }

   DeletePtr (ctx->encodingsCache);
   ctx->encodingsCache = new MEncodingsCache(ctx->mem_document);

   return true;
}
//---------------------------------------------------------------------------

static void abortPreparedMemDocument(litePDFContext *ctx)
{
   if (!ctx) {
      return;
   }

   DeletePtr (ctx->encodingsCache);
   DeletePtr (ctx->mem_document);
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateMemDocument(void *pctx)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (!prepareMemDocument (ctx, "litePDF_CreateMemDocument")) {
      return FALSE;
   }

   try {
      ctx->mem_document->SetPdfVersion(ePdfVersion_1_7);
      if (ctx->encrypt) {
         ctx->mem_document->SetEncrypted(*ctx->encrypt);
      }
      setLitePDFAsProducer(ctx->mem_document);
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_CreateMemDocument", error);
      abortPreparedMemDocument(ctx);

      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_LoadFromFile(void *pctx,
                                                   const char *fileName,
                                                   const char *password,
                                                   BOOL loadCompletely,
                                                   BOOL forUpdate)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_LoadFromFile", FALSE);

   if (!prepareMemDocument (ctx, "litePDF_LoadFromFile")) {
      return FALSE;
   }

   PdfRefCountedInputDevice *inputDevice = NULL;

   try {
      if (loadCompletely) {
         FILE *in = fopen(fileName, "rb");
         if (!in) {
            PODOFO_RAISE_ERROR_INFO (ePdfError_FileNotFound, fileName);
         }

         if (!fseek(in, 0, SEEK_END)) {
            long sz = ftell(in);
            if (!fseek(in, 0, SEEK_SET)) {
               char *data = (char *) malloc(sizeof(char) * (sz + 1));
               if (!data) {
                  fclose(in);
                  PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
               }

               if (fread(data, sizeof(char), sz, in) == sz) {
                  inputDevice = new PdfRefCountedInputDevice(data, sz);
               } else {
                  free(data);
                  fclose(in);
                  PODOFO_RAISE_ERROR_INFO (ePdfError_FileNotFound, fileName);
               }
               free(data);
            } else {
               fclose(in);
               PODOFO_RAISE_ERROR_INFO (ePdfError_FileNotFound, fileName);
            }
         } else {
            fclose(in);
            PODOFO_RAISE_ERROR_INFO (ePdfError_FileNotFound, fileName);
         }
         fclose(in);
      } else {
         inputDevice = new PdfRefCountedInputDevice(fileName, "rb");
      }

      if (!inputDevice) {
         PODOFO_RAISE_ERROR_INFO (ePdfError_InternalLogic, "Assertion 'inputDevice != NULL' failed");
      }

      ctx->loadedForUpdate = forUpdate ? true : false;
      ctx->mem_document->LoadFromDevice(*inputDevice, ctx->loadedForUpdate);
   } catch(const PdfError &error) {
      bool opened = false;
      if (error.GetError() == ePdfError_InvalidPassword && password) {
         try {
            ctx->mem_document->SetPassword(password);
            opened = true;
         } catch(const PdfError &error1) {
            handleException(ctx, "litePDF_LoadFromFile", error1);
            abortPreparedMemDocument (ctx);
            DeletePtr (inputDevice);
            return FALSE;
         }
      }

      if (!opened) {
         handleException(ctx, "litePDF_LoadFromFile", error);
         abortPreparedMemDocument (ctx);
         DeletePtr (inputDevice);
         return FALSE;
      }
   }

   DeletePtr (inputDevice);

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_LoadFromFileW(void *pctx,
                                                    const wchar_t *fileName,
                                                    const char *password,
                                                    BOOL loadCompletely,
                                                    BOOL forUpdate)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_LoadFromFileW", FALSE);

   if (!prepareMemDocument (ctx, "litePDF_LoadFromFileW")) {
      return FALSE;
   }

   PdfRefCountedInputDevice *inputDevice = NULL;

   try {
      if (loadCompletely) {
         FILE *in = _wfopen(fileName, L"rb");
         if (!in) {
            PODOFO_RAISE_ERROR (ePdfError_FileNotFound);
         }

         if (!fseek(in, 0, SEEK_END)) {
            long sz = ftell(in);
            if (!fseek(in, 0, SEEK_SET)) {
               char *data = (char *) malloc(sizeof(char) * (sz + 1));
               if (!data) {
                  fclose(in);
                  PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
               }

               if (fread(data, sizeof(char), sz, in) == sz) {
                  inputDevice = new PdfRefCountedInputDevice(data, sz);
               } else {
                  free(data);
                  fclose(in);
                  PODOFO_RAISE_ERROR (ePdfError_FileNotFound);
               }
               free(data);
            } else {
               fclose(in);
               PODOFO_RAISE_ERROR (ePdfError_FileNotFound);
            }
         } else {
            fclose(in);
            PODOFO_RAISE_ERROR (ePdfError_FileNotFound);
         }
         fclose(in);
      } else {
         inputDevice = new PdfRefCountedInputDevice(fileName, "rb");
      }

      if (!inputDevice) {
         PODOFO_RAISE_ERROR_INFO (ePdfError_InternalLogic, "Assertion 'inputDevice != NULL' failed");
      }

      ctx->loadedForUpdate = forUpdate ? true : false;
      ctx->mem_document->LoadFromDevice(*inputDevice, ctx->loadedForUpdate);
   } catch(const PdfError &error) {
      bool opened = false;
      if (error.GetError() == ePdfError_InvalidPassword && password) {
         try {
            ctx->mem_document->SetPassword(password);
            opened = true;
         } catch(const PdfError &error1) {
            handleException(ctx, "litePDF_LoadFromFileW", error1);
            abortPreparedMemDocument (ctx);
            DeletePtr (inputDevice);
            return FALSE;
         }
      }

      if (!opened) {
         handleException(ctx, "litePDF_LoadFromFileW", error);
         abortPreparedMemDocument (ctx);
         DeletePtr (inputDevice);
         return FALSE;
      }
   }

   DeletePtr (inputDevice);

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_LoadFromData(void *pctx,
                                                   const BYTE *data,
                                                   unsigned int dataLength,
                                                   const char *password,
                                                   BOOL forUpdate)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (data != NULL, "litePDF_LoadFromData", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (dataLength > 0, "litePDF_LoadFromData", FALSE);

   if (!prepareMemDocument (ctx, "litePDF_LoadFromData")) {
      return FALSE;
   }

   PdfRefCountedInputDevice inputDevice((const char *) data, dataLength);

   try {
      ctx->loadedForUpdate = forUpdate ? true : false;
      ctx->mem_document->LoadFromDevice(inputDevice, ctx->loadedForUpdate);
   } catch(const PdfError &error) {
      bool opened = false;
      if (error.GetError() == ePdfError_InvalidPassword && password) {
         try {
            ctx->mem_document->SetPassword(password);
            opened = true;
         } catch(const PdfError &error1) {
            handleException(ctx, "litePDF_LoadFromData", error1);
            abortPreparedMemDocument (ctx);
            return FALSE;
         }
      }

      if (!opened) {
         handleException(ctx, "litePDF_LoadFromData", error);
         abortPreparedMemDocument (ctx);
         return FALSE;
      }
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFile(void *pctx,
                                                 const char *fileName)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_SaveToFile", FALSE);

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SaveToFile: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SaveToFile: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (ctx->drawnData) {
         ctx->mem_document->EmbedSubsetFonts();
      }
      ctx->documentSaved = true;
      if (ctx->loadedForUpdate) {
         ctx->mem_document->WriteUpdate(fileName);
      } else {
         ctx->mem_document->Write(fileName);
      }
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_SaveToFile", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileW(void *pctx,
                                                  const wchar_t *fileName)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_SaveToFileW", FALSE);

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SaveToFileW: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SaveToFileW: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (ctx->drawnData) {
         ctx->mem_document->EmbedSubsetFonts();
      }
      ctx->documentSaved = true;
      if (ctx->loadedForUpdate) {
         ctx->mem_document->WriteUpdate(fileName);
      } else {
         ctx->mem_document->Write(fileName);
      }
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_SaveToFileW", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToData(void *pctx,
                                                 BYTE *data,
                                                 unsigned int *dataLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (dataLength != NULL, "litePDF_SaveToData", FALSE);

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SaveToData: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (!ctx->savedData) {
         if (ctx->documentSaved) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SaveToData: Document was already saved", ctx->on_error_user_data);
            }
            return FALSE;
         }

         if (ctx->drawnData) {
            ctx->mem_document->EmbedSubsetFonts();
         }

         ctx->savedData = new PdfRefCountedBuffer();
         if (!ctx->savedData) {
            PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
         }

         ctx->documentSaved = true;

         PdfOutputDevice outputDevice(ctx->savedData);
         if (ctx->loadedForUpdate) {
            ctx->mem_document->WriteUpdate(&outputDevice);
         } else {
            ctx->mem_document->Write(&outputDevice);
         }
      }

      if (data) {
         if (*dataLength >= ctx->savedData->GetSize()) {
            memcpy(data, ctx->savedData->GetBuffer(), ctx->savedData->GetSize());
            *dataLength = ctx->savedData->GetSize();
         } else {
            if (ctx->on_error) {
               ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_SaveToData: Data buffer is too small", ctx->on_error_user_data);
            }
            return FALSE;
         }
      } else {
         *dataLength = ctx->savedData->GetSize();
      }
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_SaveToData", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

void __stdcall LITEPDF_PUBLIC litePDF_Close(void *pctx)
{
   if (!pctx) {
      return;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (ctx->streamed_document) {
      try {
         if (ctx->drawnData) {
            ctx->streamed_document->EmbedSubsetFonts();
         }

         ctx->streamed_document->Close();
      } catch (const PdfError &error) {
         handleException(ctx, "litePDF_Close", error);
      }

      delete ctx->streamed_document;
      ctx->streamed_document = NULL;
   }

   resetContextMembers(ctx);
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetPageCount(void *pctx,
                                                   unsigned int *pageCount)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pageCount != NULL, "litePDF_GetPageCount", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetPageCount: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetPageCount: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      *pageCount = (unsigned int) document->GetPageCount();
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetPageCount", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetPageSize(void *pctx,
                                                  unsigned int pageIndex,
                                                  unsigned int *width_u,
                                                  unsigned int *height_u)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (width_u != NULL, "litePDF_GetPageSize", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (height_u != NULL, "litePDF_GetPageSize", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetPageSize: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetPageSize: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (pageIndex >= document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetPageSize: Page index is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      PdfRect pageSize = document->GetPage((int) pageIndex)->GetPageSize();

      *width_u = mmToUnit(ctx->unit, LITEPDF_POINTS_TO_MM(pageSize.GetWidth()));
      *height_u = mmToUnit(ctx->unit, LITEPDF_POINTS_TO_MM(pageSize.GetHeight()));
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetPageSize", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

HDC __stdcall LITEPDF_PUBLIC litePDF_AddPage(void *pctx,
                                             unsigned int width_u,
                                             unsigned int height_u,
                                             unsigned int width_px,
                                             unsigned int height_px,
                                             unsigned int drawFlags)
{
   if (!pctx) {
      return NULL;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (width_u > 0, "litePDF_AddPage", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (height_u > 0, "litePDF_AddPage", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (width_px > 0, "litePDF_AddPage", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (height_px > 0, "litePDF_AddPage", NULL);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_AddPage: Document was already saved", ctx->on_error_user_data);
      }
      return NULL;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddPage: No document is opened", ctx->on_error_user_data);
         }

         return NULL;
      }

      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_AddPage: Other drawing is in progress", ctx->on_error_user_data);
         }

         return NULL;
      }

      DBLSIZE page;

      page.cx = unitToMM(ctx->unit, width_u);
      page.cy = unitToMM(ctx->unit, height_u);

      ctx->currentDraw.hdc = CreateMeta2PdfDC(page);
      if (!ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(GetLastError(), "litePDF_AddPage: Failed to create a device context", ctx->on_error_user_data);
         }

         return NULL;
      }

      ctx->currentDraw.type = DrawTypeAddPage;
      ctx->currentDraw.mm.cx = unitToMM(ctx->unit, width_u);
      ctx->currentDraw.mm.cy = unitToMM(ctx->unit, height_u);
      ctx->currentDraw.px.cx = width_px;
      ctx->currentDraw.px.cy = height_px;
      ctx->currentDraw.flags = drawFlags;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_AddPage", error);
      return NULL;
   }

   return ctx->currentDraw.hdc;
}
//---------------------------------------------------------------------------

HDC __stdcall LITEPDF_PUBLIC litePDF_InsertPage(void *pctx,
                                                unsigned int pageIndex,
                                                unsigned int width_u,
                                                unsigned int height_u,
                                                unsigned int width_px,
                                                unsigned int height_px,
                                                unsigned int drawFlags)
{
   if (!pctx) {
      return NULL;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (width_u > 0, "litePDF_InsertPage", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (height_u > 0, "litePDF_InsertPage", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (width_px > 0, "litePDF_InsertPage", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (height_px > 0, "litePDF_InsertPage", NULL);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_InsertPage: Document was already saved", ctx->on_error_user_data);
      }
      return NULL;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_InsertPage: No document is opened", ctx->on_error_user_data);
         }

         return NULL;
      }

      if (pageIndex >= document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_InsertPage: Page index is out of range", ctx->on_error_user_data);
         }

         return NULL;
      }

      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_InsertPage: Other drawing is in progress", ctx->on_error_user_data);
         }

         return NULL;
      }

      DBLSIZE page;

      page.cx = unitToMM(ctx->unit, width_u);
      page.cy = unitToMM(ctx->unit, height_u);

      ctx->currentDraw.hdc = CreateMeta2PdfDC(page);
      if (!ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(GetLastError(), "litePDF_InsertPage: Failed to create a device context", ctx->on_error_user_data);
         }

         return NULL;
      }

      ctx->currentDraw.type = DrawTypeInsertPage;
      ctx->currentDraw.mm.cx = unitToMM(ctx->unit, width_u);
      ctx->currentDraw.mm.cy = unitToMM(ctx->unit, height_u);
      ctx->currentDraw.px.cx = width_px;
      ctx->currentDraw.px.cy = height_px;
      ctx->currentDraw.flags = drawFlags;
      ctx->currentDraw.atPageIndex = pageIndex;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_InsertPage", error);
      return NULL;
   }

   return ctx->currentDraw.hdc;
}
//---------------------------------------------------------------------------

HDC __stdcall LITEPDF_PUBLIC litePDF_UpdatePage(void *pctx,
                                                unsigned int pageIndex,
                                                unsigned int width_px,
                                                unsigned int height_px,
                                                unsigned int drawFlags)
{
   if (!pctx) {
      return NULL;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (width_px > 0, "litePDF_UpdatePage", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (height_px > 0, "litePDF_UpdatePage", NULL);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_UpdatePage: Document was already saved", ctx->on_error_user_data);
      }
      return NULL;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_UpdatePage: No document is opened", ctx->on_error_user_data);
         }

         return NULL;
      }

      if (pageIndex >= document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_UpdatePage: Page index is out of range", ctx->on_error_user_data);
         }

         return NULL;
      }

      unsigned int width_mm, height_mm;
      PdfRect pageSize = document->GetPage((int) pageIndex)->GetPageSize();

      width_mm = LITEPDF_POINTS_TO_MM(pageSize.GetWidth());
      height_mm = LITEPDF_POINTS_TO_MM(pageSize.GetHeight());

      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_UpdatePage: Other drawing is in progress", ctx->on_error_user_data);
         }

         return NULL;
      }

      DBLSIZE page;

      page.cx = width_mm;
      page.cy = height_mm;

      ctx->currentDraw.hdc = CreateMeta2PdfDC(page);
      if (!ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(GetLastError(), "litePDF_UpdatePage: Failed to create a device context", ctx->on_error_user_data);
         }

         return NULL;
      }

      ctx->currentDraw.type = DrawTypeUpdatePage;
      ctx->currentDraw.mm.cx = width_mm;
      ctx->currentDraw.mm.cy = height_mm;
      ctx->currentDraw.px.cx = width_px;
      ctx->currentDraw.px.cy = height_px;
      ctx->currentDraw.flags = drawFlags;
      ctx->currentDraw.atPageIndex = pageIndex;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_UpdatePage", error);
      return NULL;
   }

   return ctx->currentDraw.hdc;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_FinishPage(void *pctx,
                                                 HDC hDC)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (hDC != NULL, "litePDF_FinishPage", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_FinishPage: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfPainter painter;
   bool didDraw = false;
   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_FinishPage: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      HDC currentDC = ctx->currentDraw.hdc;
      LITEPDF_RETURN_VAL_IF_FAIL (currentDC == hDC, "litePDF_FinishPage", FALSE);

      PdfPage *page = NULL;
      PdfRect pageRect(0, 0, LITEPDF_MM_TO_POINTS(ctx->currentDraw.mm.cx), LITEPDF_MM_TO_POINTS(ctx->currentDraw.mm.cy));

      if (ctx->currentDraw.type == DrawTypeAddPage) {
         page = document->CreatePage(pageRect);
      } else if (ctx->currentDraw.type == DrawTypeInsertPage) {
         if (ctx->currentDraw.atPageIndex >= document->GetPageCount()) {
            page = document->CreatePage(pageRect);
            if (page && document->GetPagesTree()) {
               document->GetPagesTree()->ClearCache();
               page = document->GetPage(document->GetPageCount() - 1);
            }
         } else {
            page = document->InsertPage(pageRect, ctx->currentDraw.atPageIndex);
            if (page && document->GetPagesTree()) {
               document->GetPagesTree()->ClearCache();
               page = document->GetPage(ctx->currentDraw.atPageIndex);
            }
         }
      } else if (ctx->currentDraw.type == DrawTypeUpdatePage) {
         if (ctx->currentDraw.atPageIndex >= document->GetPageCount()) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_INDEX, "litePDF_FinishPage: Page index is out of range", ctx->on_error_user_data);
            }
         } else if (!ctx->mem_document) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_FinishPage: Only memory-based documents can update existing pages", ctx->on_error_user_data);
            }
         } else {
            page = document->GetPage((int) ctx->currentDraw.atPageIndex);
         }
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_FinishPage: Current draw operation is not a page draw", ctx->on_error_user_data);
         }
      }

      if (page) {
         didDraw = true;
         painter.SetPage(page);

         ctx->drawnData = true;
         ctx->currentDraw.hdc = NULL;

         // force font embedding when encrypting, Adobe Reader has trouble with fonts when
         // the document is encrypted and the fonts are not embedded
         if (ctx->encrypt && (ctx->currentDraw.flags & LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE) == 0) {
            ctx->currentDraw.flags |= LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET;
         }

         CloseMeta2PdfDC(currentDC, ctx->currentDraw.mm, ctx->currentDraw.px, document, &painter, ctx->encodingsCache, ctx->currentDraw.flags, ctx->on_error, ctx->on_error_user_data);

         painter.FinishPage();
      } 
   } catch (const PdfError &error) {
      try {
         if (painter.GetPage()) {
            painter.FinishPage();
         }
      } catch(...) {
      }

      handleException(ctx, "litePDF_FinishPage", error);
      didDraw = false;
   }

   if (ctx->currentDraw.hdc) {
      AbortMeta2PdfDC(ctx->currentDraw.hdc);
      ctx->currentDraw.hdc = NULL;
   }

   return didDraw ? TRUE : FALSE;
}
//---------------------------------------------------------------------------

HDC __stdcall LITEPDF_PUBLIC litePDF_AddResource(void *pctx,
                                                 unsigned int width_u,
                                                 unsigned int height_u,
                                                 unsigned int width_px,
                                                 unsigned int height_px,
                                                 unsigned int drawFlags)
{
   if (!pctx) {
      return NULL;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (width_u > 0, "litePDF_AddResource", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (height_u > 0, "litePDF_AddResource", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (width_px > 0, "litePDF_AddResource", NULL);
   LITEPDF_RETURN_VAL_IF_FAIL (height_px > 0, "litePDF_AddResource", NULL);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_AddResource: Document was already saved", ctx->on_error_user_data);
      }
      return NULL;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddResource: No document is opened", ctx->on_error_user_data);
         }

         return NULL;
      }

      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_AddResource: Other drawing is in progress", ctx->on_error_user_data);
         }

         return NULL;
      }

      DBLSIZE page;

      page.cx = unitToMM(ctx->unit, width_u);
      page.cy = unitToMM(ctx->unit, height_u);

      ctx->currentDraw.hdc = CreateMeta2PdfDC(page);
      if (!ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(GetLastError(), "litePDF_AddResource: Failed to create a device context", ctx->on_error_user_data);
         }

         return NULL;
      }

      ctx->currentDraw.type = DrawTypeToResource;
      ctx->currentDraw.mm.cx = unitToMM(ctx->unit, width_u);
      ctx->currentDraw.mm.cy = unitToMM(ctx->unit, height_u);
      ctx->currentDraw.px.cx = width_px;
      ctx->currentDraw.px.cy = height_px;
      ctx->currentDraw.flags = drawFlags;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_AddResource", error);
      return NULL;
   }

   return ctx->currentDraw.hdc;
}
//---------------------------------------------------------------------------

unsigned int __stdcall LITEPDF_PUBLIC litePDF_FinishResource(void *pctx,
                                                             HDC hDC)
{
   if (!pctx) {
      return 0;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (hDC != NULL, "litePDF_FinishResource", 0);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_FinishResource: Document was already saved", ctx->on_error_user_data);
      }
      return 0;
   }

   PdfPainter painter;
   PdfXObject *xObject = NULL;
   unsigned int resourceID = 0;
   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_FinishResource: No document is opened", ctx->on_error_user_data);
         }

         return 0;
      }

      HDC currentDC = ctx->currentDraw.hdc;
      LITEPDF_RETURN_VAL_IF_FAIL (currentDC == hDC, "litePDF_FinishResource", 0);

      PdfRect pageRect(0, 0, LITEPDF_MM_TO_POINTS(ctx->currentDraw.mm.cx), LITEPDF_MM_TO_POINTS(ctx->currentDraw.mm.cy));

      if (ctx->currentDraw.type == DrawTypeToResource) {
         xObject = new PdfXObject(pageRect, document);
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_FinishResource: Current draw operation is not a resource draw", ctx->on_error_user_data);
         }
      }

      if (xObject) {
         ctx->resources.push_back(MResourceInfo(xObject));
         resourceID = (unsigned int) ctx->resources.size();

         painter.SetPage(xObject);

         ctx->drawnData = true;
         ctx->currentDraw.hdc = NULL;

         // force font embedding when encrypting, Adobe Reader has trouble with fonts when
         // the document is encrypted and the fonts are not embedded
         if (ctx->encrypt && (ctx->currentDraw.flags & LITEPDF_DRAW_FLAG_EMBED_FONTS_COMPLETE) == 0) {
            ctx->currentDraw.flags |= LITEPDF_DRAW_FLAG_EMBED_FONTS_SUBSET;
         }

         CloseMeta2PdfDC(currentDC, ctx->currentDraw.mm, ctx->currentDraw.px, document, &painter, ctx->encodingsCache, ctx->currentDraw.flags, ctx->on_error, ctx->on_error_user_data);

         painter.FinishPage();

         DeletePtr(xObject);
      } 
   } catch (const PdfError &error) {
      try {
         if (painter.GetPage()) {
            painter.FinishPage();
         }
      } catch(...) {
      }

      handleException(ctx, "litePDF_FinishResource", error);
      resourceID = 0;
   }

   if (ctx->currentDraw.hdc) {
      AbortMeta2PdfDC(ctx->currentDraw.hdc);
      ctx->currentDraw.hdc = NULL;
   }

   DeletePtr(xObject);

   return resourceID;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_DeletePage(void *pctx,
                                                 unsigned int pageIndex)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_DeletePage: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_DeletePage: A draw operation is in progress", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_DeletePage: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_DeletePage: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (pageIndex >= ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_DeletePage: Page index is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      ctx->mem_document->DeletePages(pageIndex, 1);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_DeletePage", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_AddPagesFrom(void *pctx,
                                                   void *pctx_from,
                                                   unsigned int pageIndex,
                                                   unsigned int pageCount)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pctx_from != NULL, "litePDF_AddPagesFrom", FALSE);

   litePDFContext *ctx_from = (litePDFContext *) pctx_from;

   LITEPDF_RETURN_VAL_IF_FAIL (ctx != ctx_from, "litePDF_AddPagesFrom", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_AddPagesFrom: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_AddPagesFrom: A draw operation is in progress", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddPagesFrom: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddPagesFrom: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx_from->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddPagesFrom: Source context does not hold memory-based document", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (pageIndex >= ctx_from->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_AddPagesFrom: Page index is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      ctx->mem_document->InsertPages(*ctx_from->mem_document, pageIndex, pageCount > 0 ? pageCount : ctx_from->mem_document->GetPageCount());
      if (ctx->mem_document->GetPagesTree()) {
         ctx->mem_document->GetPagesTree()->ClearCache();
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_AddPagesFrom", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_InsertPageFrom(void *pctx,
                                                     unsigned int pageIndexTo,
                                                     void *pctx_from,
                                                     unsigned int pageIndexFrom)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pctx_from != NULL, "litePDF_InsertPageFrom", FALSE);

   litePDFContext *ctx_from = (litePDFContext *) pctx_from;

   LITEPDF_RETURN_VAL_IF_FAIL (ctx != ctx_from, "litePDF_InsertPageFrom", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_InsertPageFrom: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_InsertPageFrom: A draw operation is in progress", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_InsertPageFrom: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_InsertPageFrom: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx_from->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_InsertPageFrom: Source context does not hold memory-based document", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (pageIndexFrom >= ctx_from->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_InsertPageFrom: Page index is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      ctx->mem_document->InsertExistingPageAt(*ctx_from->mem_document, pageIndexFrom, pageIndexTo);
      if (ctx->mem_document->GetPagesTree()) {
         ctx->mem_document->GetPagesTree()->ClearCache();
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_InsertPageFrom", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

unsigned int __stdcall LITEPDF_PUBLIC litePDF_AddPageFromAsResource(void *pctx,
                                                                    void *pctx_from,
                                                                    unsigned int pageIndex,
                                                                    BOOL useTrimBox)
{
   if (!pctx) {
      return 0;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pctx_from != NULL, "litePDF_AddPageFromAsResource", 0);

   litePDFContext *ctx_from = (litePDFContext *) pctx_from;
   unsigned int resourceID = 0;

   LITEPDF_RETURN_VAL_IF_FAIL (ctx != ctx_from, "litePDF_AddPageFromAsResource", 0);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_AddPageFromAsResource: Document was already saved", ctx->on_error_user_data);
      }
      return 0;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_AddPageFromAsResource: A draw operation is in progress", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddPageFromAsResource: No document is opened", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddPageFromAsResource: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx_from->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_AddPageFromAsResource: Source context does not hold memory-based document", ctx->on_error_user_data);
         }

         return 0;
      }

      if (pageIndex >= ctx_from->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_AddPageFromAsResource: Page index is out of range", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfXObject xObject(*ctx_from->mem_document, pageIndex, ctx->mem_document, NULL, useTrimBox ? true : false);

      ctx->resources.push_back(MResourceInfo(&xObject));
      resourceID = (unsigned int) ctx->resources.size();

   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_AddPageFromAsResource", error);
      return 0;
   }

   return resourceID;
}
//---------------------------------------------------------------------------

unsigned int __stdcall LITEPDF_PUBLIC litePDF_PageToResource(void *pctx,
                                                             unsigned int pageIndex)
{
   if (!pctx) {
      return 0;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   unsigned int resourceID = 0;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_PageToResource: Document was already saved", ctx->on_error_user_data);
      }
      return 0;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_PageToResource: A draw operation is in progress", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_PageToResource: No document is opened", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_PageToResource: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return 0;
      }

      if (pageIndex >= ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_PageToResource: Page index is out of range", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfXObject xObject(ctx->mem_document, pageIndex, NULL, false);

      ctx->resources.push_back(MResourceInfo(&xObject));
      resourceID = (unsigned int) ctx->resources.size();

   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_PageToResource", error);
      return 0;
   }

   return resourceID;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetResourceSize(void *pctx,
                                                      unsigned int resourceID,
                                                      unsigned int *width_u,
                                                      unsigned int *height_u)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (resourceID > 0, "litePDF_GetResourceSize", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (width_u != NULL, "litePDF_GetResourceSize", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (height_u != NULL, "litePDF_GetResourceSize", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetResourceSize: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetResourceSize: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (resourceID - 1 >= (unsigned int) ctx->resources.size()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetResourceSize: Resource ID is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      PdfRect pageSize = ctx->resources[resourceID - 1].pageSize;

      *width_u = mmToUnit(ctx->unit, LITEPDF_POINTS_TO_MM(pageSize.GetWidth()));
      *height_u = mmToUnit(ctx->unit, LITEPDF_POINTS_TO_MM(pageSize.GetHeight()));
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetResourceSize", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

static BOOL drawResourceWithMatrix(litePDFContext *ctx,
                                   const char *where,
                                   unsigned int resourceID,
                                   unsigned int pageIndex,
                                   double dA,
                                   double dB,
                                   double dC,
                                   double dD,
                                   double dE,
                                   double dF,
                                   bool invertDF)
{
   if (!ctx) {
      return FALSE;
   }

   LITEPDF_RETURN_VAL_IF_FAIL (resourceID > 0, where, FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         std::string buff = std::string(where) + ": Document was already saved";
         ctx->on_error(ERROR_INVALID_OPERATION, buff.c_str(), ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfPainter painter;

   try {
      PdfDocument *document = NULL;

      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": Other draw operation is in progress";
            ctx->on_error(ERROR_IO_PENDING, buff.c_str(), ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": No document is opened";
            ctx->on_error(ERROR_INVALID_HANDLE, buff.c_str(), ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (resourceID - 1 >= (unsigned int) ctx->resources.size()) {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": Resource ID is out of range";
            ctx->on_error(ERROR_INVALID_INDEX, buff.c_str(), ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (pageIndex >= document->GetPageCount()) {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": Page index is out of range";
            ctx->on_error(ERROR_INVALID_INDEX, buff.c_str(), ctx->on_error_user_data);
         }

         return FALSE;
      }

      PdfPage *page = document->GetPage(pageIndex);

      LITEPDF_RETURN_VAL_IF_FAIL (page != NULL, where, FALSE);

      const MResourceInfo &resource = ctx->resources[resourceID - 1];

      if (invertDF) {
         double realResourceHeight = resource.pageSize.GetWidth() * dB + resource.pageSize.GetHeight() * dD;
         dF = page->GetPageSize().GetHeight() - dF - realResourceHeight;
      }

      page->AddResource(resource.identifier, resource.objectReference, "XObject");

      painter.SetPage(page);

      PdfStream *m_pCanvas = painter.GetCanvas();
      LITEPDF_RETURN_VAL_IF_FAIL (m_pCanvas != NULL, where, FALSE);

      painter.Save();
      painter.SetTransformationMatrix(dA, dB, dC, dD, dE, dF);

      std::ostringstream  m_oss;
      m_oss << "/" << resource.identifier.GetName() << " Do" << std::endl;

      m_pCanvas->Append(m_oss.str().c_str());
      painter.Restore();

      painter.FinishPage();
   } catch (const PdfError &error) {
      try {
         if (painter.GetPage()) {
            painter.FinishPage();
         }
      } catch(...) {
      }

      handleException(ctx, where, error);
      return FALSE;
   }

   return TRUE;
}

BOOL __stdcall LITEPDF_PUBLIC litePDF_DrawResource(void *pctx,
                                                   unsigned int resourceID,
                                                   unsigned int pageIndex,
                                                   unsigned int unitValue,
                                                   int x,
                                                   int y,
                                                   int scaleX,
                                                   int scaleY)
{
   double ratio = getUnitRatio(unitValue);
   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (ratio <= -1e-9 || ratio >= 1e-9, "litePDF_DrawResource", FALSE);

   return drawResourceWithMatrix(
      (litePDFContext *) pctx,
      "litePDF_DrawResource",
      resourceID,
      pageIndex,
      scaleX / ratio,
      0.0,
      0.0,
      scaleY / ratio,
      LITEPDF_MM_TO_POINTS(unitToMM(unitValue, x)),
      LITEPDF_MM_TO_POINTS(unitToMM(unitValue, y)),
      true);
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_DrawResourceWithMatrix(void *pctx,
                                                             unsigned int resourceID,
                                                             unsigned int pageIndex,
                                                             int a,
                                                             int b,
                                                             int c,
                                                             int d,
                                                             int e,
                                                             int f)
{
   return drawResourceWithMatrix(
      (litePDFContext *) pctx,
      "litePDF_DrawResourceWithMatrix",
      resourceID,
      pageIndex,
      a / 1000.0,
      b / 1000.0,
      c / 1000.0,
      d / 1000.0,
      e / 1000.0,
      f / 1000.0,
      false);
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetDocumentInfo(void *pctx,
                                                      const char *name,
                                                      const wchar_t *value)
{
   if (!pctx) {
      return FALSE;
   }

   unsigned short *data = NULL;
   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (name != NULL, "litePDF_SetDocumentInfo", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (value != NULL, "litePDF_SetDocumentInfo", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SetDocumentInfo: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   /* Do not allow changing Producer, it'll be always litePDF with PoDoFo */
   if (stricmp(name, "Producer") == 0) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_ACCESS_DENIED, "litePDF_SetDocumentInfo: Producer cannot be changed", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SetDocumentInfo: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      PdfString str;
      str.setFromWchar_t ((const wchar_t *) value);

      document->GetInfo()->SetCustomKey(name, str);
   } catch (const PdfError &error) {
      if (data) {
         free(data);
      }

      handleException(ctx, "litePDF_SetDocumentInfo", error);
      return FALSE;
   }

   if (data) {
      free(data);
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetDocumentInfoExists(void *pctx,
                                                            const char *name,
                                                            BOOL *pExists)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (name != NULL, "litePDF_GetDocumentInfoExists", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (pExists != NULL, "litePDF_GetDocumentInfoExists", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetDocumentInfoExists: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetDocumentInfoExists: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      *pExists = document->GetInfo()->GetObject()->GetDictionary().HasKey(name) ? TRUE : FALSE;

      return TRUE;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetDocumentInfoExists", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetDocumentInfo(void *pctx,
                                                      const char *name,
                                                      wchar_t *value,
                                                      unsigned int *valueLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (name != NULL, "litePDF_GetDocumentInfo", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (valueLength != NULL, "litePDF_GetDocumentInfo", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetDocumentInfo: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetDocumentInfo: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!document->GetInfo()->GetObject()->GetDictionary().HasKey(name)) {
         if (ctx->on_error) {
            std::string buff = "litePDF_GetDocumentInfo: Key '" + std::string(name) + "' does not exist";
            ctx->on_error(ERROR_NO_TOKEN, buff.c_str(), ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *object = document->GetInfo()->GetObject()->GetDictionary().GetKey(PdfName(name));
      if (!object) {
         if (ctx->on_error) {
            std::string buff = "litePDF_GetDocumentInfo: Key '" + std::string(name) + "' does not exist";
            ctx->on_error(ERROR_NO_TOKEN, buff.c_str(), ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!object->IsString() && !object->IsHexString()) {
         if (ctx->on_error) {
            std::string buff = "litePDF_GetDocumentInfo: Key '" + std::string(name) + "' does not contain string data";
            ctx->on_error(ERROR_INVALID_DATA, buff.c_str(), ctx->on_error_user_data);
         }

         return FALSE;
      }

      std::wstring wstr = object->GetString().GetStringW();
      if (value) {
         if (*valueLength <= wstr.length() + 1) {
            wcscpy(value, wstr.c_str());
            *valueLength = wstr.length();
         } else {
            return FALSE;
         }
      } else {
         *valueLength = wstr.length();
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetDocumentInfo", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetDocumentIsSigned(void *pctx,
                                                          BOOL *pIsSigned)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pIsSigned != NULL, "litePDF_GetDocumentIsSigned", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetDocumentIsSigned: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetDocumentIsSigned: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      *pIsSigned = FALSE;

      PdfAcroForm *form = document->GetAcroForm(ePdfDontCreateObject, ePdfAcroFormDefaultAppearance_None);
      if (form && form->GetObject()) {
         const PdfObject *pObj = form->GetObject();
         if (pObj->IsDictionary() &&
             pObj->GetDictionary().HasKey("Fields")) {
            const PdfArray &fields = pObj->GetDictionary().GetKey("Fields")->GetArray();
            PdfArray::const_iterator fit, fend = fields.end();

            for (fit = fields.begin(); fit != fend; fit++) {
               pObj = &(*fit);
               if (pObj->IsReference()) {
                  pObj = document->GetObjects()->GetObject(pObj->GetReference());
                  if (!pObj) {
                     continue;
                  }
               }

               if (pObj->IsDictionary() &&
                   pObj->GetDictionary().HasKey("FT")) {
                  pObj = pObj->GetDictionary().GetKey("FT");
               } else {
                  pObj = NULL;
               }

               if (pObj && pObj->GetName() == PdfName("Sig")) {
                  *pIsSigned = TRUE;
                  break;
               }
            }
         }
      }

      return TRUE;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetDocumentIsSigned", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

static unsigned int GetSignatureAnnotationsCount(litePDFContext *ctx,
                                                 PdfDocument *document,
                                                 bool forceRecalculate)
{
   if (!ctx || !document) {
      return 0;
   }

   if (forceRecalculate) {
      ctx->sign_annotsTraversed = false;
   }

   if (!ctx->sign_annotsTraversed) {
      ctx->sign_annots.clear();

      PdfVecObjects *objects = document->GetObjects();
      if (objects) {
         PdfVecObjects::const_iterator it, end = objects->end();
         for (it = objects->begin(); it != end; it++) {
            PdfObject *item = *it;

            if (item && item->IsDictionary()) {
               const PdfDictionary &dict = item->GetDictionary();
               if (dict.HasKey("Type") &&
                   dict.GetKey("Type")->IsName() &&
                   dict.GetKey("Type")->GetName() == PdfName("Annot") &&
                   dict.HasKey("Subtype") &&
                   dict.GetKey("Subtype")->IsName() &&
                   dict.GetKey("Subtype")->GetName() == PdfName("Widget") &&
                   dict.HasKey("FT") &&
                   dict.GetKey("FT")->IsName() &&
                   dict.GetKey("FT")->GetName() == PdfName("Sig")) {
                  ctx->sign_annots.push_back(item);
               }
            }
         }
      }

      ctx->sign_annotsTraversed = true;
   }

   return ctx->sign_annots.size();
}
//---------------------------------------------------------------------------

static PdfObject *GetSignatureAnnotationObject(litePDFContext *ctx,
                                               PdfDocument *document,
                                               unsigned int index)
{
   if (!ctx || !document) {
      return NULL;
   }

   if (index >= GetSignatureAnnotationsCount(ctx, document, false)) {
      return NULL;
   }

   return ctx->sign_annots[index];
}
//---------------------------------------------------------------------------

// free returned pointer and *out_annotation with DeletePtr() when no longer needed
static PdfSignatureField *GetSignatureField(litePDFContext *ctx,
                                            PdfDocument *document,
                                            unsigned int index,
                                            bool createIfDoesntExist,
                                            PdfAnnotation **out_annotation,
                                            unsigned int *out_ErrorCode)
{
   PdfObject *annot;

   annot = GetSignatureAnnotationObject(ctx, document, index);
   if (!annot) {
      *out_ErrorCode = ERROR_INVALID_INDEX;
      return NULL;
   }

   PdfObject *sigObj = NULL;

   if (annot->GetDictionary().HasKey(PdfName("V"))) {
      PdfObject *vObj = annot->GetDictionary().GetKey(PdfName("V"));
      if (vObj && vObj->IsReference()) {
         vObj = document->GetObjects()->GetObject(vObj->GetReference());
      }

      if (!vObj || !vObj->IsDictionary() ||
          !vObj->GetDictionary().HasKey(PdfName("Type")) ||
          !vObj->GetDictionary().GetKey(PdfName("Type")) ||
          !vObj->GetDictionary().GetKey(PdfName("Type"))->IsName() ||
          vObj->GetDictionary().GetKey(PdfName("Type"))->GetName() != PdfName("Sig")) {
         *out_ErrorCode = ERROR_INVALID_DATA;
         return NULL;
      }
   } else {
      if (!createIfDoesntExist) {
         *out_ErrorCode = ERROR_NOT_FOUND;
         return NULL;
      }
   }

   if (!annot->GetDictionary().HasKey(PdfName("P")) ||
       !annot->GetDictionary().GetKey(PdfName("P")) ||
       !annot->GetDictionary().GetKey(PdfName("P"))->IsReference()) {
      *out_ErrorCode = ERROR_INVALID_DATA;
      return NULL;
   }

   PdfPage *page = document->GetPagesTree()->GetPage(annot->GetDictionary().GetKey(PdfName("P"))->GetReference());
   if (!page) {
      PODOFO_RAISE_ERROR(ePdfError_PageNotFound);
   }

   *out_annotation = new PdfAnnotation(annot, page);
   if (!*out_annotation) {
      PODOFO_RAISE_ERROR_INFO(ePdfError_OutOfMemory, "Cannot allocate annotation object for a signature field");
   }

   PdfSignatureField *sigField = new PdfSignatureField(*out_annotation);
   if (!sigField) {
      DeletePtr(*out_annotation);
      PODOFO_RAISE_ERROR_INFO(ePdfError_OutOfMemory, "Cannot allocate signature field object");
   }

   return sigField;
}
//---------------------------------------------------------------------------

static bool ClaimGetSignatureFieldError(litePDFContext *ctx,
                                        unsigned int errorCode,
                                        const char *where)
{
   const char *suffix = NULL;

   switch(errorCode) {
   case ERROR_INVALID_INDEX:
      suffix = ": Index is out of range";
      break;
   case ERROR_INVALID_DATA:
      suffix = ": Object is not of expected type";
      break;
   case ERROR_OUTOFMEMORY:
      suffix = ": Out of memory";
      break;
   case ERROR_NOT_FOUND:
      suffix = ": Value not found";
      break;
   }

   if (!suffix) {
      return false;
   }

   if (ctx->on_error) {
      std::string buff = where + std::string(suffix);
      ctx->on_error(errorCode, buff.c_str(), ctx->on_error_user_data);
   }

   return true;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureCount(void *pctx,
                                                        unsigned int *pCount)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pCount != NULL, "litePDF_GetSignatureCount", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureCount: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureCount: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      *pCount = GetSignatureAnnotationsCount(ctx, document, true);

      return TRUE;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureCount", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureName(void *pctx,
                                                       unsigned int index,
                                                       char *name,
                                                       unsigned int *nameLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (nameLength != NULL, "litePDF_GetSignatureName", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureName: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureName: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *obj = GetSignatureAnnotationObject(ctx, document, index);
      if (obj) {
         std::string fname;

         if (obj->GetDictionary().HasKey("T")) {
            fname = obj->GetDictionary().GetKey("T")->GetString().GetString();
         } else {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_PARAMETER, "litePDF_GetSignatureName: Missing field name specifier", ctx->on_error_user_data);
            }

            return FALSE;
         }

         if (!name) {
            *nameLength = fname.size();
         } else {
            if (*nameLength >= fname.size() + 1) {
               memcpy(name, fname.c_str(), fname.size() + 1);
               *nameLength = fname.size();
            } else {
               if (ctx->on_error) {
                  ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetSignatureName: Data buffer is too small", ctx->on_error_user_data);
               }

               return FALSE;
            }
         }

         return TRUE;
      }

      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetSignatureName: Index is out of range", ctx->on_error_user_data);
      }

   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureName", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

class MTmpSigField : public PdfField
{
 public:
   MTmpSigField(PdfPage* pPage, const PdfRect & rRect, PdfDocument *pDoc) :
      PdfField(PoDoFo::ePdfField_Signature, pPage, rRect, pDoc)
   {
   }
};

BOOL __stdcall LITEPDF_PUBLIC litePDF_CreateSignature(void *pctx,
                                                      const char *name,
                                                      unsigned int annotationPageIndex,
                                                      int annotationX_u,
                                                      int annotationY_u,
                                                      int annotationWidth_u,
                                                      int annotationHeight_u,
                                                      unsigned int annotationFlags,
                                                      unsigned int *pAddedIndex)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_CreateSignature: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateSignature: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateSignature: The document has no pages", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (annotationPageIndex >= document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateSignature: Page index is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      std::string signFieldName = (name && *name) ? name : "Signature1";

      PdfAcroForm *form = document->GetAcroForm(ePdfDontCreateObject, ePdfAcroFormDefaultAppearance_None);
      if (form && form->GetObject()) {
         // the AcroForm is already there, check field names (that new should be unique)
         std::set<std::string> fieldNames;

         const PdfObject *pObj = form->GetObject();
         if (pObj->IsDictionary() &&
             pObj->GetDictionary().HasKey("Fields")) {
            const PdfArray &fields = pObj->GetDictionary().GetKey("Fields")->GetArray();
            PdfArray::const_iterator fit, fend = fields.end();

            for (fit = fields.begin(); fit != fend; fit++) {
               pObj = &(*fit);
               if (pObj->IsReference()) {
                  pObj = document->GetObjects()->GetObject(pObj->GetReference());
                  if (!pObj) {
                     continue;
                  }
               }

               if (pObj->IsDictionary() &&
                   pObj->GetDictionary().HasKey("FT")) {

                  if (pObj->GetDictionary().HasKey("T")) {
                     const PdfObject *pTObj = pTObj = pObj->GetDictionary().GetKey("T");
                     if (pTObj->IsString()) {
                        const PdfString &str = pTObj->GetString();
                        fieldNames.insert(str.GetString());
                     }
                  }
               }
            }
         }

         std::set<std::string>::iterator fnend = fieldNames.end();
         int counter = 2;
         while(counter > 0 && fieldNames.find(signFieldName) != fnend) {
            if (name && *name) {
               if (ctx->on_error) {
                  std::string buff = "litePDF_CreateSignature: Field named '" + std::string(name) + "' already exists";
                  ctx->on_error(ERROR_OBJECT_ALREADY_EXISTS, buff.c_str(), ctx->on_error_user_data);
               }
               return FALSE;
            }

            char buff[128];
            sprintf(buff, "Signature%d", counter);
            signFieldName = buff;
            counter++;
         }

         if (counter <= 0) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateSignature: Failed to reserve free signature field name", ctx->on_error_user_data);
            }
            return FALSE;
         }
      } else {
         // Disable default appearance
         form = document->GetAcroForm(ePdfCreateObject, ePdfAcroFormDefaultAppearance_None);
      }

      if (!form) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateSignature: Failed to create an AcroForm", ctx->on_error_user_data);
         }
         return FALSE;
      }

      PdfPage *page;
      PdfRect onPageRect(0, 0, 0, 0);

      page = document->GetPage(annotationPageIndex);
      if (!page) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateSignature: Cannot find page for annotation", ctx->on_error_user_data);
         }
         return FALSE;
      }

      onPageRect = PdfRect(page->GetPageSize().GetLeft() + LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationX_u)),
                           page->GetPageSize().GetBottom() + page->GetPageSize().GetHeight() - LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationY_u)),
                           LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationWidth_u)),
                           -LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationHeight_u)));

      MTmpSigField signField(page, onPageRect, document);
      signField.SetFieldName(signFieldName);

      // re-enum existing signature fields in the document
      unsigned int ii, nSigFields = GetSignatureAnnotationsCount(ctx, document, true), addedIdx;

      for (ii = 0; ii < nSigFields; ii++) {
         const PdfObject *sigObj = GetSignatureAnnotationObject(ctx, document, ii);
         if (sigObj &&
             sigObj->IsDictionary() &&
             sigObj->GetDictionary().HasKey("T")) {
            const PdfObject *pTObj = pTObj = sigObj->GetDictionary().GetKey("T");
            if (pTObj->IsString() && pTObj->GetString() == signFieldName) {
               addedIdx = ii;
               break;
            }
         }
      }

      if (ii >= nSigFields) {
         if (ctx->on_error) {
            std::string buff = "litePDF_CreateSignature: Failed to find added field by its name '" + signFieldName + "'";
            ctx->on_error(ERROR_NOT_FOUND, buff.c_str(), ctx->on_error_user_data);
         }
         return FALSE;
      }

      if (pAddedIndex) {
         *pAddedIndex = addedIdx;
      }

      return TRUE;
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_CreateSignature", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureHasData(void *pctx,
                                                          unsigned int index,
                                                          BOOL *pHasData)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pHasData != NULL, "litePDF_GetSignatureHasData", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureHasData: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureHasData: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *obj = GetSignatureAnnotationObject(ctx, document, index);
      if (obj) {
         *pHasData = FALSE;

         if (obj->IsDictionary() && obj->GetDictionary().HasKey(PdfName("V"))) {
            const PdfObject *vObj = obj->GetDictionary().GetKey(PdfName("V"));
            if (vObj && vObj->IsReference()) {
               vObj = document->GetObjects()->GetObject(vObj->GetReference());
            }

            if (vObj && vObj->IsDictionary() && vObj->GetDictionary().HasKey("Contents"))  {
               *pHasData = TRUE;
            }
         }

         return TRUE;
      } else if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetSignatureHasData: Index is out of range", ctx->on_error_user_data);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureHasData", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureData(void *pctx,
                                                       unsigned int index,
                                                       BYTE *data,
                                                       unsigned int *dataLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (dataLength != NULL, "litePDF_GetSignatureData", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureData: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureData: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *obj = GetSignatureAnnotationObject(ctx, document, index);
      if (obj) {
         if (obj->IsDictionary() && obj->GetDictionary().HasKey(PdfName("V"))) {
            const PdfObject *vObj = obj->GetDictionary().GetKey(PdfName("V"));
            if (vObj && vObj->IsReference()) {
               vObj = document->GetObjects()->GetObject(vObj->GetReference());
            }

            if (vObj && vObj->IsDictionary() && vObj->GetDictionary().HasKey("Contents"))  {
               const PdfObject *contentsObj = vObj->GetDictionary().GetKey("Contents");
               if (!contentsObj || !contentsObj->IsHexString()) {
                  if (ctx->on_error) {
                     ctx->on_error(ERROR_INVALID_TARGET_HANDLE, "litePDF_GetSignatureData: 'Contents' key is missing or invalid", ctx->on_error_user_data);
                  }
                  return FALSE;
               }

               pdf_long size2 = contentsObj->GetString().GetLength();

               if (data) {
                  if (*dataLength >= size2) {
                     const char *str = contentsObj->GetString().GetString();
                     pdf_long ii;
                     for (ii = 0; ii < size2; ii++) {
                        data[ii] = str[ii] & 0xFF;
                     }
                     *dataLength = size2;
                  } else {
                     if (ctx->on_error) {
                        ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetSignatureData: Data buffer is too small", ctx->on_error_user_data);
                     }
                     return FALSE;
                  }
               } else {
                  *dataLength = size2;
               }
            }
         }

         return TRUE;
      } else if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetSignatureData: Index is out of range", ctx->on_error_user_data);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureData", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureRanges(void *pctx,
                                                         unsigned int index,
                                                         unsigned __int64 *pRangesArray,
                                                         unsigned int *pRangesArrayLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pRangesArrayLength != NULL, "litePDF_GetSignatureRanges", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureRanges: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureRanges: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *obj = GetSignatureAnnotationObject(ctx, document, index);
      if (obj) {
         if (obj->IsDictionary() && obj->GetDictionary().HasKey(PdfName("V"))) {
            const PdfObject *vObj = obj->GetDictionary().GetKey(PdfName("V"));
            if (vObj && vObj->IsReference()) {
               vObj = document->GetObjects()->GetObject(vObj->GetReference());
            }

            if (vObj && vObj->IsDictionary() && vObj->GetDictionary().HasKey("ByteRange"))  {
               const PdfObject *byteRangeObj = vObj->GetDictionary().GetKey("ByteRange");
               if (!byteRangeObj || !byteRangeObj->IsArray()) {
                  if (ctx->on_error) {
                     ctx->on_error(ERROR_INVALID_TARGET_HANDLE, "litePDF_GetSignatureRanges: 'ByteRange' key is missing or invalid", ctx->on_error_user_data);
                  }
                  return FALSE;
               }

               pdf_long size2 = byteRangeObj->GetArray().size();

               if (pRangesArray) {
                  if (*pRangesArrayLength >= size2) {
                     pdf_long ii;
                     PdfArray::const_iterator it, end = byteRangeObj->GetArray().end();
                     for (ii = 0, it = byteRangeObj->GetArray().begin(); it != end; it++, ii++) {
                        if (!it->IsNumber()) {
                           if (ctx->on_error) {
                              ctx->on_error(ERROR_INVALID_DATA, "litePDF_GetSignatureRanges: 'ByteRange' array element is not a number", ctx->on_error_user_data);
                           }
                           return FALSE;
                        }
                        if (ii >= size2) {
                           if (ctx->on_error) {
                              ctx->on_error(ERROR_INVALID_DATA, "litePDF_GetSignatureRanges: Attempt to write out of buffer bounds", ctx->on_error_user_data);
                           }
                           return FALSE;
                        }

                        pRangesArray[ii] = (unsigned __int64) it->GetNumber();
                     }
                     *pRangesArrayLength = size2;
                  } else {
                     if (ctx->on_error) {
                        ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetSignatureRanges: Data buffer is too small", ctx->on_error_user_data);
                     }
                     return FALSE;
                  }
               } else {
                  *pRangesArrayLength = size2;
               }
            }
         }

         return TRUE;
      } else if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetSignatureRanges: Index is out of range", ctx->on_error_user_data);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureRanges", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureDate(void *pctx,
                                                       unsigned int index,
                                                       __int64 dateOfSign)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SetSignatureDate: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SetSignatureDate: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, true, &annotation, &errorCode);
      if (sigField) {
         sigField->EnsureSignatureObject();
         sigField->SetSignatureDate(dateOfSign > 0 ? PdfDate((time_t) dateOfSign) : PdfDate());
         res = TRUE;
      } else {
         DeletePtr(sigField);
         DeletePtr(annotation);

         LITEPDF_RETURN_VAL_IF_FAIL (ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_SetSignatureDate"), "litePDF_SetSignatureDate", FALSE);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SetSignatureDate", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureDate(void *pctx,
                                                       unsigned int index,
                                                       __int64 *pDateOfSign)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pDateOfSign != NULL, "litePDF_GetSignatureDate", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureDate: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureDate: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, false, &annotation, &errorCode);
      if (sigField) {
         PdfObject *sigObj = sigField->GetSignatureObject();

         if (sigObj &&
             sigObj->IsDictionary() &&
             sigObj->GetDictionary().HasKey(PdfName("M"))) {
            PdfObject *mObj = sigObj->GetDictionary().GetKey(PdfName("M"));
            if (mObj && mObj->IsString()) {
               PdfDate date(mObj->GetString());

               *pDateOfSign = (__int64) date.GetTime();
               res = TRUE;
            }
         }

         if (!res) {
            ClaimGetSignatureFieldError(ctx, ERROR_NOT_FOUND, "litePDF_GetSignatureDate");
         }
      } else {
         ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_GetSignatureDate");
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureDate", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureReason(void *pctx,
                                                         unsigned int index,
                                                         const wchar_t *reason)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (reason != NULL, "litePDF_SetSignatureReason", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SetSignatureReason: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SetSignatureReason: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, true, &annotation, &errorCode);
      if (sigField) {
         sigField->EnsureSignatureObject();
         PdfString value;
         value.setFromWchar_t(reason);
         sigField->SetSignatureReason(value);
         res = TRUE;
      } else {
         DeletePtr(sigField);
         DeletePtr(annotation);

         LITEPDF_RETURN_VAL_IF_FAIL (ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_SetSignatureReason"), "litePDF_SetSignatureReason", FALSE);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SetSignatureReason", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureReason(void *pctx,
                                                         unsigned int index,
                                                         wchar_t *value,
                                                         unsigned int *valueLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (valueLength != NULL, "litePDF_GetSignatureReason", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureReason: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureReason: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, false, &annotation, &errorCode);
      if (sigField) {
         PdfObject *sigObj = sigField->GetSignatureObject();

         if (sigObj &&
             sigObj->IsDictionary() &&
             sigObj->GetDictionary().HasKey(PdfName("Reason"))) {
            PdfObject *mObj = sigObj->GetDictionary().GetKey(PdfName("Reason"));
            if (mObj && mObj->IsString()) {
               std::wstring wvalue = mObj->GetString().GetStringW();

               res = TRUE;
               if (!value) {
                  *valueLength = wvalue.size();
               } else {
                  if (*valueLength >= wvalue.size() + 1) {
                     memcpy(value, wvalue.c_str(), (wvalue.size() + 1) * sizeof(wchar_t));
                     *valueLength = wvalue.size();
                  } else {
                     if (ctx->on_error) {
                        ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetSignatureReason: Data buffer is too small", ctx->on_error_user_data);
                     }

                     res = FALSE;
                  }
               }
            } else {
               ClaimGetSignatureFieldError(ctx, ERROR_NOT_FOUND, "litePDF_GetSignatureReason");
            }
         } else {
            ClaimGetSignatureFieldError(ctx, ERROR_NOT_FOUND, "litePDF_GetSignatureReason");
         }
      } else {
         ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_GetSignatureReason");
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureReason", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureLocation(void *pctx,
                                                           unsigned int index,
                                                           const wchar_t *location)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (location != NULL, "litePDF_SetSignatureLocation", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SetSignatureLocation: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SetSignatureLocation: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, true, &annotation, &errorCode);
      if (sigField) {
         sigField->EnsureSignatureObject();

         PdfString value;
         value.setFromWchar_t(location);
         sigField->SetSignatureLocation(value);
         res = TRUE;
      } else {
         DeletePtr(sigField);
         DeletePtr(annotation);

         LITEPDF_RETURN_VAL_IF_FAIL (ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_SetSignatureLocation"), "litePDF_SetSignatureLocation", FALSE);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SetSignatureLocation", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureLocation(void *pctx,
                                                           unsigned int index,
                                                           wchar_t *value,
                                                           unsigned int *valueLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (valueLength != NULL, "litePDF_GetSignatureLocation", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureLocation: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureLocation: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, false, &annotation, &errorCode);
      if (sigField) {
         PdfObject *sigObj = sigField->GetSignatureObject();

         if (sigObj &&
             sigObj->IsDictionary() &&
             sigObj->GetDictionary().HasKey(PdfName("Location"))) {
            PdfObject *mObj = sigObj->GetDictionary().GetKey(PdfName("Location"));
            if (mObj && mObj->IsString()) {
               std::wstring wvalue = mObj->GetString().GetStringW();

               res = TRUE;
               if (!value) {
                  *valueLength = wvalue.size();
               } else {
                  if (*valueLength >= wvalue.size() + 1) {
                     memcpy(value, wvalue.c_str(), (wvalue.size() + 1) * sizeof(wchar_t));
                     *valueLength = wvalue.size();
                  } else {
                     if (ctx->on_error) {
                        ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetSignatureLocation: Data buffer is too small", ctx->on_error_user_data);
                     }

                     res = FALSE;
                  }
               }
            } else {
               ClaimGetSignatureFieldError(ctx, ERROR_NOT_FOUND, "litePDF_GetSignatureLocation");
            }
         } else {
            ClaimGetSignatureFieldError(ctx, ERROR_NOT_FOUND, "litePDF_GetSignatureLocation");
         }
      } else {
         ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_GetSignatureLocation");
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureLocation", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureCreator(void *pctx,
                                                          unsigned int index,
                                                          const char *creator)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (creator != NULL, "litePDF_SetSignatureCreator", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SetSignatureCreator: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SetSignatureCreator: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, true, &annotation, &errorCode);
      if (sigField) {
         sigField->EnsureSignatureObject();

         PdfName value(creator);
         sigField->SetSignatureCreator(value);
         res = TRUE;
      } else {
         DeletePtr(sigField);
         DeletePtr(annotation);

         LITEPDF_RETURN_VAL_IF_FAIL (ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_SetSignatureCreator"), "litePDF_SetSignatureCreator", FALSE);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SetSignatureCreator", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetSignatureCreator(void *pctx,
                                                          unsigned int index,
                                                          char *value,
                                                          unsigned int *valueLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (valueLength != NULL, "litePDF_GetSignatureCreator", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetSignatureCreator: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetSignatureCreator: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, false, &annotation, &errorCode);
      if (sigField) {
         bool found = false;
         PdfObject *sigObj = sigField->GetSignatureObject();

         if (sigObj &&
             sigObj->IsDictionary() &&
             sigObj->GetDictionary().HasKey(PdfName("Prop_Build"))) {
            PdfObject *mProp = sigObj->GetDictionary().GetKey(PdfName("Prop_Build"));
            if (mProp &&
                mProp->IsDictionary() &&
                mProp->GetDictionary().HasKey(PdfName("App"))) {
               PdfObject *mApp = mProp->GetDictionary().GetKey(PdfName("App"));
               if (mApp &&
                   mApp->IsDictionary() &&
                   mApp->GetDictionary().HasKey(PdfName("Name"))) {
                  PdfObject *mName = mApp->GetDictionary().GetKey(PdfName("Name"));
                  if (mName && mName->IsName()) {
                     std::string svalue = mName->GetName().GetName();

                     res = TRUE;
                     if (!value) {
                        *valueLength = svalue.size();
                     } else {
                        if (*valueLength >= svalue.size() + 1) {
                           memcpy(value, svalue.c_str(), (svalue.size() + 1) * sizeof(char));
                           *valueLength = svalue.size();
                        } else {
                           if (ctx->on_error) {
                              ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetSignatureCreator: Data buffer is too small", ctx->on_error_user_data);
                           }

                           res = FALSE;
                        }
                     }
                  }
               }
            }
         }
         
         if (!found) {
            ClaimGetSignatureFieldError(ctx, ERROR_NOT_FOUND, "litePDF_GetSignatureCreator");
         }
      } else {
         ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_GetSignatureCreator");
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetSignatureCreator", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureAppearance(void *pctx,
                                                             unsigned int index,
                                                             unsigned int appearanceType,
                                                             unsigned int resourceID,
                                                             int offsetX_u,
                                                             int offsetY_u)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (appearanceType == LITEPDF_APPEARANCE_NORMAL ||
                               appearanceType == LITEPDF_APPEARANCE_ROLLOVER ||
                               appearanceType == LITEPDF_APPEARANCE_DOWN, "litePDF_SetSignatureAppearance", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (resourceID > 0, "litePDF_SetSignatureAppearance", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SetSignatureAppearance: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (resourceID - 1 >= (unsigned int) ctx->resources.size()) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_SetSignatureAppearance: Resource ID is out of range", ctx->on_error_user_data);
      }

      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SetSignatureAppearance: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, document, index, true, &annotation, &errorCode);
      if (sigField) {
         PdfXObject sigXObject(annotation->GetRect(), document);
         PdfPainter painter;

         try {
            painter.SetPage(&sigXObject);

            /* Workaround Adobe's reader error 'Expected a dict object.' when the stream
               contains only one object which does Save()/Restore() on its own, like
               the image XObject. */
            painter.MoveTo(0.0, 0.0);

            const MResourceInfo &resource = ctx->resources[resourceID - 1];
            double dA = 1.0, dB = 0.0, dC = 0.0, dD = 1.0, dE, dF;

            dE = LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, offsetX_u));
            dF = LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, offsetY_u));

            double realResourceHeight = resource.pageSize.GetWidth() * dB + resource.pageSize.GetHeight() * dD;
            dF = annotation->GetRect().GetHeight() - dF - realResourceHeight;

            // relative to signature field annotation
            dE = annotation->GetRect().GetLeft() + dE;
            dF = annotation->GetRect().GetBottom() - annotation->GetRect().GetHeight() + dF;

            sigXObject.AddResource(resource.identifier, resource.objectReference, "XObject");

            PdfStream *m_pCanvas = painter.GetCanvas();
            if (!m_pCanvas) {
               PODOFO_RAISE_ERROR(ePdfError_InvalidHandle);
            }

            painter.Save();
            painter.SetTransformationMatrix(dA, dB, dC, dD, dE, dF);

            std::ostringstream  m_oss;
            m_oss << "/" << resource.identifier.GetName() << " Do" << std::endl;

            m_pCanvas->Append(m_oss.str().c_str());
            painter.Restore();

            painter.FinishPage();

            EPdfAnnotationAppearance annotApp = ePdfAnnotationAppearance_Normal;

            switch (appearanceType) {
               default:
               case LITEPDF_APPEARANCE_NORMAL:
                  annotApp = ePdfAnnotationAppearance_Normal;
                  break;
               case LITEPDF_APPEARANCE_ROLLOVER:
                  annotApp = ePdfAnnotationAppearance_Rollover;
                  break;
               case LITEPDF_APPEARANCE_DOWN:
                  annotApp = ePdfAnnotationAppearance_Down;
                  break;
            }
            sigField->SetAppearanceStream(&sigXObject, annotApp);

            res = TRUE;
         } catch(const PdfError &error) {
            if (painter.GetPage()) {
               try {
                  painter.FinishPage();
               } catch( ... ) {
               }
            }

            handleException(ctx, "litePDF_SetSignatureAppearance", error);

            DeletePtr(sigField);
            DeletePtr(annotation);

            return FALSE;
         }

         painter.FinishPage();
      } else {
         DeletePtr(sigField);
         DeletePtr(annotation);

         LITEPDF_RETURN_VAL_IF_FAIL (ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_SetSignatureAppearance"), "litePDF_SetSignatureAppearance", FALSE);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SetSignatureAppearance", error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return res;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SetSignatureSize(void *pctx,
                                                       unsigned int requestBytes)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SetSignatureSize: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   ctx->sign_requestBytes = requestBytes;

   return TRUE;
}
//---------------------------------------------------------------------------

static int litepdf_get_openssl_error_cb(const char *str, size_t len, void *user_data)
{
   std::string *pstring = reinterpret_cast<std::string *>(user_data);

   if (!str || !len) {
      return 0;
   }

   if (!pstring) {
      return -2;
   }

   if (!pstring->empty()) {
      (*pstring) = (*pstring) + "\n";
   }

   (*pstring) = (*pstring) + std::string(str, len);

   return len;
}

static std::string getOpenSSLErrors(void)
{
   std::string res = "";

   ERR_print_errors_cb(litepdf_get_openssl_error_cb, &res);

   return res;
}

BOOL __stdcall LITEPDF_PUBLIC litePDF_AddSignerPFX(void *pctx,
                                                   const BYTE *pfxData,
                                                   unsigned int pfxDataLength,
                                                   const char *pfxPassword)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pfxData != NULL, "litePDF_AddSignerPFX", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (pfxDataLength > 0, "litePDF_AddSignerPFX", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_AddSignerPFX: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

	BIO *mem = BIO_new (BIO_s_mem ());
	if (!mem) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_OUTOFMEMORY, "litePDF_AddSignerPFX: Out of memory!", ctx->on_error_user_data);
      }
      return FALSE;
	}

	if ((unsigned int) BIO_write (mem, pfxData, pfxDataLength) != pfxDataLength) {
      std::string errs = getOpenSSLErrors();
      BIO_free (mem);

      if (ctx->on_error) {
         errs = std::string("litePDF_AddSignerPFX") + ": Failed to write PFX data to BIO: " + errs;
         ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
      }

      return FALSE;
   }

   X509 *cert = NULL;
   STACK_OF (X509) *ca = NULL;
   EVP_PKEY *pkey = NULL;
   PKCS12 *p12;

   p12 = d2i_PKCS12_bio (mem, NULL);
   if (!p12) {
      std::string errs = getOpenSSLErrors();
      BIO_free (mem);

      if (ctx->on_error) {
         errs = std::string("litePDF_AddSignerPFX") + ": Failed to read PFX data: " + errs;
         ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
      }

      return FALSE;
   }

   if (!PKCS12_parse (p12, pfxPassword ? pfxPassword : "", &pkey, &cert, &ca)) {
      std::string errs = getOpenSSLErrors();
      PKCS12_free (p12);
      BIO_free (mem);

      if (ctx->on_error) {
         errs = std::string("litePDF_AddSignerPFX") + ": Failed to parse PFX data: " + errs;
         ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
      }
      return FALSE;
   }

   PKCS12_free (p12);
   BIO_free (mem);

	if (ca)
		sk_X509_pop_free(ca, X509_free);

   if (!pkey || !cert) {
      if (cert) {
         X509_free(cert);
      }

      if (pkey) {
         EVP_PKEY_free(pkey);
      }

      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_DATA, "litePDF_AddSignerPFX: Failed to get PFX certificate and private key", ctx->on_error_user_data);
      }
      return FALSE;
   }

   MSignerData signer;
   signer.cert = cert;
   signer.pkey = pkey;
   signer.data_length = pfxDataLength;

   ctx->sign_signers.push_back(signer);

   return TRUE;
}
//---------------------------------------------------------------------------

static int pkey_password_cb(char *buf, int bufsize, int rwflag, void *userdata)
{
   const char *password = reinterpret_cast<const char *>(userdata);

   if (!password) {
      return 0;
   }

   int res = strlen(password);

   if (res > bufsize) {
      res = bufsize;
   }

   memcpy(buf, password, res);

   return res;
}

BOOL __stdcall LITEPDF_PUBLIC litePDF_AddSignerPEM(void *pctx,
                                                   const BYTE *pemData,
                                                   unsigned int pemDataLength,
                                                   const BYTE *pkeyData,
                                                   unsigned int pkeyDataLength,
                                                   const char *pkeyPassword)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (pemData != NULL, "litePDF_AddSignerPEM", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (pemDataLength > 0, "litePDF_AddSignerPEM", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (pkeyData != NULL, "litePDF_AddSignerPEM", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (pkeyDataLength > 0, "litePDF_AddSignerPEM", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_AddSignerPEM: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

	BIO *pemMem = BIO_new (BIO_s_mem ());
	if (!pemMem) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_OUTOFMEMORY, "litePDF_AddSignerPEM: Out of memory!", ctx->on_error_user_data);
      }
      return FALSE;
	}

	BIO *pkeyMem = BIO_new (BIO_s_mem ());
	if (!pkeyMem) {
      BIO_free (pemMem);
      if (ctx->on_error) {
         ctx->on_error(ERROR_OUTOFMEMORY, "litePDF_AddSignerPEM: Out of memory!", ctx->on_error_user_data);
      }
      return FALSE;
	}

	if ((unsigned int) BIO_write (pemMem, pemData, pemDataLength) != pemDataLength) {
      std::string errs = getOpenSSLErrors();
      BIO_free (pemMem);
      BIO_free (pkeyMem);

      if (ctx->on_error) {
         errs = std::string("litePDF_AddSignerPEM") + ": Failed to write PEM data to BIO: " + errs;
         ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
      }

      return FALSE;
   }

	if ((unsigned int) BIO_write (pkeyMem, pkeyData, pkeyDataLength) != pkeyDataLength) {
      std::string errs = getOpenSSLErrors();
      BIO_free (pemMem);
      BIO_free (pkeyMem);

      if (ctx->on_error) {
         errs = std::string("litePDF_AddSignerPEM") + ": Failed to write PKey data to BIO: " + errs;
         ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
      }

      return FALSE;
   }

   X509 *cert;
   EVP_PKEY *pkey;

   cert = PEM_read_bio_X509(pemMem, NULL, pkey_password_cb, const_cast<char *>(pkeyPassword));
   if (!cert) {
      std::string errs = getOpenSSLErrors();
      BIO_free (pemMem);
      BIO_free (pkeyMem);

      if (ctx->on_error) {
         errs = std::string("litePDF_AddSignerPEM") + ": Failed to read PEM data: " + errs;
         ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
      }

      return FALSE;
   }

   pkey = PEM_read_bio_PrivateKey(pkeyMem, NULL, pkey_password_cb, const_cast<char *>(pkeyPassword));
   if (!cert) {
      std::string errs = getOpenSSLErrors();
      BIO_free (pemMem);
      BIO_free (pkeyMem);
      X509_free(cert);

      if (ctx->on_error) {
         errs = std::string("litePDF_AddSignerPEM") + ": Failed to read PKey data: " + errs;
         ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
      }

      return FALSE;
   }

   BIO_free (pemMem);
   BIO_free (pkeyMem);

   MSignerData signer;
   signer.cert = cert;
   signer.pkey = pkey;
   signer.data_length = pemDataLength + pkeyDataLength;

   ctx->sign_signers.push_back(signer);

   return TRUE;
}
//---------------------------------------------------------------------------

static BOOL signToOutputDevice(litePDFContext *ctx,
                               const char *where,
                               unsigned int signatureIndex,
                               litePDF_appendSignatureDataFunc appendSignatureData,
                               void *append_user_data,
                               litePDF_finishSignatureFunc finishSignature,
                               void *finish_user_data,
                               PdfSignOutputDevice &signer,
                               unsigned int forceSigSize)
{
   LITEPDF_RETURN_VAL_IF_FAIL (appendSignatureData != NULL, where, FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (finishSignature != NULL, where, FALSE);

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         std::string buff = std::string(where) + ": Context doesn't contain memory-based PDF document";
         ctx->on_error(ERROR_INVALID_HANDLE, buff.c_str(), ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         std::string buff = std::string(where) + ": Document was already saved";
         ctx->on_error(ERROR_INVALID_OPERATION, buff.c_str(), ctx->on_error_user_data);
      }
      return FALSE;
   }

   PdfAnnotation *annotation = NULL;
   PdfSignatureField *sigField = NULL;
   BOOL res = FALSE;

   try {
      if (!ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": The document has no pages";
            ctx->on_error(ERROR_INVALID_HANDLE, buff.c_str(), ctx->on_error_user_data);
         }
         return FALSE;
      }

      if (ctx->mem_document->GetPdfVersion() < ePdfVersion_1_5) {
         ctx->mem_document->SetPdfVersion(ePdfVersion_1_5);
      }

      if (forceSigSize) {
         signer.SetSignatureSize(forceSigSize);
      } else if (ctx->sign_requestBytes) {
         signer.SetSignatureSize(ctx->sign_requestBytes);
      } else {
         signer.SetSignatureSize(2048);
      }

      PdfAcroForm *form = ctx->mem_document->GetAcroForm(ePdfDontCreateObject, ePdfAcroFormDefaultAppearance_None);
      if (form && form->GetObject()) {
         if (!form->GetObject()->GetDictionary().HasKey(PdfName("SigFlags")) ||
             !form->GetObject()->GetDictionary().GetKey(PdfName("SigFlags"))->IsNumber() ||
             form->GetObject()->GetDictionary().GetKeyAsLong(PdfName("SigFlags")) != 3) {
            if (form->GetObject()->GetDictionary().HasKey(PdfName("SigFlags"))) {
               form->GetObject()->GetDictionary().RemoveKey(PdfName("SigFlags"));
            }

            pdf_int64 val = 3;
            form->GetObject()->GetDictionary().AddKey(PdfName("SigFlags"), PdfObject(val));
         }

         if (form->GetNeedAppearances()) {
            #if 0 /* TODO */
            update_default_appearance_streams(form);
            #endif

            form->SetNeedAppearances(false);
         }
      } else {
         // Disable default appearance
         form = ctx->mem_document->GetAcroForm(ePdfCreateObject, ePdfAcroFormDefaultAppearance_None);
         if (form) {
            form->GetObject()->GetDictionary().AddKey("SigFlags", PdfVariant(static_cast<pdf_int64>(3)));
         }
      }

      if (!form) {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": Failed to create an AcroForm";
            ctx->on_error(ERROR_INVALID_HANDLE, buff.c_str(), ctx->on_error_user_data);
         }
         return FALSE;
      }

      unsigned int errorCode = 0;

      sigField = GetSignatureField(ctx, ctx->mem_document, signatureIndex, true, &annotation, &errorCode);
      if (sigField) {
         sigField->EnsureSignatureObject();
         sigField->SetSignature(*signer.GetSignatureBeacon());

         if (sigField->GetSignatureObject() &&
             sigField->GetSignatureObject()->IsDictionary() &&
             !sigField->GetSignatureObject()->GetDictionary().HasKey(PdfName("M"))) {
            // Set time of signing
            sigField->SetSignatureDate(PdfDate());
         }

         if (ctx->drawnData) {
            ctx->mem_document->EmbedSubsetFonts();
         }
         ctx->documentSaved = true;
         if (ctx->loadedForUpdate) {
            ctx->mem_document->WriteUpdate(&signer);
         } else {
            ctx->mem_document->Write(&signer);
         }

         if (!signer.HasSignaturePosition()) {
            DeletePtr(sigField);
            DeletePtr(annotation);

            if (ctx->on_error) {
               std::string buff = std::string(where) + ": Cannot find signature position in document data";
               ctx->on_error(ERROR_INVALID_HANDLE, buff.c_str(), ctx->on_error_user_data);
            }
            return FALSE;
         }

         // Adjust ByteRange for signature
         signer.AdjustByteRange();

         // Read data for signature and count it
         // We seek at the beginning of the file
         signer.Seek(0);

         unsigned int buffLen = PDF_MAX ((unsigned int) signer.GetSignatureSize(), 65535u), len;
         char *buff;

         while (buff = (char *) malloc(sizeof(char) * buffLen), !buff) {
            buffLen = buffLen / 2;
            if (!buffLen || buffLen < signer.GetSignatureSize())
               break;
         }

         if (!buff) {
            PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
         }

         while ((len = signer.ReadForSignature(buff, buffLen)) > 0) {
            appendSignatureData(buff, len, append_user_data);
         }

         len = buffLen;
         finishSignature(buff, &len, finish_user_data);

         if (len > 0) {
            if (len > signer.GetSignatureSize()) {
               free (buff);
               std::ostringstream oss;
               oss << "Requires at least " << len << " bytes for the signature, but reserved is only " << signer.GetSignatureSize() << " bytes";

               PODOFO_RAISE_ERROR_INFO( ePdfError_ValueOutOfRange, oss.str().c_str() );
            }

            PdfData signature(buff, len);
            signer.SetSignature(signature);
         }

         free (buff);

         signer.Flush();

         return TRUE;
      } else {
         DeletePtr(sigField);
         DeletePtr(annotation);

         LITEPDF_RETURN_VAL_IF_FAIL (ClaimGetSignatureFieldError(ctx, errorCode, "litePDF_SetSignatureReason"), "litePDF_SetSignatureReason", FALSE);
      }
   } catch (const PdfError &error) {
      handleException(ctx, where, error);

      DeletePtr(sigField);
      DeletePtr(annotation);

      return FALSE;
   }

   DeletePtr(sigField);
   DeletePtr(annotation);

   return FALSE;
}
//---------------------------------------------------------------------------

class MSignContext
{
 private:
   PKCS7 *p7;
   BIO *p7bio;
   unsigned int flags;
 public:
   litePDFContext *ctx;
   const char *where;
   unsigned int reserveBytes;

   MSignContext(litePDFContext *mctx,
                const char *mwhere)
   {
      ctx = mctx;
      where = mwhere;
      p7 = NULL;
      p7bio = NULL;
      reserveBytes = 0;
      flags = PKCS7_DETACHED | PKCS7_BINARY;
   }

   ~MSignContext()
   {
      clear();
   }

   void clear(void)
   {
      if (p7) {
         PKCS7_free(p7);
         p7 = NULL;
      }

      if (p7bio) {
         BIO_free_all(p7bio);
         p7bio = NULL;
      }
   }

   bool prepare(void)
   {
      if (!ctx) {
         return false;
      }

      if (ctx->sign_signers.empty()) {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": No signer specified";
            ctx->on_error(ERROR_INVALID_OPERATION, buff.c_str(), ctx->on_error_user_data);
         }
         return false;
      }

      p7 = PKCS7_new();
      if (!p7) {
         if (ctx->on_error) {
            std::string buff = std::string(where) + ": Out of memory!";
            ctx->on_error(ERROR_OUTOFMEMORY, buff.c_str(), ctx->on_error_user_data);
         }
         return false;
      }

	   if (!PKCS7_set_type(p7, NID_pkcs7_signed) ||
	       !PKCS7_content_new(p7, NID_pkcs7_data)) {
         std::string errs = getOpenSSLErrors();

         clear();

         if (ctx->on_error) {
            errs = std::string(where) + ": Failed to initialize signature context: " + errs;
            ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
         }

         return false;
      }

      reserveBytes = 0;
      std::vector<MSignerData>::iterator it, end = ctx->sign_signers.end();
      for (it = ctx->sign_signers.begin(); it != end; it++) {
         if (!PKCS7_sign_add_signer(p7, it->cert, it->pkey, NULL, 0/*!reserveBytes ? 0 : PKCS7_REUSE_DIGEST*/)) {
            std::string errs = getOpenSSLErrors();

            clear();

            if (ctx->on_error) {
               errs = std::string(where) + ": Failed to add signer: " + errs;
               ctx->on_error(ERROR_INVALID_DATA, errs.c_str(), ctx->on_error_user_data);
            }

            return false;
         }

         reserveBytes += it->data_length;
      }

      PKCS7_set_detached(p7, 1);

      p7bio = PKCS7_dataInit(p7, NULL);
      if (!p7bio) {
         std::string errs = getOpenSSLErrors();

         clear();

         if (ctx->on_error) {
            errs = std::string(where) + ": Failed to allocate P7 BIO: " + errs;
            ctx->on_error(ERROR_OUTOFMEMORY, errs.c_str(), ctx->on_error_user_data);
         }

         return false;
      }

      return true;
   }

   void addData(const char *bytes,
                unsigned int bytes_len)
   {
      if (!ctx || !p7bio) {
         return;
      }

      if (!bytes || !bytes_len) {
         return;
      }

	   BIO_write(p7bio, bytes, bytes_len);
	   BIO_flush(p7bio);
   }

   void finishData(char *signature,
                   unsigned int *signature_len)
   {
      if (!ctx || !p7bio || !p7 || !signature || !signature_len) {
         return;
      }

	   BIO_flush(p7bio);

      if (!PKCS7_dataFinal(p7, p7bio)) {
         std::string errs = getOpenSSLErrors();

         clear();

         *signature_len = 0;

         if (ctx->on_error) {
            errs = std::string(where) + ": Failed to finish digital sign: " + errs;
            ctx->on_error(ERROR_OUTOFMEMORY, errs.c_str(), ctx->on_error_user_data);
         }

         return;
      }

      BIO *out = BIO_new(BIO_s_mem());
      if (!out) {
         std::string errs = getOpenSSLErrors();

         clear();

         *signature_len = 0;

         if (ctx->on_error) {
            errs = std::string(where) + ": Failed to allocated output BIO: " + errs;
            ctx->on_error(ERROR_OUTOFMEMORY, errs.c_str(), ctx->on_error_user_data);
         }

         return;
      }

      char *outBuff = NULL;
      long outLen;

      i2d_PKCS7_bio(out, p7);

      outLen = BIO_get_mem_data(out, &outBuff);

      if (outLen > 0 && outLen < *signature_len && outBuff) {
         *signature_len = outLen;
         memcpy(signature, outBuff, outLen);
      } else {
         *signature_len = 0;
      }

      BIO_free(out);

      clear();
   }
};
//---------------------------------------------------------------------------

static void __stdcall internalAppendSignatureDataCB(const char *bytes,
                                                    unsigned int bytes_len,
                                                    void *user_data)
{
   MSignContext *signContext = reinterpret_cast<MSignContext *>(user_data);
   if (signContext) {
      signContext->addData(bytes, bytes_len);
   }
}
//---------------------------------------------------------------------------

static void __stdcall internalFinishSignatureCB(char *signature,
                                                unsigned int *signature_len,
                                                void *user_data)
{
   MSignContext *signContext = reinterpret_cast<MSignContext *>(user_data);
   if (signContext) {
      signContext->finishData(signature, signature_len);
   }
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSign(void *pctx,
                                                         const char *fileName,
                                                         unsigned int signatureIndex)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_SaveToFileWithSign", FALSE);

   try {
      PdfOutputDevice outputDevice(fileName);
      PdfSignOutputDevice signer(&outputDevice);

      MSignContext signCtx(ctx, "litePDF_SaveToFileWithSign");

      if (!signCtx.prepare()) {
         return FALSE;
      }

      return signToOutputDevice(ctx, "litePDF_SaveToFileWithSign", signatureIndex,
         internalAppendSignatureDataCB, &signCtx,
         internalFinishSignatureCB, &signCtx,
         signer, signCtx.reserveBytes);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SaveToFileWithSign", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSignW(void *pctx,
                                                          const wchar_t *fileName,
                                                          unsigned int signatureIndex)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_SaveToFileWithSignW", FALSE);

   try {
      PdfOutputDevice outputDevice(fileName);
      PdfSignOutputDevice signer(&outputDevice);

      MSignContext signCtx(ctx, "litePDF_SaveToFileWithSignW");

      if (!signCtx.prepare()) {
         return FALSE;
      }

      return signToOutputDevice(ctx, "litePDF_SaveToFileWithSignW", signatureIndex,
         internalAppendSignatureDataCB, &signCtx,
         internalFinishSignatureCB, &signCtx,
         signer, signCtx.reserveBytes);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SaveToFileWithSignW", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToDataWithSign(void *pctx,
                                                         unsigned int signatureIndex,
                                                         BYTE *data,
                                                         unsigned int *dataLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (dataLength != NULL, "litePDF_SaveToDataWithSign", FALSE);

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SaveToDataWithSign: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (!ctx->signedSavedData) {
         if (ctx->documentSaved) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SaveToDataWithSign: Document was already saved", ctx->on_error_user_data);
            }
            return FALSE;
         }

         ctx->signedSavedData = new PdfRefCountedBuffer();
         if (!ctx->signedSavedData) {
            PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
         }

         PdfOutputDevice outputDevice(ctx->signedSavedData);
         PdfSignOutputDevice signer(&outputDevice);

         MSignContext signCtx(ctx, "litePDF_SaveToDataWithSign");

         if (!signCtx.prepare()) {
            return FALSE;
         }

         BOOL result = signToOutputDevice(ctx,
            "litePDF_SaveToDataWithSign", signatureIndex,
            internalAppendSignatureDataCB, &signCtx,
            internalFinishSignatureCB, &signCtx,
            signer, signCtx.reserveBytes);

         ctx->documentSaved = true;

         if (!result) {
            return result;
         }
      }

      if (data) {
         if (*dataLength >= ctx->signedSavedData->GetSize()) {
            memcpy(data, ctx->signedSavedData->GetBuffer(), ctx->signedSavedData->GetSize());
            *dataLength = ctx->signedSavedData->GetSize();
         } else {
            if (ctx->on_error) {
               ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_SaveToDataWithSign: Data buffer is too small", ctx->on_error_user_data);
            }
            return FALSE;
         }
      } else {
         *dataLength = ctx->signedSavedData->GetSize();
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SaveToDataWithSign", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSignManual(void *pctx,
                                                               const char *fileName,
                                                               unsigned int signatureIndex,
                                                               litePDF_appendSignatureDataFunc appendSignatureData,
                                                               void *append_user_data,
                                                               litePDF_finishSignatureFunc finishSignature,
                                                               void *finish_user_data)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_SaveToFileWithSignManual", FALSE);

   try {
      PdfOutputDevice outputDevice(fileName);
      PdfSignOutputDevice signer(&outputDevice);

      return signToOutputDevice(ctx, "litePDF_SaveToFileWithSignManual", signatureIndex,
         appendSignatureData, append_user_data,
         finishSignature, finish_user_data,
         signer, 0);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SaveToFileWithSignManual", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToFileWithSignManualW(void *pctx,
                                                                const wchar_t *fileName,
                                                                unsigned int signatureIndex,
                                                                litePDF_appendSignatureDataFunc appendSignatureData,
                                                                void *append_user_data,
                                                                litePDF_finishSignatureFunc finishSignature,
                                                                void *finish_user_data)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_SaveToFileWithSignManualW", FALSE);

   try {
      PdfOutputDevice outputDevice(fileName);
      PdfSignOutputDevice signer(&outputDevice);

      return signToOutputDevice(ctx, "litePDF_SaveToFileWithSignManualW", signatureIndex,
         appendSignatureData, append_user_data,
         finishSignature, finish_user_data,
         signer, 0);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SaveToFileWithSignManualW", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_SaveToDataWithSignManual(void *pctx,
                                                               unsigned int signatureIndex,
                                                               litePDF_appendSignatureDataFunc appendSignatureData,
                                                               void *append_user_data,
                                                               litePDF_finishSignatureFunc finishSignature,
                                                               void *finish_user_data,
                                                               BYTE *data,
                                                               unsigned int *dataLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (dataLength != NULL, "litePDF_SaveToDataWithSignManual", FALSE);

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_SaveToDataWithSignManual: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (!ctx->signedSavedData) {
         if (ctx->documentSaved) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_SaveToDataWithSignManual: Document was already saved", ctx->on_error_user_data);
            }
            return FALSE;
         }

         ctx->signedSavedData = new PdfRefCountedBuffer();
         if (!ctx->signedSavedData) {
            PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
         }

         PdfOutputDevice outputDevice(ctx->signedSavedData);
         PdfSignOutputDevice signer(&outputDevice);

         BOOL result = signToOutputDevice(ctx,
            "litePDF_SaveToDataWithSignManual", signatureIndex,
            appendSignatureData, append_user_data,
            finishSignature, finish_user_data,
            signer, 0);

         ctx->documentSaved = true;

         if (!result) {
            return result;
         }
      }

      if (data) {
         if (*dataLength >= ctx->signedSavedData->GetSize()) {
            memcpy(data, ctx->signedSavedData->GetBuffer(), ctx->signedSavedData->GetSize());
            *dataLength = ctx->signedSavedData->GetSize();
         } else {
            if (ctx->on_error) {
               ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_SaveToDataWithSignManual: Data buffer is too small", ctx->on_error_user_data);
            }
            return FALSE;
         }
      } else {
         *dataLength = ctx->signedSavedData->GetSize();
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_SaveToDataWithSignManual", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

static const PdfEncoding *getNativeEncoding (litePDFContext *ctx,
                                             BYTE lfCharSet,
                                             PdfDocument *overrideDocument)
{
   if (lfCharSet == DEFAULT_CHARSET) {
      return PdfEncodingFactory::GlobalIdentityEncodingInstance();
   }

   if (!ctx || !ctx->encodingsCache) {
      return PdfEncodingFactory::GlobalWinAnsiEncodingInstance();
   }

   const PdfEncoding *nativeEncoding = ctx->encodingsCache->getEncodingForCharSet(lfCharSet, overrideDocument);

   if (!nativeEncoding) {
      nativeEncoding = PdfEncodingFactory::GlobalWinAnsiEncodingInstance();
      ctx->encodingsCache->put_encodingReplacement(lfCharSet, nativeEncoding);

      if (ctx->on_error) {
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
         ctx->on_error(ERROR_NOT_SUPPORTED, buff, ctx->on_error_user_data);
      }
   }

   return nativeEncoding;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedFile(void *pctx,
                                                const char *fileName)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_EmbedFile", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_EmbedFile: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedFile: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   wchar_t *wbuff = NULL;
   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedFile: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      int wrote;

      wrote = MultiByteToWideChar(CP_ACP, 0, fileName, strlen(fileName), NULL, 0);
      wbuff = (wchar_t *) malloc (sizeof(wchar_t) * (1 + wrote));
      if (!wbuff) {
         PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
      }

      wrote = MultiByteToWideChar(CP_ACP, 0, fileName, strlen(fileName), wbuff, wrote);
      wbuff[wrote] = 0;

      PdfFileSpec fileSpec(wbuff, true, document, true);
      document->AttachFile(fileSpec);
   } catch (const PdfError &error) {
      if (wbuff) {
         free (wbuff);
      }
      handleException(ctx, "litePDF_EmbedFile", error);
      return FALSE;
   }

   if (wbuff) {
      free (wbuff);
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedFileW(void *pctx,
                                                 const wchar_t *fileName)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_EmbedFileW", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_EmbedFileW: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedFileW: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedFileW: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      PdfFileSpec fileSpec(fileName, true, document, true);
      document->AttachFile(fileSpec);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_EmbedFileW", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedData(void *pctx,
                                                const char *fileName,
                                                const BYTE *data,
                                                unsigned int dataLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_EmbedData", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (data != NULL, "litePDF_EmbedData", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_EmbedData: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedData: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   wchar_t *wbuff = NULL;
   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedData: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      int wrote;

      wrote = MultiByteToWideChar(CP_ACP, 0, fileName, strlen(fileName), NULL, 0);
      wbuff = (wchar_t *) malloc (sizeof(wchar_t) * (1 + wrote));
      if (!wbuff) {
         PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
      }

      wrote = MultiByteToWideChar(CP_ACP, 0, fileName, strlen(fileName), wbuff, wrote);
      wbuff[wrote] = 0;

      PdfFileSpec fileSpec(wbuff, data, dataLength, document, true);
      document->AttachFile(fileSpec);
   } catch (const PdfError &error) {
      if (wbuff) {
         free (wbuff);
      }
      handleException(ctx, "litePDF_EmbedData", error);
      return FALSE;
   }

   if (wbuff) {
      free (wbuff);
   }

   return TRUE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_EmbedDataW(void *pctx,
                                                 const wchar_t *fileName,
                                                 const BYTE *data,
                                                 unsigned int dataLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileName != NULL, "litePDF_EmbedDataW", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (data != NULL, "litePDF_EmbedDataW", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_EmbedDataW: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   if (!ctx->mem_document) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedDataW: Context doesn't contain memory-based PDF document", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_EmbedDataW: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      PdfFileSpec fileSpec(fileName, data, dataLength, document, true);
      document->AttachFile(fileSpec);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_EmbedDataW", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

INT __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileCount(void *pctx)
{
   if (!pctx) {
      return -1;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetEmbeddedFileCount: Document was already saved", ctx->on_error_user_data);
      }
      return -1;
   }

   int res = 0;

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetEmbeddedFileCount: No document is opened", ctx->on_error_user_data);
         }

         return -1;
      }

      PdfNamesTree *names = document->GetNamesTree(false);
      if (names) {
         PdfObject *pObj = names->GetObject()->GetIndirectKey("EmbeddedFiles");
         if (pObj) {
            const PdfObject *kidsKey = pObj->GetIndirectKey("Kids");
            if (kidsKey) {
               const PdfArray &kids = kidsKey->GetArray();
               PdfArray::const_iterator kit, kend = kids.end();

               for (kit = kids.begin(); kit != kend; kit++) {
                  const PdfObject *obj = &(*kit);
                  if (obj->IsReference()) {
                     obj = names->GetObject()->GetOwner()->GetObject(obj->GetReference());
                     if (!obj) {
                        continue;
                     }
                  }

                  const PdfObject *namesKey = obj->GetIndirectKey("Names");
                  if (namesKey) {
                     res += namesKey->GetArray().GetSize();
                  }
               }

               // pair of PdfString/PdfObject, thus divide by two
               res = res / 2;
            }
         }
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetEmbeddedFileCount", error);
      return -1;
   }

   return res;
}
//---------------------------------------------------------------------------

static const PdfObject *
get_embedded_filespec (litePDFContext *ctx,
                       PdfDocument *document,
                       unsigned int index)
{
   if (!ctx) {
      return NULL;
   }

   LITEPDF_RETURN_VAL_IF_FAIL (document != NULL, "get_embedded_filespec", NULL);

   PdfNamesTree *names = document->GetNamesTree(false);
   if (names) {
      PdfObject *pObj = names->GetObject()->GetIndirectKey("EmbeddedFiles");
      if (pObj) {
         const PdfObject *kidsKey = pObj->GetIndirectKey("Kids");
         if (kidsKey) {
            const PdfArray &kids = kidsKey->GetArray();
            PdfArray::const_iterator kit, kend = kids.end();

            for (kit = kids.begin(); kit != kend; kit++) {
               const PdfObject *obj = &(*kit);
               if (obj->IsReference()) {
                  obj = document->GetObjects()->GetObject(obj->GetReference());
                  if (!obj) {
                     continue;
                  }
               }

               const PdfObject *namesKey = obj->GetIndirectKey("Names");
               if (namesKey) {
                  const PdfArray &names = namesKey->GetArray();
                  // pair of PdfString/PdfObject, thus divide by two
                  if (names.GetSize() / 2 > index) {
                     PdfArray::const_iterator nit, nend = names.end();
                     for (nit = names.begin(); nit != nend;) {
                        if (index == 0) {
                           nit++;

                           const PdfObject *obj = &(*nit);
                           if (obj && obj->IsReference()) {
                              obj = document->GetObjects()->GetObject(obj->GetReference());
                           }

                           if (obj && obj->IsDictionary()) {
                              const PdfDictionary &dict = obj->GetDictionary();
                              if (dict.HasKey("Type") &&
                                  dict.GetKey("Type")->GetName() == PdfName("Filespec") &&
                                  dict.HasKey("EF") &&
                                  dict.GetKey("EF")->GetDictionary().HasKey("F")) {
                                 /* ok, it's an embedded file with stream */
                              } else {
                                 obj = NULL;
                              }
                           } else {
                              obj = NULL;
                           }

                           return obj;
                        }
                        index--;
                        nit++;
                        nit++;
                     }
                     break;
                  } else {
                     index -= names.GetSize() / 2;
                  }
               }
            }
         }
      }
   }

   return NULL;
}

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileName(void *pctx,
                                                          unsigned int index,
                                                          char *fileName,
                                                          unsigned int *fileNameLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileNameLength != NULL, "litePDF_GetEmbeddedFileName", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetEmbeddedFileName: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetEmbeddedFileName: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *obj = get_embedded_filespec (ctx, document, index);
      if (obj) {
         std::string name;

         if (obj->GetDictionary().HasKey("UF")) {
            std::wstring wname = obj->GetDictionary().GetKey("UF")->GetString().GetStringW();

            int wrote = WideCharToMultiByte(CP_ACP, 0, wname.c_str(), wname.size(), NULL, 0, NULL, NULL);
            char *buff = (char *) malloc (sizeof(char) * (1 + wrote));
            if (!buff) {
               PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
            }

            wrote = WideCharToMultiByte(CP_ACP, 0, wname.c_str(), wname.size(), buff, wrote, NULL, NULL);
            buff[wrote] = 0;

            name = buff;
            free(buff);
         } else if (obj->GetDictionary().HasKey("F")) {
            name = obj->GetDictionary().GetKey("F")->GetString().GetString();
         } else {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_PARAMETER, "litePDF_GetEmbeddedFileName: Missing file name specifier", ctx->on_error_user_data);
            }

            return FALSE;
         }

         if (!fileName) {
            *fileNameLength = name.size();
         } else {
            if (*fileNameLength >= name.size() + 1) {
               memcpy(fileName, name.c_str(), name.size() + 1);
               *fileNameLength = name.size();
            } else {
               if (ctx->on_error) {
                  ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetEmbeddedFileName: Data buffer is too small", ctx->on_error_user_data);
               }

               return FALSE;
            }
         }

         return TRUE;
      }

      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetEmbeddedFileName: Index is out of range", ctx->on_error_user_data);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetEmbeddedFileName", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileNameW(void *pctx,
                                                           unsigned int index,
                                                           wchar_t *fileName,
                                                           unsigned int *fileNameLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (fileNameLength != NULL, "litePDF_GetEmbeddedFileNameW", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetEmbeddedFileNameW: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetEmbeddedFileNameW: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *obj = get_embedded_filespec (ctx, document, index);
      if (obj) {
         std::wstring wname;

         if (obj->GetDictionary().HasKey("UF")) {
            wname = obj->GetDictionary().GetKey("UF")->GetString().GetStringW();
         } else if (obj->GetDictionary().HasKey("F")) {
            std::string name = obj->GetDictionary().GetKey("F")->GetString().GetString();

            int wrote = MultiByteToWideChar(CP_ACP, 0, name.c_str(), name.size(), NULL, 0);
            wchar_t *wbuff = (wchar_t *) malloc (sizeof(wchar_t) * (1 + wrote));
            if (!wbuff) {
               PODOFO_RAISE_ERROR (ePdfError_OutOfMemory);
            }

            wrote = MultiByteToWideChar(CP_ACP, 0, name.c_str(), name.size(), wbuff, wrote);
            wbuff[wrote] = 0;

            wname = wbuff;
            free(wbuff);
         } else {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_PARAMETER, "litePDF_GetEmbeddedFileNameW: Missing file name specifier", ctx->on_error_user_data);
            }

            return FALSE;
         }

         if (!fileName) {
            *fileNameLength = wname.size();
         } else {
            if (*fileNameLength >= wname.size() + 1) {
               memcpy(fileName, wname.c_str(), (wname.size() + 1) * sizeof(wchar_t));
               *fileNameLength = wname.size();
            } else {
               if (ctx->on_error) {
                  ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetEmbeddedFileNameW: Data buffer is too small", ctx->on_error_user_data);
               }

               return FALSE;
            }
         }

         return TRUE;
      }

      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetEmbeddedFileNameW: Index is out of range", ctx->on_error_user_data);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetEmbeddedFileNameW", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_GetEmbeddedFileData(void *pctx,
                                                          unsigned int index,
                                                          BYTE *data,
                                                          unsigned int *dataLength)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   LITEPDF_RETURN_VAL_IF_FAIL (dataLength != NULL, "litePDF_GetEmbeddedFileData", FALSE);

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_GetEmbeddedFileData: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      PdfDocument *document = NULL;

      if (ctx->streamed_document) {
         document = ctx->streamed_document;
      } else if (ctx->mem_document) {
         document = ctx->mem_document;
      } else {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetEmbeddedFileData: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      const PdfObject *obj = get_embedded_filespec (ctx, document, index);
      if (obj) {
         const PdfDictionary &dict = obj->GetDictionary();
         if (dict.HasKey("Type") &&
             dict.GetKey("Type")->GetName() == PdfName("Filespec") &&
             dict.HasKey("EF") &&
             dict.GetKey("EF")->GetDictionary().HasKey("F")) {
            obj = dict.GetKey("EF")->GetDictionary().GetKey("F");
            if (obj && obj->IsReference()) {
               obj = document->GetObjects()->GetObject(obj->GetReference());
            }
         } else {
            obj = NULL;
         }

         pdf_int64 streamSize = 0;
         const PdfStream *stream = NULL;

         if (obj) {
            const PdfDictionary &dict = obj->GetDictionary();
            if (dict.HasKey("Type") &&
                dict.GetKey("Type")->GetName() == PdfName("EmbeddedFile") &&
                dict.HasKey("Params") &&
                dict.GetKey("Params")->GetDictionary().HasKey("Size") &&
                dict.GetKey("Params")->GetDictionary().GetKey("Size")->IsNumber() &&
                obj->HasStream()) {
               // get the stream
               streamSize = dict.GetKey("Params")->GetDictionary().GetKey("Size")->GetNumber();
               stream = obj->GetStream();
            }
         }

         if (!obj) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetEmbeddedFileData: Failed to get embedded file object", ctx->on_error_user_data);
            }

            return FALSE;
         }

         if (!stream) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetEmbeddedFileData: Failed to get embedded file stream", ctx->on_error_user_data);
            }

            return FALSE;
         }

         if (!data) {
            *dataLength = streamSize;
         } else {
            if (*dataLength >= streamSize) {
               char *buff = NULL;
               pdf_long buffLength = 0;

               stream->GetFilteredCopy(&buff, &buffLength);

               if (buff && (unsigned int) buffLength <= *dataLength) {
                  memcpy(data, buff, buffLength);
               } else {
                  if (ctx->on_error) {
                     ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetEmbeddedFileData: Data buffer is too small", ctx->on_error_user_data);
                  }

                  return FALSE;
               }

               *dataLength = (unsigned int) buffLength;

               if (buff) {
                  free(buff);
               }
            } else {
               if (ctx->on_error) {
                  ctx->on_error(ERROR_NOT_ENOUGH_MEMORY, "litePDF_GetEmbeddedFileData: Data buffer is too small", ctx->on_error_user_data);
               }

               return FALSE;
            }
         }

         return TRUE;
      }

      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_INDEX, "litePDF_GetEmbeddedFileData: Index is out of range", ctx->on_error_user_data);
      }
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_GetEmbeddedFileData", error);
      return FALSE;
   }

   return FALSE;
}
//---------------------------------------------------------------------------

void * __stdcall LITEPDF_PUBLIC litePDF_GetPoDoFoDocument(void *pctx)
{
   if (!pctx) {
      return NULL;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   PdfDocument *document = NULL;

   if (ctx->streamed_document) {
      document = ctx->streamed_document;
   } else if (ctx->mem_document) {
      document = ctx->mem_document;
   } else {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_GetPoDoFoDocument: No document is opened", ctx->on_error_user_data);
      }

      return NULL;
   }

   return (void *) document;
}
//---------------------------------------------------------------------------

BOOL __stdcall LITEPDF_PUBLIC litePDF_DrawDebugPage(void *pctx,
                                                    const char *filename)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;
   PdfDocument *document;

   LITEPDF_RETURN_VAL_IF_FAIL (filename != NULL, "litePDF_DrawDebugPage", FALSE);
   LITEPDF_RETURN_VAL_IF_FAIL (ctx->currentDraw.hdc == NULL, "litePDF_DrawDebugPage", FALSE);

   document = ctx->streamed_document ? static_cast<PdfDocument *>(ctx->streamed_document) : static_cast<PdfDocument *>(ctx->mem_document);
   LITEPDF_RETURN_VAL_IF_FAIL (document != NULL, "litePDF_DrawDebugPage", FALSE);

   FILE *f = fopen(filename, "rb");
   if (!f) {
      if (ctx->on_error) {
         ctx->on_error (ERROR_INVALID_HANDLE, "litePDF_DrawDebugPage: Failed to open debug page file", ctx->on_error_user_data);
      }
      return FALSE;
   }

   std::string line;
   char c;
   while (c = fgetc(f), !feof(f) && c != '\n') {
      line += c;
   }

   if (feof(f) || line.find("litePDF-page: ") != 0) {
      if (ctx->on_error) {
         ctx->on_error (ERROR_INVALID_HANDLE, "litePDF_DrawDebugPage: Incorrect debug page file", ctx->on_error_user_data);
      }
      fclose (f);
      return FALSE;
   }

   int token = 0;
   double ratio = -1.0;
   DBLSIZE page_mm = {0.0, 0.0};
   SIZE page_px = {0, 0};
   unsigned int ii, sz = line.length();
   for (ii = 0; ii < sz; ii++) {
      char c = line[ii];
      if (token == 0) {
         if (c == 'm' && ii + 2 < sz && line[ii + 1] == 'm' && line[ii + 2] == ':') {
            ii += 2;
            token++;
         }
      } else if (token == 1 || token == 2) {
         if (c >= '0' && c <= '9') {
            if (token == 1) {
               page_mm.cx *= 10;
               page_mm.cx += c - '0';
            } else {
               page_mm.cy *= 10;
               page_mm.cy += c - '0';
            }
            if (ratio > 0.0) {
               ratio *= 10.0;
            }
         } else if (c == '.') {
            ratio = 1.0;
         } else if (c == ',' || c == ' ') {
            if (ratio > 0) {
               if (token == 1) {
                  page_mm.cx /= ratio;
               } else {
                  page_mm.cy /= ratio;
               }
               ratio = -1.0;
            }
            token++;
         } else {
            break;
         }
      } else if (token == 3) {
         if (c == 'p' && ii + 2 < sz && line[ii + 1] == 'x' && line[ii + 2] == ':') {
            ii += 2;
            token++;
         }
      } else if (token == 4 || token == 5) {
         if (c >= '0' && c <= '9') {
            if (token == 4) {
               page_px.cx *= 10;
               page_px.cx += c - '0';
            } else {
               page_px.cy *= 10;
               page_px.cy += c - '0';
            }
         } else if (c == ',' || c == ' ') {
            token++;
         } else {
            break;
         }
      } else {
         break;
      }
   }

   if ((token != 5 && token != 6) || !page_px.cy) {
      if (ctx->on_error) {
         ctx->on_error (ERROR_INVALID_HANDLE, "litePDF_DrawDebugPage: Incorrect debug page file - no header found", ctx->on_error_user_data);
      }
      fclose (f);
      return FALSE;
   }

   long pos = ftell(f);
   fseek (f, 0, SEEK_END);

   UINT cb = ftell(f); //longer by header
   BYTE *bits;

   bits = (BYTE *) malloc (sizeof(BYTE) * (cb + 1));
   if (!bits) {
      if (ctx->on_error) {
         ctx->on_error (ERROR_NOT_ENOUGH_MEMORY, "litePDF_DrawDebugPage: Cannot allocate memory for page", ctx->on_error_user_data);
      }
      fclose (f);
      return FALSE;
   }

   fseek (f, pos, SEEK_SET);
   cb = fread (bits, sizeof (BYTE), cb, f);

   HENHMETAFILE emf = SetEnhMetaFileBits(cb, bits);
   if (!emf) {
      if (ctx->on_error) {
         ctx->on_error (GetLastError(), "litePDF_DrawDebugPage: Incorrect page bits", ctx->on_error_user_data);
      }
      fclose (f);
      free (bits);
      return FALSE;
   }

   free (bits);
   fclose (f);

   bool canPaint = false;
   PdfPainter painter;
   PdfPage *pPage;

   try {
      pPage = document->CreatePage(PdfRect(0, 0, LITEPDF_MM_TO_POINTS (page_mm.cx), LITEPDF_MM_TO_POINTS (page_mm.cy)));
      canPaint = true;
   } catch(const PdfError &error) {
      handleException(ctx, "litePDF_DrawDebugPage", error);
   }

   if (canPaint) {
      try {
         painter.SetPage(pPage);
         PlayMeta2Pdf(emf, page_mm, page_px, document, canPaint ? &painter : NULL, ctx->encodingsCache, LITEPDF_DRAW_FLAG_NONE, ctx->on_error, ctx->on_error_user_data);
      } catch (const PdfError &error) {
         handleException(ctx, "litePDF_DrawDebugPage", error);
      }

      painter.FinishPage();
   }

   DeleteEnhMetaFile (emf);

   return TRUE;
}
//---------------------------------------------------------------------------

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
                                                           const wchar_t *destinationDescription)
{
   if (!pctx) {
      return FALSE;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_CreateLinkAnnotation: Document was already saved", ctx->on_error_user_data);
      }
      return FALSE;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_CreateLinkAnnotation: A draw operation is in progress", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateLinkAnnotation: No document is opened", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateLinkAnnotation: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (annotationPageIndex >= ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateLinkAnnotation: Annotation page index is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      if (destinationPageIndex >= ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateLinkAnnotation: Destination page index is out of range", ctx->on_error_user_data);
         }

         return FALSE;
      }

      PdfPage *page = ctx->mem_document->GetPage(annotationPageIndex);
      if (!page) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateLinkAnnotation: Cannot find page for annotation", ctx->on_error_user_data);
         }
         return FALSE;
      }

      PdfRect onPageRect(page->GetPageSize().GetLeft() + LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationX_u)),
                         page->GetPageSize().GetBottom() + page->GetPageSize().GetHeight() - LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationY_u)),
                         LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationWidth_u)),
                         -LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, annotationHeight_u)));

      PdfObject *annots = page->GetOwnAnnotationsArray(true, ctx->mem_document);

      if (annotationResourceID > 0) {
         if (annotationResourceID - 1 >= (unsigned int) ctx->resources.size()) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateLinkAnnotation: Annotation resource ID is out of range", ctx->on_error_user_data);
            }

            return FALSE;
         }

         PdfAnnotation annotLine(page, ePdfAnnotation_Line, onPageRect, &ctx->mem_document->GetObjects());
         PdfDictionary dict;
         dict.AddKey("N", ctx->resources[annotationResourceID - 1].objectReference);
         annotLine.GetObject()->GetDictionary().AddKey("AP", dict);
         annotLine.SetBorderStyle(0.0, 0.0, 0.0);
         annotLine.SetFlags(annotationFlags);

         if (annots) {
            annots->GetArray().push_back(annotLine.GetObject()->Reference());
         }
      }

      PdfAnnotation annot(page, ePdfAnnotation_Link, onPageRect, &ctx->mem_document->GetObjects());
      if (destinationDescription && *destinationDescription) {
         PdfString str;
         str.setFromWchar_t ((const wchar_t *) destinationDescription);

         annot.SetContents(str);
      }
      annot.SetBorderStyle(0.0, 0.0, 0.0);
      annot.SetFlags(annotationFlags);

      if (annots) {
         annots->GetArray().push_back(annot.GetObject()->Reference());
      }

      page = ctx->mem_document->GetPage(destinationPageIndex);
      if (!page) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateLinkAnnotation: Cannot find destination page", ctx->on_error_user_data);
         }
         return FALSE;
      }

      PdfDestination dest(page, page->GetPageSize().GetLeft() + LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationX_u)),
                         page->GetPageSize().GetBottom() + page->GetPageSize().GetHeight() - LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationY_u)), 0);
      annot.SetDestination(dest);
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_CreateLinkAnnotation", error);
      return FALSE;
   }

   return TRUE;
}
//---------------------------------------------------------------------------

unsigned int __stdcall LITEPDF_PUBLIC litePDF_CreateBookmarkRoot(void *pctx,
                                                                 const wchar_t *title,
                                                                 unsigned int flags,
                                                                 unsigned char titleColor_red,
                                                                 unsigned char titleColor_green,
                                                                 unsigned char titleColor_blue,
                                                                 unsigned int destinationPageIndex,
                                                                 unsigned int destinationX_u,
                                                                 unsigned int destinationY_u)
{
   if (!pctx) {
      return 0;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   unsigned int bookmarkID = 0;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_CreateBookmarkRoot: Document was already saved", ctx->on_error_user_data);
      }
      return 0;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_CreateBookmarkRoot: A draw operation is in progress", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkRoot: No document is opened", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkRoot: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return 0;
      }

      if (destinationPageIndex >= ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkRoot: Destination page index is out of range", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfPage *page = ctx->mem_document->GetPage(destinationPageIndex);
      if (!page) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkRoot: Cannot find destination page", ctx->on_error_user_data);
         }
         return 0;
      }

      PdfOutlines *outlines = ctx->mem_document->GetOutlines();
      if (!outlines) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkRoot: Cannot get table of document bookmarks", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfString str;
      str.setFromWchar_t ((const wchar_t *) title);
      PdfDestination dest(page, page->GetPageSize().GetLeft() + LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationX_u)),
                         page->GetPageSize().GetBottom() + page->GetPageSize().GetHeight() - LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationY_u)), 0);
      PdfOutlineItem *created = outlines->CreateChild(str, dest);
      if (!created) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkRoot: Failed to create document bookmark", ctx->on_error_user_data);
         }

         return 0;
      }

      created->SetTextFormat((EPdfOutlineFormat) (ePdfOutlineFormat_Default |
         ((flags & LITEPDF_BOOKMARK_FLAG_ITALIC) != 0 ? ePdfOutlineFormat_Italic : 0) |
         ((flags & LITEPDF_BOOKMARK_FLAG_BOLD) != 0 ? ePdfOutlineFormat_Bold : 0)));
      created->SetTextColor(titleColor_red / 255.0, titleColor_green / 255.0, titleColor_blue / 255.0);

      ctx->outlines.push_back(created);
      bookmarkID = ctx->outlines.size();
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_CreateBookmarkRoot", error);
      return 0;
   }

   return bookmarkID;
}
//---------------------------------------------------------------------------

static bool checkBookmarkExists(PdfOutlineItem *root,
                                PdfOutlineItem *mayExist)
{
   if (!root || !mayExist) {
      return false;
   }

   PdfOutlineItem *item = root;
   while (item) {
      if (item == mayExist) {
         return true;
      }

      if (item->First() && checkBookmarkExists(item->First(), mayExist)) {
         return true;
      }

      item = item->Next();
   }

   return false;
}
//---------------------------------------------------------------------------

unsigned int __stdcall LITEPDF_PUBLIC litePDF_CreateBookmarkChild(void *pctx,
                                                                  unsigned int parentBookmarkID,
                                                                  const wchar_t *title,
                                                                  unsigned int flags,
                                                                  unsigned char titleColor_red,
                                                                  unsigned char titleColor_green,
                                                                  unsigned char titleColor_blue,
                                                                  unsigned int destinationPageIndex,
                                                                  unsigned int destinationX_u,
                                                                  unsigned int destinationY_u)
{
   if (!pctx) {
      return 0;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   unsigned int bookmarkID = 0;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_CreateBookmarkChild: Document was already saved", ctx->on_error_user_data);
      }
      return 0;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_CreateBookmarkChild: A draw operation is in progress", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkChild: No document is opened", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkChild: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return 0;
      }

      if (destinationPageIndex >= ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkChild: Destination page index is out of range", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfPage *page = ctx->mem_document->GetPage(destinationPageIndex);
      if (!page) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkChild: Cannot find destination page", ctx->on_error_user_data);
         }
         return 0;
      }

      PdfOutlines *outlines = ctx->mem_document->GetOutlines();
      if (!outlines) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkChild: Cannot get table of document bookmarks", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfOutlineItem *createIn;

      if (parentBookmarkID == 0) {
         createIn = outlines;
      } else {
         if (parentBookmarkID - 1 >= (unsigned int) ctx->outlines.size()) {
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkChild: Parent bookmark ID is out of range", ctx->on_error_user_data);
            }

            return 0;
         }

         createIn = ctx->outlines[parentBookmarkID - 1];
         if (!checkBookmarkExists(outlines->First(), createIn)) {
            ctx->outlines[parentBookmarkID - 1] = NULL;
            if (ctx->on_error) {
               ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkChild: Parent bookmark doesn't exist", ctx->on_error_user_data);
            }

            return 0;
         }
      }

      PdfString str;
      str.setFromWchar_t ((const wchar_t *) title);
      PdfDestination dest(page, page->GetPageSize().GetLeft() + LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationX_u)),
                         page->GetPageSize().GetBottom() + page->GetPageSize().GetHeight() - LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationY_u)), 0);
      PdfOutlineItem *created = createIn->CreateChild(str, dest);
      if (!created) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkChild: Failed to create document bookmark", ctx->on_error_user_data);
         }

         return 0;
      }

      created->SetTextFormat((EPdfOutlineFormat) (ePdfOutlineFormat_Default |
         ((flags & LITEPDF_BOOKMARK_FLAG_ITALIC) != 0 ? ePdfOutlineFormat_Italic : 0) |
         ((flags & LITEPDF_BOOKMARK_FLAG_BOLD) != 0 ? ePdfOutlineFormat_Bold : 0)));
      created->SetTextColor(titleColor_red / 255.0, titleColor_green / 255.0, titleColor_blue / 255.0);

      ctx->outlines.push_back(created);
      bookmarkID = ctx->outlines.size();
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_CreateBookmarkChild", error);
      return 0;
   }

   return bookmarkID;
}
//---------------------------------------------------------------------------

unsigned int __stdcall LITEPDF_PUBLIC litePDF_CreateBookmarkSibling(void *pctx,
                                                                    unsigned int previousBookmarkID,
                                                                    const wchar_t *title,
                                                                    unsigned int flags,
                                                                    unsigned char titleColor_red,
                                                                    unsigned char titleColor_green,
                                                                    unsigned char titleColor_blue,
                                                                    unsigned int destinationPageIndex,
                                                                    unsigned int destinationX_u,
                                                                    unsigned int destinationY_u)
{
   if (!pctx) {
      return 0;
   }

   litePDFContext *ctx = (litePDFContext *) pctx;

   unsigned int bookmarkID = 0;

   if (ctx->documentSaved) {
      if (ctx->on_error) {
         ctx->on_error(ERROR_INVALID_OPERATION, "litePDF_CreateBookmarkSibling: Document was already saved", ctx->on_error_user_data);
      }
      return 0;
   }

   try {
      if (ctx->currentDraw.hdc) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_IO_PENDING, "litePDF_CreateBookmarkSibling: A draw operation is in progress", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->streamed_document && !ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkSibling: No document is opened", ctx->on_error_user_data);
         }

         return 0;
      }

      if (!ctx->mem_document) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkSibling: Can be called on memory-based documents only", ctx->on_error_user_data);
         }

         return 0;
      }

      if (destinationPageIndex >= ctx->mem_document->GetPageCount()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkSibling: Destination page index is out of range", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfPage *page = ctx->mem_document->GetPage(destinationPageIndex);
      if (!page) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_HANDLE, "litePDF_CreateBookmarkSibling: Cannot find destination page", ctx->on_error_user_data);
         }
         return 0;
      }

      PdfOutlines *outlines = ctx->mem_document->GetOutlines();
      if (!outlines) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkSibling: Cannot get table of document bookmarks", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfOutlineItem *createIn;

      if (previousBookmarkID - 1 >= (unsigned int) ctx->outlines.size()) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkSibling: Previous bookmark ID is out of range", ctx->on_error_user_data);
         }

         return 0;
      }

      createIn = ctx->outlines[previousBookmarkID - 1];
      if (!checkBookmarkExists(outlines->First(), createIn)) {
         ctx->outlines[previousBookmarkID - 1] = NULL;
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkChild: Previous bookmark doesn't exist", ctx->on_error_user_data);
         }

         return 0;
      }

      PdfString str;
      str.setFromWchar_t ((const wchar_t *) title);
      PdfDestination dest(page, page->GetPageSize().GetLeft() + LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationX_u)),
                         page->GetPageSize().GetBottom() + page->GetPageSize().GetHeight() - LITEPDF_MM_TO_POINTS(unitToMM(ctx->unit, destinationY_u)), 0);
      PdfOutlineItem *created = createIn->CreateNext(str, dest);
      if (!created) {
         if (ctx->on_error) {
            ctx->on_error(ERROR_INVALID_INDEX, "litePDF_CreateBookmarkChild: Failed to create document bookmark", ctx->on_error_user_data);
         }

         return 0;
      }

      created->SetTextFormat((EPdfOutlineFormat) (ePdfOutlineFormat_Default |
         ((flags & LITEPDF_BOOKMARK_FLAG_ITALIC) != 0 ? ePdfOutlineFormat_Italic : 0) |
         ((flags & LITEPDF_BOOKMARK_FLAG_BOLD) != 0 ? ePdfOutlineFormat_Bold : 0)));
      created->SetTextColor(titleColor_red / 255.0, titleColor_green / 255.0, titleColor_blue / 255.0);

      ctx->outlines.push_back(created);
      bookmarkID = ctx->outlines.size();
   } catch (const PdfError &error) {
      handleException(ctx, "litePDF_CreateBookmarkChild", error);
      return 0;
   }

   return bookmarkID;
}
//---------------------------------------------------------------------------
