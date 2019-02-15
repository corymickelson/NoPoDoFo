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

#include "base/Array.h"
#include "base/Color.h"
#include "base/ContentsTokenizer.h"
#include "base/Data.h"
#include "base/Date.h"
#include "base/Dictionary.h"
#include "base/Obj.h"
#include "base/Ref.h"
#include "base/Stream.h"
#include "base/XObject.h"
#include "doc/Action.h"
#include "doc/Annotation.h"
#include "doc/CheckBox.h"
#include "doc/ComboBox.h"
#include "doc/Destination.h"
#include "doc/Encoding.h"
#include "doc/Encrypt.h"
#include "doc/ExtGState.h"
#include "doc/FileSpec.h"
#include "doc/Font.h"
#include "doc/Form.h"
#include "doc/Image.h"
#include "doc/ListBox.h"
#include "doc/ListField.h"
#include "doc/Outline.h"
#include "doc/Page.h"
#include "doc/Painter.h"
#include "doc/PushButton.h"
#include "doc/Rect.h"
#include "doc/SignatureField.h"
#include "doc/Signer.h"
#include "doc/SimpleTable.h"
#include "doc/StreamDocument.h"
#include "doc/TextField.h"
#include <napi.h>

Napi::Object
init(Napi::Env env, Napi::Object exports)
{
#if PODOFO_VERSION_MINOR < 9 && PODOFO_VERSION_PATCH < 6
  Napi::Error::New(env, "PoDoFo Version must be >= 0.9.6")
    .ThrowAsJavaScriptException();
  return;
#endif
#ifdef NOPODOFO_DEBUG
  cout << "/******************************************************/" << endl;
  cout << "/***** You are running a Debug build of NoPoDoFo ******/" << endl;
  cout << "/******************************************************/" << endl;
#endif
  NoPoDoFo::Action::Initialize(env, exports);
  NoPoDoFo::Date::Initialize(env, exports);
  NoPoDoFo::Annotation::Initialize(env, exports);
  NoPoDoFo::Array::Initialize(env, exports);
  NoPoDoFo::CheckBox::Initialize(env, exports);
  NoPoDoFo::Color::Initialize(env, exports);
  NoPoDoFo::ComboBox::Initialize(env, exports);
  NoPoDoFo::ContentsTokenizer::Initialize(env, exports);
  NoPoDoFo::Data::Initialize(env, exports);
  NoPoDoFo::Destination::Initialize(env, exports);
  NoPoDoFo::Dictionary::Initialize(env, exports);
  NoPoDoFo::Document::Initialize(env, exports);
  NoPoDoFo::ExtGState::Initialize(env, exports);
  NoPoDoFo::Encoding::Initialize(env, exports);
  NoPoDoFo::Encrypt::Initialize(env, exports);
  NoPoDoFo::FileSpec::Initialize(env, exports);
  NoPoDoFo::Font::Initialize(env, exports);
  NoPoDoFo::Form::Initialize(env, exports);
  NoPoDoFo::Image::Initialize(env, exports);
  NoPoDoFo::ListBox::Initialize(env, exports);
  NoPoDoFo::Obj::Initialize(env, exports);
  NoPoDoFo::XObject::Initialize(env, exports);
  NoPoDoFo::Outline::Initialize(env, exports);
  NoPoDoFo::Page::Initialize(env, exports);
  NoPoDoFo::Painter::Initialize(env, exports);
  NoPoDoFo::PushButton::Initialize(env, exports);
  NoPoDoFo::Rect::Initialize(env, exports);
  NoPoDoFo::Stream::Initialize(env, exports);
  NoPoDoFo::Signer::Initialize(env, exports);
  NoPoDoFo::SignatureField::Initialize(env, exports);
  NoPoDoFo::SimpleTable::Initialize(env, exports);
  NoPoDoFo::StreamDocument::Initialize(env, exports);
  NoPoDoFo::TextField::Initialize(env, exports);
  NoPoDoFo::Ref::Initialize(env, exports);

  return exports;
}

NODE_API_MODULE(npdf, init);
