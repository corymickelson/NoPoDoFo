/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
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
#include <podofo/podofo.h>
namespace NoPoDoFo {
class SignatureField : public Napi::ObjectWrap<SignatureField>
{
public:
  explicit SignatureField(const Napi::CallbackInfo&);
  ~SignatureField();

  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void SetAppearanceStream(const Napi::CallbackInfo&);
  void SetReason(const Napi::CallbackInfo&);
  void SetLocation(const Napi::CallbackInfo&);
  void SetCreator(const Napi::CallbackInfo&);
  void SetDate(const Napi::CallbackInfo&);
  void SetFieldName(const Napi::CallbackInfo&);
  void AddCertificateReference(const Napi::CallbackInfo&);
  Napi::Value GetSignatureObject(const Napi::CallbackInfo&);
  Napi::Value EnsureSignatureObject(const Napi::CallbackInfo&);

  PoDoFo::PdfSignatureField* GetField() { return field; }
  PoDoFo::PdfData* GetSignatureData() { return signatureBuffer; }

private:
  PoDoFo::PdfSignatureField* field;
  PoDoFo::PdfData* signatureBuffer;
  bool isExternalInstance = false;
};
}
#endif
