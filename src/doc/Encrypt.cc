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


#include "Encrypt.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include <algorithm>

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {
Napi::FunctionReference Encrypt::constructor;

Encrypt::Encrypt(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 2, { napi_external, napi_external });
  encrypt = info[0].As<External<PdfEncrypt>>().Data();
  document = info[1].As<External<Document>>().Data();
}

void
Encrypt::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Encrypt",
    { InstanceAccessor("user", &Encrypt::GetUserValue, nullptr),
      InstanceAccessor("owner", &Encrypt::GetOwnerValue, nullptr),
      InstanceAccessor("permission", &Encrypt::GetOwnerValue, nullptr),
      InstanceAccessor("encryptionKey", &Encrypt::GetEncryptionKey, nullptr),
      InstanceAccessor("keyLength", &Encrypt::GetKeyLength, nullptr),
      InstanceMethod("isAllowed", &Encrypt::IsAllowed),
      InstanceMethod("authenticate", &Encrypt::Authenticate) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Encrypt", ctor);
}

Napi::Value
Encrypt::IsAllowed(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_string });
  vector<string> candidates = { "Copy",        "Print",       "Edit",
                                "EditNotes",   "FillAndSign", "Accessible",
                                "DocAssembly", "HighPrint" };
  string key = info[0].As<String>().Utf8Value();
  if (find(candidates.begin(), candidates.end(), key) == candidates.end()) {
    throw Napi::Error::New(info.Env(), "key must be of type ProtectionOption");
  }
  bool is = false;
  try {
    if (key == "Copy") {
      is = encrypt->IsCopyAllowed();
    } else if (key == "Print") {
      is = encrypt->IsEditAllowed();
    } else if (key == "Edit") {
      is = encrypt->IsEditAllowed();
    } else if (key == "EditNotes") {
      is = encrypt->IsEditNotesAllowed();
    } else if (key == "FillAndSign") {
      is = encrypt->IsFillAndSignAllowed();
    } else if (key == "Accessible") {
      is = encrypt->IsAccessibilityAllowed();
    } else if (key == "DocAssembly") {
      is = encrypt->IsDocAssemblyAllowed();
    } else if (key == "HighPrint") {
      is = encrypt->IsHighPrintAllowed();
    } else {
      throw Napi::Error::New(info.Env(), "Key unknown");
    }

  } catch (PdfError& err) {
    ErrorHandler(err, info);
  } catch (Error& err) {
    ErrorHandler(err, info);
  }
  return Napi::Boolean::New(info.Env(), is);
}

Napi::Value
Encrypt::Authenticate(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 1, { napi_valuetype::napi_object });
  auto pwdObj = info[0].As<Object>();
  string pwd = nullptr;
  if (pwdObj.Has("userPassword") && pwdObj.Get("userPassword").IsString()) {
    pwd = pwdObj.Get("userPassword").As<String>().Utf8Value();
  } else if (pwdObj.Has("ownerPassword") &&
             pwdObj.Get("ownerPassword").IsString()) {
    pwd = pwdObj.Get("ownerPassword").As<String>().Utf8Value();
  }
  if (!pwd.c_str()) {
    throw Napi::Error::New(
      info.Env(), "must contain property userPassword OR ownerPassword");
  }
  if (!document->GetDocument()->GetTrailer()->GetDictionary().HasKey(
        PdfName("ID"))) {
    throw Napi::Error::New(info.Env(), "No document ID found in trailer");
  }
  string id = document->GetDocument()
                ->GetTrailer()
                ->GetDictionary()
                .GetKey(PdfName("ID"))
                ->GetArray()[0]
                .GetString()
                .GetStringUtf8();
  return Napi::Boolean::New(info.Env(), encrypt->Authenticate(pwd, id));
}

Napi::Value
Encrypt::GetOwnerValue(const CallbackInfo& info)
{
  return String::New(info.Env(),
                     reinterpret_cast<const char*>(encrypt->GetOValue()));
}

Napi::Value
Encrypt::GetUserValue(const CallbackInfo& info)
{
  return String::New(info.Env(),
                     reinterpret_cast<const char*>(encrypt->GetUValue()));
}

Napi::Value
Encrypt::GetPermissionValue(const CallbackInfo& info)
{
  return Number::New(info.Env(), encrypt->GetPValue());
}

Napi::Value
Encrypt::GetEncryptionKey(const CallbackInfo& info)
{
  return String::New(
    info.Env(), reinterpret_cast<const char*>(encrypt->GetEncryptionKey()));
}

Napi::Value
Encrypt::GetKeyLength(const CallbackInfo& info)
{
  return Number::New(info.Env(), encrypt->GetKeyLength());
}
Encrypt::~Encrypt()
{
  if (encrypt != nullptr) {
    HandleScope scope(Env());
    delete encrypt;
    document = nullptr;
  }
}
}
