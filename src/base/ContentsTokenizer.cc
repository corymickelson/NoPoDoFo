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
#include "../doc/Page.h"
#include <iostream>
#include <stack>

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference ContentsTokenizer::constructor;

ContentsTokenizer::ContentsTokenizer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 2, { napi_object, napi_object });
  auto wrap = info[0].As<Object>();
  if (!wrap.InstanceOf(Page::constructor.Value())) {
    throw Error::New(info.Env(), "must be an instance of Page");
  }
  auto dWrap = info[1].As<Object>();
  if (!dWrap.InstanceOf(Document::constructor.Value())) {
    throw Error::New(info.Env(), "must be an instance of Document");
  }
  doc = Document::Unwrap(dWrap);
  page = Page::Unwrap(wrap);
  self = new PdfContentsTokenizer(Page::Unwrap(wrap)->GetPage());
}

ContentsTokenizer::~ContentsTokenizer()
{
  if (self != nullptr) {
    HandleScope scope(Env());
    delete self;
    page = nullptr;
    doc = nullptr;
  }
}

void
ContentsTokenizer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  Function ctor =
    DefineClass(env,
                "ContentsTokenizer",
                { InstanceMethod("readAll", &ContentsTokenizer::ReadAll) });
  constructor = Persistent(ctor);
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
  //  double posX = 0.0, posY = 0.0;
  bool blockText = false;
  PdfFont* font = nullptr;
  Napi::Array out = Array::New(info.Env());

  while (self->ReadNext(type, token, var)) {
    if (type == ePdfContentsType_Keyword) {
      if (strcmp(token, "l") == 0 || strcmp(token, "m") == 0) {
        if (stack.size() == 2) {
          //          posX = stack.top().GetReal();
          stack.pop();
          //          posY = stack.top().GetReal();
        } else {
          stringstream msg;
          msg << "WARNING: Token '" << token << "' expects two arguments, but %"
              << static_cast<pdf_int64>(stack.size()) << " given; ignoring\n"
              << endl;
          std::cout << msg.str() << endl;
          //          throw Error::New(info.Env(), msg.str());
        }
      } else if (strcmp(token, "BT") == 0) {
        blockText = true;
      } else if (strcmp(token, "ET") == 0) {
        if (!blockText) {
          //          throw Error::New(info.Env(), "WARNGIN: Found ET without
          //          BT");
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
            page->GetPage()->GetFromResources(PdfName("Font"), fontName);
          if (!pFont) {
            throw Error::New(info.Env(), "Failed to create font");
          }
          font = doc->GetDocument()->GetFont(pFont);
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
  return out;
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
  printf("%s \n", unicode.GetStringUtf8().c_str());
}
}
