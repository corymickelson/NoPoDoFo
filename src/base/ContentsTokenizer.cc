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

#include "ContentsTokenizer.h"
#include "../doc/Document.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stack>

using namespace Napi;
using namespace PoDoFo;
using std::make_unique;
using std::stringstream;

namespace NoPoDoFo {

FunctionReference ContentsTokenizer::Constructor; // NOLINT

/**
 * @note JS new ContentsTokenizer(doc: IBase, pageIndex: number)
 * @param info
 */
ContentsTokenizer::ContentsTokenizer(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
  , Doc(*Document::Unwrap(info[0].As<Object>()))
  , PageIndex(info[1].As<Number>().Int32Value())
{
  Self =
    make_unique<PdfContentsTokenizer>(Doc.GetDocument().GetPage(PageIndex));
  Log = spdlog::get("Log");
}

ContentsTokenizer::~ContentsTokenizer()
{
  if(Log != nullptr) Log->debug("ContentsTokenizer Cleanup");
}
void
ContentsTokenizer::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  auto ctor =
    DefineClass(env,
                "ContentsTokenizer",
                { InstanceMethod("readSync", &ContentsTokenizer::ReadSync),
                  InstanceMethod("read", &ContentsTokenizer::Read) });
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("ContentsTokenizer", ctor);
}

void
ContentsTokenizer::ReadIntoData()
{
  const char* token = nullptr;
  PdfVariant var;
  EPdfContentsType type;
  std::stack<PdfVariant> stack;
  auto blockText = false;
  PdfFont* font = nullptr;
  while (Self->ReadNext(type, token, var)) {
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
          auto fontName = stack.top().GetName();
          const auto pFont =
            Doc.GetDocument().GetPage(PageIndex)->GetFromResources(
              PdfName("Font"), fontName);
          if (!pFont) {
            throw std::runtime_error("Unable to create font object");
          }
          font = Doc.GetDocument().GetFont(pFont);
          if (!font) {
            throw std::runtime_error("Unable to create font object");
          }
        } else if (strcmp(token, "Tj") == 0 || strcmp(token, "'") == 0) {
          if (stack.empty())
            continue;
          AddText(font, stack.top().GetString());
          stack.pop();
        } else if (strcmp(token, "\"") == 0) {
          if (stack.size() < 3) {
            while (!stack.empty()) {
              stack.pop();
            }
            continue;
          }
          AddText(font, stack.top().GetString());
          stack.pop();
          stack.pop(); // remove char spacing
          stack.pop(); // remove word spacing
        } else if (strcmp(token, "TJ") == 0) {
          if (stack.empty())
            continue;
          auto array = stack.top().GetArray();
          stack.pop();
          for (auto& i : array) {
            if (i.IsString() || i.IsHexString()) {
              AddText(font, i.GetString());
            }
          }
        }
      }
    } else if (type == ePdfContentsType_Variant) {
      stack.push(var);
    } else if (type == ePdfContentsType_ImageData) {
    } else {
      throw std::runtime_error(
        "Contents Tokenizer failed with an unknown exception");
    }
  }
}
JsValue
ContentsTokenizer::ReadSync(const CallbackInfo& info)
{
  if (!Data.empty()) {
    cout << "Clearing previous results from ContentsTokenizer::Read" << endl;
    Data.clear();
    ContentsString = "";
  }
  auto out = Napi::Array::New(info.Env());
  try {
    ReadIntoData();
  } catch (...) {
    Error::New(
      info.Env(),
      "Unable to finish reader, please report this error to maintainer")
      .ThrowAsJavaScriptException();
    return {};
  }
  for (uint32_t i = 0; i < Data.size(); i++) {
    out.Set(i, String::New(info.Env(), Data[i]));
  }
  return out.Get(Napi::Symbol::WellKnown(info.Env(), "iterator"))
    .As<Napi::Function>()
    .Call(out, {});
}

void
ContentsTokenizer::AddText(PdfFont* font, const PdfString& text)
{
  if (!font || !font->GetEncoding()) {
    // handle error
    return;
  }
  const auto unicode = font->GetEncoding()->ConvertToUnicode(text, font);
  auto chunk = unicode.GetStringUtf8().c_str();
  Data.emplace_back(chunk);
}

class AsyncContentReader final : public AsyncWorker
{
public:
  AsyncContentReader(Function& cb, Napi::Object self)
    : AsyncWorker(cb, "async_content_reader", self)
    , Tokenizer(ContentsTokenizer::Unwrap(self))
  {}

protected:
  void Execute() override
  {
    try {
      Tokenizer->ReadIntoData();
    } catch (...) {
      SetError("Async Reader failure");
    }
  }
  void OnOK() override
  {
    if (Tokenizer->Data.empty()) {
      Callback().Call({ Env().Null(), Env().Null() });
      return;
    }
    string all;
    for (auto& i : Tokenizer->Data) {
      all += i;
    }
    Tokenizer->ContentsString = all;
    Callback().Call({ Env().Null(), String::New(Env(), all) });
  }

private:
  ContentsTokenizer* Tokenizer;
};

void
ContentsTokenizer::Read(const CallbackInfo& info)
{
  if (!Data.empty()) {
    cout << "Clearing previous results from ContentsTokenizer::Read" << endl;
    Data.clear();
    ContentsString = "";
  }
  if (info.Length() < 1 || !info[0].IsFunction()) {
    Error::New(info.Env(), "A Callback is required")
      .ThrowAsJavaScriptException();
    return;
  }
  auto cb = info[0].As<Function>();
  AsyncWorker* async = new AsyncContentReader(cb, this->Value());
  async->Queue();
}
}
