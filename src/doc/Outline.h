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

#ifndef NPDF_OUTLINE_H
#define NPDF_OUTLINE_H

#include <napi.h>
#include <podofo/podofo.h>

using std::vector;

namespace NoPoDoFo {

/**
 * @note PdfOutlineItem Wrapper, PdfOutline::CreateRoot exposed through
 * BaseDocument.
 */
class Outline : public Napi::ObjectWrap<Outline>
{
public:
  explicit Outline(const Napi::CallbackInfo& info);
  ~Outline();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
//  void CreateRoot(const Napi::CallbackInfo&);
  Napi::Value CreateChild(const Napi::CallbackInfo&);
  Napi::Value CreateNext(const Napi::CallbackInfo&);
  Napi::Value InsertChild(const Napi::CallbackInfo&);
  Napi::Value Prev(const Napi::CallbackInfo&);
  Napi::Value Next(const Napi::CallbackInfo&);
  Napi::Value First(const Napi::CallbackInfo&);
  Napi::Value Last(const Napi::CallbackInfo&);
  Napi::Value GetParent(const Napi::CallbackInfo&);
  Napi::Value Erase(const Napi::CallbackInfo&);
  Napi::Value GetDestination(const Napi::CallbackInfo&);
  void SetDestination(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetAction(const Napi::CallbackInfo&);
  void SetAction(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetTitle(const Napi::CallbackInfo&);
  void SetTitle(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetTextFormat(const Napi::CallbackInfo&);
  void SetTextFormat(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetTextColor(const Napi::CallbackInfo&);
  void SetTextColor(const Napi::CallbackInfo&, const Napi::Value&);
  PoDoFo::PdfOutlineItem& GetOutline() { return outline; }
private:
  PoDoFo::PdfOutlineItem& outline; // owned by the document
};
}
#endif // NPDF_OUTLINE_H
