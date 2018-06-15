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

#include "FileSpec.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "StreamDocument.h"

using namespace Napi;
using namespace PoDoFo;

using std::make_unique;
using std::string;

namespace NoPoDoFo {

FunctionReference FileSpec::constructor; // NOLINT

void
FileSpec::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor =
    DefineClass(env,
                "FileSpec",
                { InstanceAccessor("name", &FileSpec::GetFileName, nullptr),
                  InstanceMethod("getContents", &FileSpec::Data) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("FileSpec", ctor);
}
FileSpec::FileSpec(const CallbackInfo& info)
  : ObjectWrap<FileSpec>(info)
{
  if (info.Length() == 1 && info[0].IsObject() &&
      info[0].As<Object>().InstanceOf(Obj::constructor.Value())) {
    spec =
      make_unique<PdfFileSpec>(&Obj::Unwrap(info[0].As<Object>())->GetObject());
  } else if (info.Length() == 1 && info[0].Type() == napi_external) {
    auto pObj = info[0].As<External<PdfObject>>().Data();
    spec = make_unique<PdfFileSpec>(pObj);
  } else if (info.Length() >= 2) {
    string file = info[0].As<String>().Utf8Value();
    auto docObj = info[1].As<Object>();
    PdfDocument* doc;
    if (docObj.InstanceOf(Document::constructor.Value()))
      doc = Document::Unwrap(docObj)->GetBaseDocument().get();
    else if (docObj.InstanceOf(StreamDocument::constructor.Value()))
      doc = StreamDocument::Unwrap(docObj)->GetBaseDocument().get();
    else {
      TypeError::New(
        info.Env(),
        "Unknown Document type. Requires a Document or StreamDocument")
        .ThrowAsJavaScriptException();
    }
    bool embed = true;
    if (info.Length() >= 3 && info[2].IsBuffer()) {
      embed = info[2].As<Boolean>();
    }
    spec = make_unique<PdfFileSpec>(file.c_str(), embed, doc, true);
  } else {
    TypeError::New(info.Env(),
                   "Valid constructor args: [ [Obj], [External<PdfObject>], "
                   "[string, BaseDocument] ]")
      .ThrowAsJavaScriptException();
  }
}

Napi::Value
FileSpec::GetFileName(const Napi::CallbackInfo& info)
{
  return String::New(info.Env(), spec->GetFilename(true).GetStringUtf8());
}
/**
 * @todo: update to handle unicode
 * @param info
 * @return
 */
Napi::Value
FileSpec::Data(const Napi::CallbackInfo& info)
{
  if (!spec->GetObject()->GetDictionary().HasKey(Name::EF)) {
    return info.Env().Null();
  } else {
    auto ef = spec->GetObject()->GetDictionary().GetKey(Name::EF);
    if (ef->IsReference()) {
      auto efr = ef->GetOwner()->GetObject(ef->GetReference());
      if (!efr->IsDictionary()) {
        Error::New(info.Env(),
                   "Failed to parse fileSpec for EmbeddedFile dictionary")
          .ThrowAsJavaScriptException();
        return info.Env().Undefined();
      } else {
        if (!ef->IsDictionary()) {
          Error::New(info.Env(), "EmbeddedFile expected dictionary")
            .ThrowAsJavaScriptException();
          return info.Env().Undefined();
        } else {
          auto efd = ef->GetDictionary();
          if (!efd.HasKey(Name::F)) {
            Error::New(info.Env(), "Missing File Key")
              .ThrowAsJavaScriptException();
            return info.Env().Undefined();
          } else {
            PdfObject* data;
            auto fd = efd.GetKey(Name::F);
            if (fd->IsReference()) {
              data = fd->GetOwner()->GetObject(fd->GetReference());
            } else {
              data = fd;
            }
            auto pStream = dynamic_cast<PdfMemStream*>(data->GetStream());
            const char* stream = pStream->Get();
            pdf_long length = pStream->GetLength();
            auto value = Buffer<char>::Copy(
              info.Env(), stream, static_cast<size_t>(length));
            return value;
          }
        }
      }
    }
  }
  return info.Env().Undefined();
}
}
