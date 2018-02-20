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
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "Font.h"
#include "Page.h"
#include "Painter.h"

using namespace PoDoFo;
using namespace Napi;

namespace NoPoDoFo {

FunctionReference SimpleTable::constructor;

SimpleTable::SimpleTable(const CallbackInfo& info)
  : ObjectWrap(info)
{
  AssertFunctionArgs(info, 3, { napi_object, napi_number, napi_number });
  doc = Document::Unwrap(info[0].As<Object>());
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
  delete backgroundColor;
  delete foregroundColor;
  doc = nullptr;
}

void
SimpleTable::Initialize(Napi::Env& env, Napi::Object& target)
{
  HandleScope scope(env);
  const auto ctor = DefineClass(
    env,
    "SimpleTable",
    { InstanceAccessor("font", nullptr, &SimpleTable::SetFont),
      InstanceAccessor("text", nullptr, &SimpleTable::SetText),
      InstanceAccessor("borderWidth",
                       &SimpleTable::GetBorderWidth,
                       &SimpleTable::SetBorderWidth),
      InstanceAccessor(
        "foregroundColor", nullptr, &SimpleTable::SetForegroundColor),
      InstanceAccessor(
        "backgroundColor", nullptr, &SimpleTable::SetBackgroundColor),
      InstanceAccessor("alignment", nullptr, &SimpleTable::SetAlignment),
      InstanceAccessor("wordWrap", nullptr, &SimpleTable::SetWordWrapEnabled),
      InstanceMethod("getFont", &SimpleTable::GetFont),
      InstanceMethod("getText", &SimpleTable::GetText),
      InstanceMethod("getForegroundColor", &SimpleTable::GetForegroundColor),
      InstanceMethod("getBackgroundColor", &SimpleTable::GetBackgroundColor),
      InstanceMethod("getAlignment", &SimpleTable::GetAlignment),
      InstanceMethod("getVerticalAlignment",
                     &SimpleTable::GetVerticalAlignment),
      InstanceMethod("getWordWrap", &SimpleTable::HasWordWrap),
      InstanceMethod("borderEnable", &SimpleTable::SetBorderEnabled),
      InstanceMethod("hasBorders", &SimpleTable::HasBorders),
      InstanceMethod("getImage", &SimpleTable::GetImage),
      InstanceMethod("hasImage", &SimpleTable::HasImage),
      InstanceMethod("hasBackgroundColor", &SimpleTable::HasBackgroundColor),
      InstanceMethod("enableBackground", &SimpleTable::SetBackgroundEnabled),
      InstanceMethod("getBorderColor", &SimpleTable::GetBorderColor),
      InstanceAccessor("tableWidth", nullptr, &SimpleTable::SetTableWidth),
      InstanceAccessor("tableHeight", nullptr, &SimpleTable::SetTableHeight),
      InstanceAccessor("autoPageBreak",
                       &SimpleTable::GetAutoPageBreak,
                       &SimpleTable::SetAutoPageBreak),
      InstanceMethod("draw", &SimpleTable::Draw),
      InstanceMethod("getTableWidth", &SimpleTable::GetWidth),
      InstanceMethod("getTableHeight", &SimpleTable::GetHeight),
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Font::constructor.New(
    { External<PdfFont>::New(info.Env(), model->GetFont(col, row)) });
}

void
SimpleTable::SetFont(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsObject() ||
      !value.As<Object>().InstanceOf(Font::constructor.Value())) {
    throw Error::New(info.Env(), "value must be an instance of NoPoDoFo Font");
  }
  const auto pFont = Font::Unwrap(value.As<Object>())->GetPoDoFoFont();
  model->SetFont(pFont);
}

Value
SimpleTable::GetText(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return String::New(info.Env(), model->GetText(col, row).GetStringUtf8());
}

void
SimpleTable::SetText(const CallbackInfo& info, const Napi::Value& value)
{
  const int col = value.As<Object>().Get("col").As<Number>();
  const int row = value.As<Object>().Get("row").As<Number>();
  const auto text = value.As<Object>().Get("text").As<String>().Utf8Value();
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  const int col = info[0].As<Number>();
  const int row = info[1].As<Number>();
  return Boolean::New(info.Env(), model->HasImage(col, row));
}
Value
SimpleTable::GetForegroundColor(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
    Array a = value.As<Array>();
    foregroundColor = SetColor(a);
    model->SetForegroundColor(*foregroundColor);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
SimpleTable::GetBackgroundColor(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
    auto a = value.As<Array>();
    backgroundColor = SetColor(a);
    model->SetBackgroundColor(*backgroundColor);
  } catch (PdfError& err) {
    ErrorHandler(err, info);
  }
}

Value
SimpleTable::HasBackgroundColor(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
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
  AssertFunctionArgs(info, 2, { napi_object, napi_object });
  const Object point = info[0].As<Object>();
  const double posX = point.Get("x").As<Number>();
  const double posY = point.Get("y").As<Number>();
  auto painter = Painter::Unwrap(info[1].As<Object>());
  if (!table->GetModel()) {
    cout << model->GetText(0, 0).GetStringUtf8() << endl;
    table->SetModel(model);
  }
  table->Draw(posX, posY, painter->GetPainter());
}

Value
SimpleTable::GetWidth(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 3, { napi_number, napi_number, napi_object });
  const double posX = info[0].As<Number>();
  const double posY = info[1].As<Number>();
  auto page = Page::Unwrap(info[2].As<Object>());
  const auto w = table->GetWidth(posX, posY, page->GetPage());
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
  AssertFunctionArgs(info, 3, { napi_number, napi_number, napi_object });
  const double posX = info[0].As<Number>();
  const double posY = info[1].As<Number>();
  auto page = Page::Unwrap(info[2].As<Object>());
  const auto w = table->GetHeight(posX, posY, page->GetPage());
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
  auto* d = static_cast<void*>(doc->GetDocument());
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
      const double r = js.Get(i).As<Number>();
      const double g = js.Get(++i).As<Number>();
      const double b = js.Get(++i).As<Number>();
      color = new PdfColor(r, g, b);
    } else if (js.Length() == 4) {
      // cmyk color
      const double c = js.Get(i).As<Number>();
      const double m = js.Get(++i).As<Number>();
      const double y = js.Get(++i).As<Number>();
      const double k = js.Get(++i).As<Number>();
      color = new PdfColor(c, m, y, k);
    } else if (js.Length() == 1) {
      // gray scale
      const double gs = js.Get(i).As<Number>();
      color = new PdfColor(gs);
    }
  } catch (PdfError& err) {
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
