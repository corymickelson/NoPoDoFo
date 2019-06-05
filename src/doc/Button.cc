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

#include "Button.h"
#include "../base/Names.h"
#include "Field.h"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;

namespace NoPoDoFo {
Button::Button(PdfField& field)
  : Field(field)
{
  Btn = new PdfButton(field);
  DbgLog = spdlog::get("DbgLog");
}

Button::~Button()
{
  if (DbgLog != nullptr)
    DbgLog->debug("Button Cleanup");
  delete Btn;
}
JsValue
Button::GetCaption(const Napi::CallbackInfo& info)
{
  return String::New(info.Env(), Btn->GetCaption().GetStringUtf8());
}
void
Button::SetCaption(const Napi::CallbackInfo&, const JsValue& value)
{
  Btn->SetCaption(PdfString(value.As<String>().Utf8Value()));
}
void
Button::WriteAppearanceStream(const PoDoFo::PdfXObject& canvas)
{
  std::stringstream ss;
  PdfLocaleImbue(ss);
  PdfRefCountedBuffer buffer;
  std::map<string, PdfObject*> apKeys = Field::GetFieldRefreshKeys(&Field);
  auto ap = apKeys.find(Name::AP)->second;
  if (apKeys.find(Name::AS)->second == nullptr) {
    // handle null
  }
  string state = apKeys.find(Name::AS)->second->GetString().GetStringUtf8();
  if (state == "Off") {
    if (ap->GetDictionary().HasKey(Name::N) &&
        ap->MustGetIndirectKey(Name::N)->GetDictionary().HasKey("Off")) {
    }
    if (ap->GetDictionary().HasKey(Name::D) &&
        ap->MustGetIndirectKey(Name::D)->GetDictionary().HasKey("Off")) {
    }
  } else {
    if (ap->GetDictionary().HasKey(Name::N) &&
        ap->MustGetIndirectKey(Name::N)->GetDictionary().HasKey(state)) {
      PdfXObject xObj(
        apKeys.find(Name::AP)->second->MustGetIndirectKey(Name::N));
      xObj.GetContentsForAppending()->GetStream()->BeginAppend();
      PdfOutputDevice device(&buffer);
      ap->MustGetIndirectKey(Name::N)
        ->GetDictionary()
        .GetKey(state)
        ->GetString()
        .Write(&device, ePdfWriteMode_Compact);

      ss << buffer.GetBuffer() << endl;
      xObj.GetContentsForAppending()->GetStream()->Append(ss.str());
      xObj.GetContentsForAppending()->GetStream()->EndAppend();

      PdfRect r(0,
                0,
                Field.GetWidgetAnnotation()->GetRect().GetWidth(),
                Field.GetWidgetAnnotation()->GetRect().GetHeight());
      xObj.GetObject()->GetDictionary().RemoveKey(Name::BBOX);
      PdfVariant ra;
      r.ToVariant(ra);
      xObj.GetObject()->GetDictionary().AddKey(Name::BBOX, ra.GetArray());
    }
  }
}
PoDoFo::PdfObject*
Button::GetStateAppearanceStream()
{
  std::map<string, PdfObject*> apKeys = Field::GetFieldRefreshKeys(&Field);
  auto ap = apKeys.find(Name::AP)->second;
  if (apKeys.find(Name::AS)->second == nullptr) {
    // handle null
  }
  string state = apKeys.find(Name::AS)->second->GetString().GetStringUtf8();
  if (ap->GetDictionary().HasKey(Name::N) &&
      ap->MustGetIndirectKey(Name::N)->GetDictionary().HasKey(state)) {
    return ap->MustGetIndirectKey(Name::N)->MustGetIndirectKey(state);
  }
  if (ap->GetDictionary().HasKey(Name::D) &&
      ap->MustGetIndirectKey(Name::D)->GetDictionary().HasKey(state)) {
    return ap->MustGetIndirectKey(Name::D)->MustGetIndirectKey(state);
  }
  if (ap->GetDictionary().HasKey(Name::R) &&
      ap->MustGetIndirectKey(Name::R)->GetDictionary().HasKey(state)) {
    return ap->MustGetIndirectKey(Name::R)->MustGetIndirectKey(state);
  }
  return nullptr;
}

}
