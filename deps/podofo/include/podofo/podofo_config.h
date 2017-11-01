/*
 * *** THIS HEADER IS INCLUDED BY PdfCompilerCompat.h ***
 * *** DO NOT INCLUDE DIRECTLY ***
 */
#ifndef _PDF_COMPILERCOMPAT_H
#error Please include PdfDefines.h instead
#endif

#define PODOFO_VERSION_MAJOR 0
#define PODOFO_VERSION_MINOR 9
#define PODOFO_VERSION_PATCH 5

/* PoDoFo configuration options */
/* #undef PODOFO_MULTI_THREAD */

/* somewhat platform-specific headers */
/* #undef PODOFO_HAVE_STRINGS_H */
/* #undef PODOFO_HAVE_ARPA_INET_H */
#define PODOFO_HAVE_WINSOCK2_H 1
/* #undef PODOFO_HAVE_MEM_H */
/* #undef PODOFO_HAVE_CTYPE_H */

/* Integer types - headers */
/* #undef PODOFO_HAVE_STDINT_H */
#define PODOFO_HAVE_BASETSD_H 1 
#define PODOFO_HAVE_SYS_TYPES_H 1
/* Integer types - type names */
#define PDF_INT8_TYPENAME   signed char
#define PDF_INT16_TYPENAME  short
#define PDF_INT32_TYPENAME  int
#define PDF_INT64_TYPENAME  __int64
#define PDF_UINT8_TYPENAME  unsigned char
#define PDF_UINT16_TYPENAME unsigned short
#define PDF_UINT32_TYPENAME unsigned int
#define PDF_UINT64_TYPENAME unsigned __int64

/* Endianness */
/* #undef TEST_BIG */

/* Features */
/* #undef PODOFO_NO_FONTMANAGER */

/* Libraries */
#define PODOFO_HAVE_JPEG_LIB
#define PODOFO_HAVE_PNG_LIB
#define PODOFO_HAVE_TIFF_LIB
/* #undef PODOFO_HAVE_FONTCONFIG */
/* #undef PODOFO_HAVE_LUA */
/* #undef PODOFO_HAVE_BOOST */
/* #undef PODOFO_HAVE_CPPUNIT */
#define PODOFO_HAVE_OPENSSL
/* #undef PODOFO_HAVE_OPENSSL_1_1 */
/* #undef PODOFO_HAVE_OPENSSL_NO_RC4 */

#ifndef PODOFO_HAVE_LIBIDN
#define PODOFO_HAVE_LIBIDN
#endif
