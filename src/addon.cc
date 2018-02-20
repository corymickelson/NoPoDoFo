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
#include "base/ContentsTokenizer.h"
#include "base/Data.h"
#include "base/Dictionary.h"
#include "base/Obj.h"
#include "base/Ref.h"
#include "base/Stream.h"
#include "base/Vector.h"
#include "crypto/Signature.h"
#include "doc/Annotation.h"
#include "doc/CheckBox.h"
#include "doc/ComboBox.h"
#include "doc/Encoding.h"
#include "doc/Encrypt.h"
#include "doc/ExtGState.h"
#include "doc/FileSpec.h"
#include "doc/Font.h"
#include "doc/Form.h"
#include "doc/Image.h"
#include "doc/ListField.h"
#include "doc/Painter.h"
#include "doc/SignatureField.h"
#include "doc/Signer.h"
#include "doc/TextField.h"
#include "doc/SimpleTable.h"
#include "doc/ListBox.h"
#include <napi.h>

Napi::Object
init(Napi::Env env, Napi::Object exports)
{
  NoPoDoFo::Document::Initialize(env, exports);
  NoPoDoFo::Page::Initialize(env, exports);
  NoPoDoFo::Field::Initialize(env, exports);
  NoPoDoFo::TextField::Initialize(env, exports);
  NoPoDoFo::Image::Initialize(env, exports);
  NoPoDoFo::Annotation::Initialize(env, exports);
  NoPoDoFo::Rect::Initialize(env, exports);
  NoPoDoFo::Painter::Initialize(env, exports);
  NoPoDoFo::CheckBox::Initialize(env, exports);
  NoPoDoFo::ComboBox::Initialize(env, exports);
  NoPoDoFo::ListBox::Initialize(env, exports);
  NoPoDoFo::Form::Initialize(env, exports);
  NoPoDoFo::Dictionary::Initialize(env, exports);
  NoPoDoFo::FileSpec::Initialize(env, exports);
  NoPoDoFo::Obj::Initialize(env, exports);
  NoPoDoFo::Ref::Initialize(env, exports);
  NoPoDoFo::Array::Initialize(env, exports);
  NoPoDoFo::Stream::Initialize(env, exports);
  NoPoDoFo::Encrypt::Initialize(env, exports);
  NoPoDoFo::ListField::Initialize(env, exports);
  NoPoDoFo::Font::Initialize(env, exports);
  NoPoDoFo::Encoding::Initialize(env, exports);
  NoPoDoFo::ExtGState::Initialize(env, exports);
  NoPoDoFo::Signer::Initialize(env, exports);
  NoPoDoFo::SignatureField::Initialize(env, exports);
  NoPoDoFo::Vector::Initialize(env, exports);
  NoPoDoFo::Data::Initialize(env, exports);
  NoPoDoFo::ContentsTokenizer::Initialize(env, exports);
  NoPoDoFo::SimpleTable::Initialize(env, exports);

  exports["signature"] = Function::New(env, NPDFSignatureData);

  return exports;
}

NODE_API_MODULE(npdf, init);
