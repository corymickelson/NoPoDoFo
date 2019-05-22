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
#ifndef NPDF_CONTENTSTOKENIZER_H
#define NPDF_CONTENTSTOKENIZER_H

#include "../doc/Document.h"
#include "spdlog/logger.h"
#include <napi.h>
#include <podofo/podofo.h>

using Napi::CallbackInfo;
using JsValue = Napi::Value;
using std::vector;

namespace NoPoDoFo {
class ContentsTokenizer final : public ObjectWrap<ContentsTokenizer>
{
public:
  explicit ContentsTokenizer(const Napi::CallbackInfo&);
  explicit ContentsTokenizer(const ContentsTokenizer&) = delete;
  const ContentsTokenizer& operator=(const ContentsTokenizer&) = delete;
  ~ContentsTokenizer();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue ReadSync(const Napi::CallbackInfo&);
  void Read(const CallbackInfo&);
  void ReadIntoData();
  vector<string> Data;
  string ContentsString;

private:
  std::unique_ptr<PoDoFo::PdfContentsTokenizer> Self;
  Document& Doc;
  std::shared_ptr<spdlog::logger> DbgLog;

  int PageIndex;
  void AddText(PoDoFo::PdfFont*, const PoDoFo::PdfString&);
};
}

#endif
