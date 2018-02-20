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

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Font : public Napi::ObjectWrap<Font>
{
public:
  explicit Font(const Napi::CallbackInfo& callbackInfo);
  ~Font();

  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetFontSize(const Napi::CallbackInfo&);
  void SetFontSize(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetFontScale(const Napi::CallbackInfo&);
  void SetFontScale(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetFontCharSpace(const Napi::CallbackInfo&);
  void SetFontCharSpace(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetWordSpace(const Napi::CallbackInfo&);
  void SetWordSpace(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsUnderlined(const Napi::CallbackInfo&);
  void SetUnderline(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsStrikeOut(const Napi::CallbackInfo&);
  void SetStrikeOut(const Napi::CallbackInfo&, const Napi::Value&);

  Napi::Value GetIdentifier(const Napi::CallbackInfo&);
  Napi::Value GetEncoding(const Napi::CallbackInfo&);
  Napi::Value GetFontMetric(const Napi::CallbackInfo&);
  Napi::Value IsBold(const Napi::CallbackInfo&);
  Napi::Value IsItalic(const Napi::CallbackInfo&);
  Napi::Value StringWidth(const Napi::CallbackInfo&);

  void WriteToStream(const Napi::CallbackInfo&);
  void EmbedFont(const Napi::CallbackInfo&);

  PoDoFo::PdfFont* GetPoDoFoFont() { return font; }

private:
  PoDoFo::PdfFont* font;
};
}
#endif // NPDF_FONT_H
