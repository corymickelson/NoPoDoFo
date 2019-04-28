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

#ifndef NPDF_FONT_H
#define NPDF_FONT_H

#include <iostream>
#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using std::cout;
using std::endl;
using JsValue = Napi::Value;

namespace NoPoDoFo {
class Font : public Napi::ObjectWrap<Font>
{
public:
  explicit Font(const Napi::CallbackInfo& callbackInfo);
	explicit Font(const Font&) = delete;
	const Font&operator=(const Font&) = delete;
  ~Font();
  static Napi::FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue GetFontSize(const Napi::CallbackInfo&);
  void SetFontSize(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetFontScale(const Napi::CallbackInfo&);
  void SetFontScale(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetFontCharSpace(const Napi::CallbackInfo&);
  void SetFontCharSpace(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetWordSpace(const Napi::CallbackInfo&);
  void SetWordSpace(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsUnderlined(const Napi::CallbackInfo&);
  void SetUnderline(const Napi::CallbackInfo&, const JsValue&);
  JsValue IsStrikeOut(const Napi::CallbackInfo&);
  void SetStrikeOut(const Napi::CallbackInfo&, const JsValue&);
  JsValue GetIdentifier(const Napi::CallbackInfo&);
  JsValue GetEncoding(const Napi::CallbackInfo&);
  JsValue GetFontMetric(const Napi::CallbackInfo&);
  JsValue IsBold(const Napi::CallbackInfo&);
  JsValue IsItalic(const Napi::CallbackInfo&);
  JsValue StringWidth(const Napi::CallbackInfo&);
  JsValue GetObject(const Napi::CallbackInfo&);
  void WriteToStream(const Napi::CallbackInfo&);
  void EmbedFont(const Napi::CallbackInfo&);
  JsValue IsSubsetting(const Napi::CallbackInfo&);
  void EmbedSubsetFont(const Napi::CallbackInfo&);

  PoDoFo::PdfFont& GetFont() { return Self; }

private:
  PoDoFo::PdfFont& Self; // owned by the document
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_FONT_H
