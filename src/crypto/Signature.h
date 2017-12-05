//
// Created by red on 4/12/17.
//

#ifndef NPDF_SIGNATURE_H
#define NPDF_SIGNATURE_H

#include <napi.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <podofo/podofo.h>
#include <openssl/pkcs7.h>
#include <openssl/err.h>

Napi::Value NPDFSignatureData(const Napi::CallbackInfo &);

#endif //NPDF_SIGNATURE_H
