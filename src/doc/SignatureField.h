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

#ifndef NPDF_SIGNATUREFIELD_H
#define NPDF_SIGNATUREFIELD_H

#include <napi.h>
#include <openssl/evp.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using std::string;
using std::vector;
using JsValue = Napi::Value;

namespace NoPoDoFo {

struct SignatureInfo
{
  vector<uint32_t> Range = {};
  string Contents;
};

class SignatureField : public Napi::ObjectWrap<SignatureField>
{
public:
  explicit SignatureField(const Napi::CallbackInfo&);
  explicit SignatureField(const SignatureField&) = delete;
  const SignatureField& operator=(const SignatureField&) = delete;
  ~SignatureField();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetAppearanceStream(const Napi::CallbackInfo&);
  void SetReason(const Napi::CallbackInfo&);
  void SetLocation(const Napi::CallbackInfo&);
  void SetCreator(const Napi::CallbackInfo&);
  void SetDate(const Napi::CallbackInfo&);
  void SetFieldName(const Napi::CallbackInfo&);
  void AddCertificateReference(const Napi::CallbackInfo&);
  JsValue GetAnnotation(const Napi::CallbackInfo&);
  JsValue GetFieldObject(const Napi::CallbackInfo&);
  JsValue GetSignatureObject(const Napi::CallbackInfo&);
  JsValue EnsureSignatureObject(const Napi::CallbackInfo&);
  JsValue GetInfo(const Napi::CallbackInfo&);
  PoDoFo::PdfData* GetSignatureData() const { return SigningContent.get(); }
  std::shared_ptr<PoDoFo::PdfSignatureField> GetField() const { return Self; }

private:

  std::shared_ptr<PoDoFo::PdfSignatureField> Self;
  std::unique_ptr<PoDoFo::PdfData> SigningContent;
  SignatureInfo Info;
  std::shared_ptr<spdlog::logger> Log;
};
}
#endif
