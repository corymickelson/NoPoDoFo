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

namespace NoPoDoFo {

using namespace Napi;
using namespace PoDoFo;

using std::string;

FunctionReference FileSpec::constructor; // NOLINT

void
FileSpec::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(env, "FileSpec", {});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();

  target.Set("FileSpec", ctor);
}
FileSpec::FileSpec(const CallbackInfo& info)
  : ObjectWrap<FileSpec>(info)
{
  if (info.Length() == 1 && info[0].Type() == napi_external) {
    spec = info[0].As<External<PdfFileSpec>>().Data();
  } else if (info.Length() == 2) {
    string file = info[0].As<String>().Utf8Value();
    auto docObj = info[1].As<Object>();
    Document* doc = Document::Unwrap(docObj);
    spec =
      new PdfFileSpec(file.c_str(), true, doc->GetMemDocument().get(), true);
  }
}
FileSpec::~FileSpec()
{
  if (spec != nullptr) {
    HandleScope scope(Env());
    delete spec;
  }
}
}
