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

using namespace PoDoFo;
using namespace Napi;

using std::shared_ptr;
using std::string;

namespace NoPoDoFo {

FunctionReference StreamDocument::constructor; // NOLINT

/**
 * @note Javascript args (filename:string, create:boolean, optsion: {
 * version:number, writeMode:number, encrypt:External})
 * @brief StreamDocument::StreamDocument
 * @param info
 */
StreamDocument::StreamDocument(const CallbackInfo& info)
  : ObjectWrap(info)
  , BaseDocument(info)
{
  document = std::static_pointer_cast<PdfStreamedDocument>(
    BaseDocument::GetBaseDocument());
}

StreamDocument::~StreamDocument()
{
  std::cout << "Destructing StreamDocument" << std::endl;
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
