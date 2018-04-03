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


namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::string;
using std::vector;

Napi::FunctionReference Encrypt::constructor; // NOLINT

Encrypt::Encrypt(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 1, { napi_object });
  if(!info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
    throw TypeError();
  }
  document = Document::Unwrap(info[0].As<Object>());
}
Encrypt::~Encrypt()
{
  HandleScope scope(Env());
  cout << "Destructing encrypt" << endl;
  document = nullptr;
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
      InstanceAccessor("protections", &Encrypt::GetProtectionsValue, nullptr),
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
  if (std::find(candidates.begin(), candidates.end(), key) ==
      candidates.end()) {
    throw Napi::Error::New(info.Env(), "key must be of type ProtectionOption");
  }
  bool is = false;
  try {
    if (key == "Copy") {
      is = GetEncrypt()->IsCopyAllowed();
    } else if (key == "Print") {
      is = GetEncrypt()->IsEditAllowed();
    } else if (key == "Edit") {
      is = GetEncrypt()->IsEditAllowed();
    } else if (key == "EditNotes") {
      is = GetEncrypt()->IsEditNotesAllowed();
    } else if (key == "FillAndSign") {
      is = GetEncrypt()->IsFillAndSignAllowed();
    } else if (key == "Accessible") {
      is = GetEncrypt()->IsAccessibilityAllowed();
    } else if (key == "DocAssembly") {
      is = GetEncrypt()->IsDocAssemblyAllowed();
    } else if (key == "HighPrint") {
      is = GetEncrypt()->IsHighPrintAllowed();
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
  // TODO: Fix const cast
  auto auth = const_cast<PdfEncrypt*>(GetEncrypt())->Authenticate(pwd, id);
  return Napi::Boolean::New(info.Env(), auth);
}

Napi::Value
Encrypt::GetOwnerValue(const CallbackInfo& info)
{
  return String::New(info.Env(),
                     reinterpret_cast<const char*>(GetEncrypt()->GetOValue()));
}

Napi::Value
Encrypt::GetUserValue(const CallbackInfo& info)
{
  return String::New(info.Env(),
                     reinterpret_cast<const char*>(GetEncrypt()->GetUValue()));
}

Napi::Value
Encrypt::GetProtectionsValue(const CallbackInfo &info)
{
  auto perm = Object::New(info.Env());
  perm.Set("Accessible", GetEncrypt()->IsAccessibilityAllowed());
  perm.Set("Print", GetEncrypt()->IsPrintAllowed());
  perm.Set("Copy", GetEncrypt()->IsCopyAllowed());
  perm.Set("DocAssembly", GetEncrypt()->IsDocAssemblyAllowed());
  perm.Set("Edit", GetEncrypt()->IsEditAllowed());
  perm.Set("EditNotes", GetEncrypt()->IsEditNotesAllowed());
  perm.Set("FillAndSign", GetEncrypt()->IsFillAndSignAllowed());
  perm.Set("HighPrint", GetEncrypt()->IsHighPrintAllowed());
  return perm;
}

Napi::Value
Encrypt::GetEncryptionKey(const CallbackInfo& info)
{
  return String::New(
    info.Env(),
    reinterpret_cast<const char*>(GetEncrypt()->GetEncryptionKey()));
}

Napi::Value
Encrypt::GetKeyLength(const CallbackInfo& info)
{
  return Number::New(info.Env(), GetEncrypt()->GetKeyLength());
}
}
