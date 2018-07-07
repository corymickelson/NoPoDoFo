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

#include "ContentsTokenizer.h"
#include "../ValidateArguments.h"
#include "../doc/Document.h"
#include "../doc/Page.h"
#include "../doc/StreamDocument.h"
#include <iostream>
#include <stack>

using namespace Napi;
using namespace PoDoFo;
using std::make_unique;
using std::unique_ptr;

namespace NoPoDoFo {

FunctionReference ContentsTokenizer::constructor; // NOLINT

/**
 * @note JS new ContentsTokenizer(doc: IBase, pageIndex: number)
 * @param info
 */
ContentsTokenizer::ContentsTokenizer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , pIndex(info[1].As<Number>().Int32Value())
  , doc(*Document::Unwrap(info[0].As<Object>()))
  , self(make_unique<PdfContentsTokenizer>(doc.GetDocument().GetPage(pIndex)))
{}

void
ContentsTokenizer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor =
    DefineClass(env,
                "ContentsTokenizer",
                { InstanceMethod("reader", &ContentsTokenizer::ReadAll) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("ContentsTokenizer", ctor);
}

Napi::Value
ContentsTokenizer::ReadAll(const CallbackInfo& info)
{
  const char* token = nullptr;
  PdfVariant var;
  EPdfContentsType type;
  std::stack<PdfVariant> stack;
  bool blockText = false;
  PdfFont* font = nullptr;
  Napi::Array out = Array::New(info.Env());

  while (self->ReadNext(type, token, var)) {
    if (type == ePdfContentsType_Keyword) {
      if (strcmp(token, "l") == 0 || strcmp(token, "m") == 0) {
        if (stack.size() == 2) {
          stack.pop();
        } else {
          stringstream msg;
        }
      } else if (strcmp(token, "BT") == 0) {
        blockText = true;
      } else if (strcmp(token, "ET") == 0) {
        if (!blockText) {
        }
      } else if (blockText) {
        if (strcmp(token, "Tf") == 0) {
          if (stack.size() < 2) {
            font = nullptr;
            continue;
          }
          stack.pop();
          PdfName fontName = stack.top().GetName();
          PdfObject* pFont =
            doc.GetDocument().GetPage(pIndex)->GetFromResources(
              PdfName("Font"), fontName);
          if (!pFont) {
            throw Error::New(info.Env(), "Failed to create font");
          }
          font = doc.GetDocument().GetFont(pFont);
          if (!font) {
            throw Error::New(info.Env(), "Failed to create font");
          }
        } else if (strcmp(token, "Tj") == 0 || strcmp(token, "'") == 0) {
          if (stack.empty())
            continue;
          AddText(font, stack.top().GetString(), out);
          stack.pop();
        } else if (strcmp(token, "\"") == 0) {
          if (stack.size() < 3) {
            while (!stack.empty()) {
              stack.pop();
            }
            continue;
          }
          AddText(font, stack.top().GetString(), out);
          stack.pop();
          stack.pop(); // remove char spacing
          stack.pop(); // remove word spacing
        } else if (strcmp(token, "TJ") == 0) {
          if (stack.empty())
            continue;
          PdfArray array = stack.top().GetArray();
          stack.pop();
          for (auto& i : array) {
            if (i.IsString() || i.IsHexString()) {
              AddText(font, i.GetString(), out);
            }
          }
        }
      }
    } else if (type == ePdfContentsType_Variant) {
      stack.push(var);
    } else if (type == ePdfContentsType_ImageData) {
    } else {
      throw Error::New(info.Env(), "Something has gone terribly wrong :(");
    }
  }
  return out.Get(Napi::Symbol::WellKnown(info.Env(), "iterator"))
    .As<Napi::Function>()
    .Call(out, {});
}

void
ContentsTokenizer::AddText(PdfFont* font,
                           const PdfString& text,
                           Napi::Array& out)
{
  if (!font || !font->GetEncoding()) {
    // handle error
    return;
  }
  PdfString unicode = font->GetEncoding()->ConvertToUnicode(text, font);
  const char* chunk = unicode.GetStringUtf8().c_str();
  out.Set(out.Length(), chunk);
}
}
