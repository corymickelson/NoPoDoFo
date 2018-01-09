//
// Created by red on 4/12/17.
//

#ifndef NPDF_SIGNATURE_H
#define NPDF_SIGNATURE_H

#include <gsl/gsl>
#include <napi.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <podofo/podofo.h>

Napi::Value
NPDFSignatureData(const Napi::CallbackInfo&);

#endif // NPDF_SIGNATURE_H
