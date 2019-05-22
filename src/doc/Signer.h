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

#ifndef NPDF_SIGNER_H
#define NPDF_SIGNER_H

#include <future>
#include <napi.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <podofo/podofo.h>
#include <spdlog/spdlog.h>

using JsValue = Napi::Value;
namespace NoPoDoFo {

class Signer : public Napi::ObjectWrap<Signer>
{
public:
  static Napi::FunctionReference Constructor;
  explicit Signer(const Napi::CallbackInfo&);
  explicit Signer(const Signer&) = delete;
  const Signer& operator=(const Signer&) = delete;
  ~Signer();
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetField(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetField(const Napi::CallbackInfo&);
  JsValue SignWorker(const Napi::CallbackInfo&);
  JsValue LoadCertificateAndKey(const Napi::CallbackInfo&);

  PoDoFo::PdfMemDocument& Doc;
  std::string Output;
  std::shared_ptr<PoDoFo::PdfSignatureField> Field;

  EVP_PKEY* Pkey = nullptr;
  X509* Cert = nullptr;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif
