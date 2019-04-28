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
#include "Document.h"
#include "Font.h"
#include "Page.h"
#include "Painter.h"
#include "StreamDocument.h"
#include <optional/optional.hpp>
#include <spdlog/spdlog.h>

using namespace PoDoFo;
using namespace Napi;
using tl::nullopt;

namespace NoPoDoFo {

FunctionReference SimpleTable::Constructor; // NOLINT

SimpleTable::SimpleTable(const CallbackInfo& info)
  : ObjectWrap(info)
{
  DbgLog = spdlog::get("DbgLog");
  if (info[0].As<Object>().InstanceOf(Document::Constructor.Value())) {
    Doc = Document::Unwrap(info[0].As<Object>())->Base;
  } else if (info[0].As<Object>().InstanceOf(
               StreamDocument::Constructor.Value())) {
    Doc = StreamDocument::Unwrap(info[0].As<Object>())->Base;
  }
  const int cols = info[1].As<Number>();
  const int rows = info[2].As<Number>();
  Model = new PdfSimpleTableModel(cols, rows);
  Table = new PdfTable(cols, rows);
}

SimpleTable::~SimpleTable()
{
  DbgLog->debug("SimpleTable Cleanup");
  HandleScope scope(Env());
  delete Model;
  delete Table;
  Doc = nullptr;
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
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("SimpleTable", ctor);
}

Value
SimpleTable::GetFont(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Font::Constructor.New(
    { External<PdfFont>::New(info.Env(), Model->GetFont(col, row)) });
}

void
SimpleTable::SetFont(const CallbackInfo& info)
{
  const auto value = info[0].As<Object>();
  if (!value.IsObject() ||
      !value.As<Object>().InstanceOf(Font::Constructor.Value())) {
    throw Error::New(info.Env(), "value must be an instance of NoPoDoFo Font");
  }
  Model->SetFont(&Font::Unwrap(value.As<Object>())->GetFont());
}

Value
SimpleTable::GetText(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return String::New(info.Env(), Model->GetText(col, row).GetStringUtf8());
}

void
SimpleTable::SetText(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  const auto text = info[2].As<String>().Utf8Value();
  Model->SetText(col, row, text);
}

Value
SimpleTable::GetBorderWidth(const CallbackInfo& info)
{
  return Number::New(info.Env(), Model->GetBorderWidth());
}

void
SimpleTable::SetBorderWidth(const CallbackInfo& info, const Napi::Value& value)
{
  Model->SetBorderWidth(value.As<Number>().DoubleValue());
}

Value
SimpleTable::GetBorderColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto color = Model->GetBorderColor(col, row);
  return Color::Constructor.New(
    { External<PdfColor>::New(info.Env(), &color) });
}

void
SimpleTable::SetBorderEnabled(const CallbackInfo& info)
{
  Model->SetBorderEnabled(info[0].As<Boolean>());
}

Value
SimpleTable::HasBorders(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), Model->HasBorders());
}

Value
SimpleTable::GetImage(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto image = Model->GetImage(col, row);
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
  return Boolean::New(info.Env(), Model->HasImage(col, row));
}
Value
SimpleTable::GetForegroundColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto color = Model->GetForegroundColor(col, row);
  return Color::Constructor.New(
    { External<PdfColor>::New(info.Env(), &color) });
}
void
SimpleTable::SetForegroundColor(const CallbackInfo& info,
                                const Napi::Value& value)
{

  Model->SetForegroundColor(*Color::Unwrap(info[0].As<Object>())->Self);
}

Value
SimpleTable::GetBackgroundColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  auto color = Model->GetForegroundColor(col, row);
  return Color::Constructor.New(
    { External<PdfColor>::New(info.Env(), &color) });
}

void
SimpleTable::SetBackgroundColor(const CallbackInfo& info,
                                const Napi::Value& value)
{
  Model->SetBackgroundColor(*Color::Unwrap(info[0].As<Object>())->Self);
}

Value
SimpleTable::HasBackgroundColor(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Boolean::New(info.Env(), Model->HasBackgroundColor(col, row));
}

void
SimpleTable::SetBackgroundEnabled(const CallbackInfo& info)
{
  Model->SetBackgroundEnabled(info[0].As<Boolean>());
}

Value
SimpleTable::GetAlignment(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  const int alignment = Model->GetAlignment(col, row);
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
  Model->SetAlignment(static_cast<EPdfAlignment>(i));
}

Value
SimpleTable::GetVerticalAlignment(const CallbackInfo& info)
{
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  string i;
  switch (static_cast<int>(Model->GetVerticalAlignment(col, row))) {
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
  return Boolean::New(info.Env(), Model->HasWordWrap(col, row));
}

void
SimpleTable::SetWordWrapEnabled(const CallbackInfo& info,
                                const Napi::Value& value)
{
  Model->SetWordWrapEnabled(value.As<Boolean>());
}

void
SimpleTable::Draw(const CallbackInfo& info)
{
  const Object point = info[0].As<Object>();
  const double posX = point.Get("x").As<Number>();
  const double posY = point.Get("y").As<Number>();
  auto painter = Painter::Unwrap(info[1].As<Object>());
  if (!Table->GetModel()) {
    Table->SetModel(Model);
  }
  Table->Draw(posX, posY, &painter->GetPainter());
}

Value
SimpleTable::GetWidth(const CallbackInfo& info)
{
  const double posX = info[0].As<Number>();
  const double posY = info[1].As<Number>();
  auto page = Page::Unwrap(info[2].As<Object>());
  const auto w = Table->GetWidth(posX, posY, &page->Self);
  return Number::New(info.Env(), w);
}

void
SimpleTable::SetTableWidth(const CallbackInfo& info, const Napi::Value& value)
{
  Table->SetTableWidth(value.As<Number>().DoubleValue());
}

Value
SimpleTable::GetHeight(const CallbackInfo& info)
{
  const double posX = info[0].As<Number>();
  const double posY = info[1].As<Number>();
  auto page = Page::Unwrap(info[2].As<Object>());
  const auto w = Table->GetHeight(posX, posY, &page->Self);
  return Number::New(info.Env(), w);
}

void
SimpleTable::SetTableHeight(const CallbackInfo& info, const Napi::Value& value)
{
  Table->SetTableHeight(value.As<Number>().DoubleValue());
}

Value
SimpleTable::GetCols(const CallbackInfo& info)
{
  return Number::New(info.Env(), Table->GetCols());
}

Value
SimpleTable::GetRows(const CallbackInfo& info)
{
  return Number::New(info.Env(), Table->GetRows());
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
  Table->SetColumnWidths(ws);
}

void
SimpleTable::SetColumnWidth(const CallbackInfo& info)
{
  Table->SetColumnWidth(info[0].As<Number>().DoubleValue());
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
  Table->SetRowHeights(pdHeights);
}

void
SimpleTable::SetRowHeight(const CallbackInfo& info)
{
  Table->SetRowHeight(info[0].As<Number>().DoubleValue());
}

void
SimpleTable::SetAutoPageBreak(const CallbackInfo& info,
                              const Napi::Value& value)
{
  auto* d = static_cast<void*>(Doc);
  Table->SetAutoPageBreak(
    value.As<Boolean>(),
    [](PdfRect& rect, void* data) -> PdfPage* {
      return static_cast<PdfMemDocument*>(data)->CreatePage(rect);
    },
    d);
}

Value
SimpleTable::GetAutoPageBreak(const CallbackInfo& info)
{
  return Boolean::New(info.Env(), Table->GetAutoPageBreak());
}

}
