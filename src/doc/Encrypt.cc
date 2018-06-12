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
#include "Document.h"
#include <algorithm>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace NoPoDoFo {

Napi::FunctionReference Encrypt::constructor; // NOLINT

/**
 * @note PdfEncrypt is owned by the PdfDocument
 * @param info
 */
Encrypt::Encrypt(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
#if defined(PODOFO_HAVE_OPENSSL) || defined(PODOFO_HAVE_OPENSSL_1_1)
  if (info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
    encrypt =
      Document::Unwrap(info[0].As<Object>())->GetDocument().GetEncrypt();
  } else if (info[0].IsExternal()) {
    encrypt = info[0].As<External<PdfEncrypt>>().Data();
  } else {
    TypeError::New(info.Env(), "Invalid constructor args")
      .ThrowAsJavaScriptException();
    return;
  }
#else
  Error::New(info.Env(), "This build does not include OpenSSL")
    .ThrowAsJavaScriptException();
#endif
}

void
Encrypt::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "Encrypt",
    { StaticMethod("createEncrypt", &Encrypt::CreateEncrypt),
      InstanceAccessor("user", &Encrypt::GetUserValue, nullptr),
      InstanceAccessor("owner", &Encrypt::GetOwnerValue, nullptr),
      InstanceAccessor("protections", &Encrypt::GetProtectionsValue, nullptr),
      InstanceAccessor("encryptionKey", &Encrypt::GetEncryptionKey, nullptr),
      InstanceAccessor("keyLength", &Encrypt::GetKeyLength, nullptr),
      InstanceMethod("isAllowed", &Encrypt::IsAllowed) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("Encrypt", ctor);
}

Napi::Value
Encrypt::CreateEncrypt(const CallbackInfo& info)
{
  auto value = info[0].As<Object>();
  try {
    if (!value.IsObject()) {
      throw Error::New(info.Env(),
                       "Set encrypt requires a single argument of"
                       " type Object<{userPassword:string,"
                       " ownerPassword:string, protection:Array<string>,"
                       " algorithm: string, keyLength: int");
    }
    auto encryption = value.As<Object>();
    string ownerPwd;
    string userPwd;
    int nperm = 0;
    int algoParameter = 0;
    int key = 0;
    if (!encryption.Has("ownerPassword") || !encryption.Has("keyLength") ||
        !encryption.Has("protection") || !encryption.Has("algorithm")) {
      throw Error::New(info.Env(), "something is not right");
    }
    try {
      if (encryption.Has("ownerPassword")) {
        ownerPwd = encryption.Get("ownerPassword").As<String>().Utf8Value();
      }
      if (encryption.Has("userPassword")) {
        userPwd = encryption.Get("userPassword").As<String>().Utf8Value();
      }
      if (encryption.Has("protection")) {
        if (encryption.Get("protection").IsArray()) {
          auto permissions = encryption.Get("protection").As<Array>();
          if (!permissions.IsEmpty()) {
            for (uint32_t i = 0; i < permissions.Length(); ++i) {
              if (permissions.Get(i).IsString()) {
                string permission = permissions.Get(i).As<String>().Utf8Value();
                if (permission == "Copy")
                  nperm |= 0x00000010;
                else if (permission == "Print")
                  nperm |= 0x00000004;
                else if (permission == "Edit")
                  nperm |= 0x00000008;
                else if (permission == "EditNotes")
                  nperm |= 0x00000020;
                else if (permission == "FillAndSign")
                  nperm |= 0x00000100;
                else if (permission == "Accessible")
                  nperm |= 0x00000200;
                else if (permission == "DocAssembly")
                  nperm |= 0x00000400;
                else if (permission == "HighPrint")
                  nperm |= 0x00000800;
                else {
                  stringstream msg;
                  msg << "Unknown permission parameter: " << permission
                      << ". Permission must be one or more of: "
                      << "[Copy, Print, Edit, EditNotes, FillAndSign, "
                         "Accessible, DocAssembly, HighPrint]"
                      << endl;
                  throw Error::New(info.Env(), msg.str());
                }
              }
            }
          }
        } else {
          throw Error::New(info.Env(), "shit");
        }
      }
      if (encryption.Has("algorithm")) {
        // rc4v1 =1 rc4v2 = 2 aesv2 = 4 aesv3 = 8
        Napi::Value algoProp = encryption.Get("algorithm");
        string algo;
        if (algoProp.IsString()) {
          algo = algoProp.As<String>().Utf8Value();
          if (algo == "rc4v1")
            algoParameter = 1;
          else if (algo == "rc4v2")
            algoParameter = 2;
#ifdef PODOFO_HAVE_LIBIDN
          else if (algo == "aesv2")
            algoParameter = 4;
          else if (algo == "aesv3")
            algoParameter = 8;
#else
          else if (algo == "aesv2")
            Error::New(info.Env(), "AES support not available in this build")
              .ThrowAsJavaScriptException();
          else if (algo == "aesv3")
            Error::New(info.Env(), "AES support not available in this build")
              .ThrowAsJavaScriptException();
#endif
          else {
            stringstream msg;
            msg << "Unknown permission parameter: " << algo
                << ". Permission must be one or more of: [rc4v1, rc4v2, aesv2, "
                   "aesv3]"
                << endl;
            throw Error::New(info.Env(), msg.str());
          }
        }
      }
      if (encryption.Has("keyLength")) {
        // 40 56 80 96 128 256
        int keyValues[6] = { 40, 56, 80, 96, 128, 256 };
        Napi::Value keyProp = encryption.Get("keyLength");
        if (keyProp.IsNumber()) {
          key = keyProp.As<Number>();
          for (int i = 0; i < 6; ++i) {
            if (keyValues[i] == key)
              break;
            if (keyValues[i] != key && i == 6) {
              stringstream msg;
              msg
                << "Unknown permission parameter: " << key
                << ". Permission must be one or more of: [40, 56, 80, 96, 128, "
                   "256]"
                << endl;
              throw Error::New(info.Env(), msg.str());
            }
          }
        }
      }
    } catch (PdfError& err) {
      stringstream msg;
      msg << "Parse Encrypt Object failed with error: " << err.GetError()
          << endl;
      throw Error::New(info.Env(), msg.str());
    }
    PdfEncrypt* encrypt = PdfEncrypt::CreatePdfEncrypt(
      userPwd,
      ownerPwd,
      nperm,
      static_cast<PdfEncrypt::EPdfEncryptAlgorithm>(algoParameter),
      static_cast<PdfEncrypt::EPdfKeyLength>(key));
    return External<PdfEncrypt>::New(
      info.Env(), encrypt, [](Napi::Env env, PdfEncrypt* data) {
        cout << "Finalizer CreateEncrypt" << endl;
      });
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PdfMemDocument::SetEncrypt failed with error: " << err.GetError()
        << endl;
    throw Error::New(info.Env(), msg.str());
  }
}

Napi::Value
Encrypt::IsAllowed(const CallbackInfo& info)
{
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
Encrypt::GetProtectionsValue(const CallbackInfo& info)
{
  auto perm = Object::New(info.Env());
  perm.Set("Accessible", encrypt->IsAccessibilityAllowed());
  perm.Set("Print", encrypt->IsPrintAllowed());
  perm.Set("Copy", encrypt->IsCopyAllowed());
  perm.Set("DocAssembly", encrypt->IsDocAssemblyAllowed());
  perm.Set("Edit", encrypt->IsEditAllowed());
  perm.Set("EditNotes", encrypt->IsEditNotesAllowed());
  perm.Set("FillAndSign", encrypt->IsFillAndSignAllowed());
  perm.Set("HighPrint", encrypt->IsHighPrintAllowed());
  return perm;
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
}
