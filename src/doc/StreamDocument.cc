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

#include "StreamDocument.h"
#include "Encrypt.h"

#include <iostream>

namespace NoPoDoFo {
using namespace PoDoFo;
using namespace Napi;
using std::string;

FunctionReference StreamDocument::constructor;

/**
 * @note Javascript args (filename:string, version:number, writeMode:number,
 * encrypt:External)
 * @brief StreamDocument::StreamDocument
 * @param info
 */
StreamDocument::StreamDocument(const CallbackInfo& info)
  : ObjectWrap(info)
  , BaseDocument()
{
  string filename = info[0].As<String>().Utf8Value();
  auto version = static_cast<EPdfVersion>(info[1].As<Number>().Int32Value());
  auto writeMode =
    static_cast<EPdfWriteMode>(info[2].As<Number>().Int32Value());
  PdfEncrypt* encrypt = nullptr;
  if (info.Length() == 3 && info[3].Type() == napi_external) {
    encrypt = info[3].As<External<PdfEncrypt>>().Data();
  }
  document =
    new PdfStreamedDocument(filename.c_str(), version, encrypt, writeMode);
  BaseDocument::SetInstance(document);
}

StreamDocument::~StreamDocument()
{
  std::cout << "Destructing StreamDocument" << std::endl;
  HandleScope scope(Env());
  delete document;
}

void
StreamDocument::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env, "StreamDocument", { InstanceMethod("close", &StreamDocument::Close) });
  constructor = Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("StreamDocument", ctor);
}

void
StreamDocument::Close(const CallbackInfo&)
{
  document->Close();
}
}
