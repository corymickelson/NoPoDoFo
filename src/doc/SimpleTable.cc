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

#include "SimpleTable.h"
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "../base/Color.h"
#include "Font.h"
#include "Page.h"
#include "Painter.h"
#include "StreamDocument.h"
#include "Document.h"
#include <optional/optional.hpp>

using namespace PoDoFo;
using namespace Napi;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference SimpleTable::constructor; // NOLINT

SimpleTable::SimpleTable(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info[0].As<Object>().InstanceOf(Document::constructor.Value())) {
    doc = Document::Unwrap(info[0].As<Object>())->base;
  } else if (info[0].As<Object>().InstanceOf(
               StreamDocument::constructor.Value())) {
    doc = StreamDocument::Unwrap(info[0].As<Object>())->base;
  }
  const int cols = info[1].As<Number>();
  const int rows = info[2].As<Number>();
  model = new PdfSimpleTableModel(cols, rows);
  table = new PdfTable(cols, rows);
}

SimpleTable::~SimpleTable()
{
  HandleScope scope(Env());
  delete model;
  delete table;
  doc = nullptr;
}

void
SimpleTable::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  const auto ctor = DefineClass(
    env,
    "SimpleTable",
    { InstanceAccessor("borderWidth",
                       &SimpleTable::GetBorderWidth,
                       &SimpleTable::SetBorderWidth),
      InstanceAccessor("foregroundColor",
                       &SimpleTable::GetForegroundColor,
                       &SimpleTable::SetForegroundColor),
      InstanceAccessor("backgroundColor",
                       &SimpleTable::GetBackgroundColor,
                       &SimpleTable::SetBackgroundColor),
      InstanceAccessor(
        "alignment", &SimpleTable::GetAlignment, &SimpleTable::SetAlignment),
      InstanceAccessor("wordWrap",
                       &SimpleTable::HasWordWrap,
                       &SimpleTable::SetWordWrapEnabled),
      InstanceAccessor(
        "tableWidth", &SimpleTable::GetWidth, &SimpleTable::SetTableWidth),
      InstanceAccessor(
        "tableHeight", &SimpleTable::GetHeight, &SimpleTable::SetTableHeight),
      InstanceAccessor("autoPageBreak",
                       &SimpleTable::GetAutoPageBreak,
                       &SimpleTable::SetAutoPageBreak),
      InstanceMethod("getFont", &SimpleTable::GetFont),
      InstanceMethod("setFont", &SimpleTable::SetFont),
      InstanceMethod("getText", &SimpleTable::GetText),
      InstanceMethod("setText", &SimpleTable::SetText),
      InstanceMethod("getVerticalAlignment",
                     &SimpleTable::GetVerticalAlignment),
      InstanceMethod("borderEnable", &SimpleTable::SetBorderEnabled),
      InstanceMethod("hasBorders", &SimpleTable::HasBorders),
      InstanceMethod("getImage", &SimpleTable::GetImage),
      InstanceMethod("hasImage", &SimpleTable::HasImage),
      InstanceMethod("hasBackgroundColor", &SimpleTable::HasBackgroundColor),
      InstanceMethod("enableBackground", &SimpleTable::SetBackgroundEnabled),
      InstanceMethod("getBorderColor", &SimpleTable::GetBorderColor),
      InstanceMethod("draw", &SimpleTable::Draw),
      InstanceMethod("columnCount", &SimpleTable::GetCols),
      InstanceMethod("rowCount", &SimpleTable::GetRows),
      InstanceMethod("setColumnWidths", &SimpleTable::SetColumnWidths),
      InstanceMethod("setColumnWidth", &SimpleTable::SetColumnWidth),
      InstanceMethod("setRowHeight", &SimpleTable::SetRowHeight),
      InstanceMethod("setRowHeights", &SimpleTable::SetRowHeights) });
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  target.Set("SimpleTable", ctor);
}

Value
SimpleTable::GetFont(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Font::constructor.New(
    { External<PdfFont>::New(info.Env(), model->GetFont(col, row)) });
}

void
SimpleTable::SetFont(const CallbackInfo& info)
{
  auto value = info[0].As<Object>();
  if (!value.IsObject() ||
      !value.As<Object>().InstanceOf(Font::constructor.Value())) {
    throw Error::New(info.Env(), "value must be an instance of NoPoDoFo Font");
  }
  model->SetFont(&Font::Unwrap(value.As<Object>())->GetFont());
}

Value
SimpleTable::GetText(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return String::New(info.Env(), model->GetText(col, row).GetStringUtf8());
}

void
SimpleTable::SetText(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  const auto text = info[2].As<String>().Utf8Value();
  model->SetText(col, row, text);
}

Value
SimpleTable::GetBorderWidth(const CallbackInfo& info)
{
  return Number::New(info.Env(), model->GetBorderWidth());
}

void
SimpleTable::SetBorderWidth(const CallbackInfo& info, const Napi::Value& value)
{
  model->SetBorderWidth(value.As<Number>().DoubleValue());
}

Value
SimpleTable::GetBorderColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto color = model->GetBorderColor(col, row);
  auto colorValues = color.ToArray();
  auto js = Array::New(info.Env());
  GetColor(color, js);
  return js;
}

void
SimpleTable::SetBorderEnabled(const CallbackInfo& info)
{
  model->SetBorderEnabled(info[0].As<Boolean>());
}

Value
SimpleTable::HasBorders(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), model->HasBorders());
}

Value
SimpleTable::GetImage(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto image = model->GetImage(col, row);
  auto* pStream = dynamic_cast<PdfMemStream*>(image->GetObject()->GetStream());
  const auto stream = pStream->Get();
  const auto length = pStream->GetLength();
  const auto value =
    Buffer<char>::Copy(info.Env(), stream, static_cast<size_t>(length));
  return static_cast<Napi::Value>(value);
}
Value
SimpleTable::HasImage(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Boolean::New(info.Env(), model->HasImage(col, row));
}
Value
SimpleTable::GetForegroundColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto color = model->GetForegroundColor(col, row);
  auto js = Array::New(info.Env());
  GetColor(color, js);
  return js;
}
void
SimpleTable::SetForegroundColor(const CallbackInfo& info,
                                const Napi::Value& value)
{
  try {
    if (info[0].IsArray()) {
      Array a = value.As<Array>();
      model->SetForegroundColor(*SetColor(a));

    } else if (info[0].IsObject() &&
               info[0].As<Object>().InstanceOf(Color::constructor.Value())) {
      model->SetForegroundColor(*Color::Unwrap(info[0].As<Object>())->color);
    }
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
SimpleTable::GetBackgroundColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto color = model->GetForegroundColor(col, row).ToArray();
  auto js = Array::New(info.Env());
  uint32_t n = 1;
  for (auto& i : color) {
    js.Set(n, i.GetNumber());
    ++n;
  }
  return js;
}

void
SimpleTable::SetBackgroundColor(const CallbackInfo& info,
                                const Napi::Value& value)
{
  try {
    if (info[0].IsArray()) {
      auto a = value.As<Array>();
      model->SetBackgroundColor(*SetColor(a));
    } else if (info[0].As<Object>().InstanceOf(Color::constructor.Value())) {
      model->SetBackgroundColor(*Color::Unwrap(info[0].As<Object>())->color);
    }
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
SimpleTable::HasBackgroundColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Boolean::New(info.Env(), model->HasBackgroundColor(col, row));
}

void
SimpleTable::SetBackgroundEnabled(const CallbackInfo& info)
{
  model->SetBackgroundEnabled(info[0].As<Boolean>());
}

Value
SimpleTable::GetAlignment(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  const int alignment = model->GetAlignment(col, row);
  string alignmentResponse;
  switch (alignment) {
    case 0:
      alignmentResponse = "LEFT";
      break;
    case 1:
      alignmentResponse = "CENTER";
      break;
    case 2:
      alignmentResponse = "RIGHT";
      break;
    default:
      break;
  }
  return String::New(info.Env(), alignmentResponse);
}

void
SimpleTable::SetAlignment(const CallbackInfo& info, const Napi::Value& value)
{

  int i;
  if (value.As<String>().Utf8Value() == "LEFT")
    i = 0;
  else if (value.As<String>().Utf8Value() == "CENTER")
    i = 1;
  else if (value.As<String>().Utf8Value() == "RIGHT")
    i = 2;
  else {
    throw Error::New(info.Env(),
                     R"(value must be one of ["LEFT", "CENTER", "RIGHT"])");
  }
  model->SetAlignment(static_cast<EPdfAlignment>(i));
}

Value
SimpleTable::GetVerticalAlignment(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  string i;
  switch (static_cast<int>(model->GetVerticalAlignment(col, row))) {
    case 0:
      i = "TOP";
      break;
    case 1:
      i = "CENTER";
      break;
    case 2:
      i = "BOTTOM";
      break;
    default:
      throw Error::New(info.Env(), "Unknown identifier");
  }
  return String::New(info.Env(), i);
}

Value
SimpleTable::HasWordWrap(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Boolean::New(info.Env(), model->HasWordWrap(col, row));
}

void
SimpleTable::SetWordWrapEnabled(const CallbackInfo& info,
                                const Napi::Value& value)
{
  model->SetWordWrapEnabled(value.As<Boolean>());
}

void
SimpleTable::Draw(const CallbackInfo& info)
{
  const Object point = info[0].As<Object>();
  const double posX = point.Get("x").As<Number>();
  const double posY = point.Get("y").As<Number>();
  auto painter = Painter::Unwrap(info[1].As<Object>());
  if (!table->GetModel()) {
    table->SetModel(model);
  }
  table->Draw(posX, posY, &painter->GetPainter());
}

Value
SimpleTable::GetWidth(const CallbackInfo& info)
{
  const double posX = info[0].As<Number>();
  const double posY = info[1].As<Number>();
  auto page = Page::Unwrap(info[2].As<Object>());
  const auto w = table->GetWidth(posX, posY, &page->page);
  return Number::New(info.Env(), w);
}

void
SimpleTable::SetTableWidth(const CallbackInfo& info, const Napi::Value& value)
{
  table->SetTableWidth(value.As<Number>().DoubleValue());
}

Value
SimpleTable::GetHeight(const CallbackInfo& info)
{
  const double posX = info[0].As<Number>();
  const double posY = info[1].As<Number>();
  auto page = Page::Unwrap(info[2].As<Object>());
  const auto w = table->GetHeight(posX, posY, &page->page);
  return Number::New(info.Env(), w);
}

void
SimpleTable::SetTableHeight(const CallbackInfo& info, const Napi::Value& value)
{
  table->SetTableHeight(value.As<Number>().DoubleValue());
}

Value
SimpleTable::GetCols(const CallbackInfo& info)
{
  return Number::New(info.Env(), table->GetCols());
}

Value
SimpleTable::GetRows(const CallbackInfo& info)
{
  return Number::New(info.Env(), table->GetRows());
}

void
SimpleTable::SetColumnWidths(const CallbackInfo& info)
{
  const auto widths = info[0].As<Array>();
  const auto ws = static_cast<double*>(calloc(sizeof(double), widths.Length()));
  const auto array = info[0].As<Array>();
  for (uint32_t i = 0; i < array.Length(); i++) {
    const double index = array.Get(i).As<Number>();
    ws[i] = index;
  }
  table->SetColumnWidths(ws);
}

void
SimpleTable::SetColumnWidth(const CallbackInfo& info)
{
  table->SetColumnWidth(info[0].As<Number>().DoubleValue());
}

void
SimpleTable::SetRowHeights(const CallbackInfo& info)
{
  auto heights = info[0].As<Array>();
  const auto pdHeights =
    static_cast<double*>(calloc(sizeof(double), heights.Length()));
  auto array = info[0].As<Array>();
  for (uint32_t i = 0; i < array.Length(); i++) {
    const double index = array.Get(i).As<Number>();
    pdHeights[i] = index;
  }
  table->SetRowHeights(pdHeights);
}

void
SimpleTable::SetRowHeight(const CallbackInfo& info)
{
  table->SetRowHeight(info[0].As<Number>().DoubleValue());
}

void
SimpleTable::SetAutoPageBreak(const CallbackInfo& info,
                              const Napi::Value& value)
{
  auto* d = static_cast<void*>(doc);
  table->SetAutoPageBreak(
    value.As<Boolean>(),
    [](PdfRect& rect, void* data) -> PdfPage* {
      return static_cast<PdfMemDocument*>(data)->CreatePage(rect);
    },
    d);
}

Value
SimpleTable::GetAutoPageBreak(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), table->GetAutoPageBreak());
}

PdfColor*
SimpleTable::SetColor(Array& js)
{
  PdfColor* color = nullptr;
  try {
    uint32_t i = 0;
    if (js.Length() == 3) {
      const float r = js.Get(i).As<Number>();
      const float g = js.Get(++i).As<Number>();
      const float b = js.Get(++i).As<Number>();
      color = new PdfColor(r, g, b);
    } else if (js.Length() == 4) {
      // cmyk color
      const float c = js.Get(i).As<Number>();
      const float m = js.Get(++i).As<Number>();
      const float y = js.Get(++i).As<Number>();
      const float k = js.Get(++i).As<Number>();
      color = new PdfColor(c, m, y, k);
    } else if (js.Length() == 1) {
      // gray scale
      const float gs = js.Get(i).As<Number>();
      color = new PdfColor(gs);
    }
  } catch (PdfError& err) {
    err.PrintErrorMsg();
  }
  return color;
}

void
SimpleTable::GetColor(PdfColor& color, Array& js)
{
  uint32_t n = 0;
  for (auto& i : color.ToArray()) {
    js.Set(n, i.GetNumber());
    ++n;
  }
}
}
