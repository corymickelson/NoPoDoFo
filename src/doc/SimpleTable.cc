//
// Created by red on 14/12/17.
//

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
  int cols = info[1].As<Number>();
  int rows = info[2].As<Number>();
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
  Function ctor = DefineClass(
    env,
    "SimpleTable",
    { InstanceAccessor("font", &SimpleTable::GetFont, &SimpleTable::SetFont),
      InstanceAccessor("text", &SimpleTable::GetText, &SimpleTable::SetText),
      InstanceAccessor("borderWidth",
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
      InstanceAccessor(
        "verticalAlignment", &SimpleTable::GetVerticalAlignment, nullptr),
      InstanceAccessor("wordWrap",
                       &SimpleTable::HasWordWrap,
                       &SimpleTable::SetWordWrapEnabled),
      InstanceMethod("borderEnable", &SimpleTable::SetBorderEnabled),
      InstanceMethod("hasBorders", &SimpleTable::HasBorders),
      InstanceMethod("getImage", &SimpleTable::GetImage),
      InstanceMethod("hasImage", &SimpleTable::HasImage),
      InstanceMethod("hasBackgroundColor", &SimpleTable::HasBackgroundColor),
      InstanceMethod("enableBackground", &SimpleTable::SetBackgroundEnabled),
      InstanceMethod("getBorderColor", &SimpleTable::GetBorderColor),
      InstanceAccessor(
        "tableWidth", &SimpleTable::GetWidth, &SimpleTable::SetTableWidth),
      InstanceAccessor(
        "tableHeight", &SimpleTable::GetHeight, &SimpleTable::SetTableHeight),
      InstanceAccessor("autoPageBreak",
                       &SimpleTable::GetAutoPageBreak,
                       &SimpleTable::SetAutoPageBreak),
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
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  return Font::constructor.New({ External<PdfFont>::New(
    info.Env(), model->GetFont(col, row), [](Napi::Env env, PdfFont* font) {
      HandleScope scope(env);
      delete font;
    }) });
}

void
SimpleTable::SetFont(const CallbackInfo& info, const Napi::Value& value)
{
  if (!value.IsObject() ||
      !value.As<Object>().InstanceOf(Font::constructor.Value())) {
    throw Error::New(info.Env(), "value must be an instance of NoPoDoFo Font");
  }
  PdfFont* pFont = Font::Unwrap(value.As<Object>())->GetPoDoFoFont();
  model->SetFont(pFont);
}

Value
SimpleTable::GetText(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  return String::New(info.Env(), model->GetText(col, row).GetStringUtf8());
}

void
SimpleTable::SetText(const CallbackInfo& info, const Napi::Value& value)
{
  int col = value.As<Object>().Get("col").As<Number>();
  int row = value.As<Object>().Get("row").As<Number>();
  string text = value.As<Object>().Get("text").As<String>().Utf8Value();
  model->SetText(col, row, PdfString(text));
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
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  PdfColor color = model->GetBorderColor(col, row);
  PdfArray colorValues = color.ToArray();
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
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  auto image = model->GetImage(col, row);
  auto* pStream = dynamic_cast<PdfMemStream*>(image->GetObject()->GetStream());
  auto stream = pStream->Get();
  auto length = pStream->GetLength();
  auto value =
    Buffer<char>::Copy(info.Env(), stream, static_cast<size_t>(length));
  return value;
}
Value
SimpleTable::HasImage(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  return Boolean::New(info.Env(), model->HasImage(col, row));
}
Value
SimpleTable::GetForegroundColor(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  PdfColor color = model->GetForegroundColor(col, row);
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
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  PdfArray color = model->GetForegroundColor(col, row).ToArray();
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
    Array a = value.As<Array>();
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
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
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
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
  auto alignment = model->GetAlignment(col, row);
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
                     "value must be one of [\"LEFT\", \"CENTER\", \"RIGHT\"]");
  }
  model->SetAlignment(static_cast<EPdfAlignment>(i));
}

Value
SimpleTable::GetVerticalAlignment(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
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
  }
  return String::New(info.Env(), i);
}

Value
SimpleTable::HasWordWrap(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 2, { napi_number, napi_number });
  int col = info[0].As<Number>();
  int row = info[1].As<Number>();
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
  AssertFunctionArgs(info, 3, { napi_number, napi_number, napi_object });
  double posX = info[0].As<Number>();
  double posY = info[1].As<Number>();
  Painter* painter = Painter::Unwrap(info[2].As<Object>());
  if (!table->GetModel()) {
    table->SetModel(model);
  }
  table->Draw(posX, posY, painter->GetPainter());
}

Value
SimpleTable::GetWidth(const CallbackInfo& info)
{
  AssertFunctionArgs(info, 3, { napi_number, napi_number, napi_object });
  double posX = info[0].As<Number>();
  double posY = info[1].As<Number>();
  Page* page = Page::Unwrap(info[2].As<Object>());
  double w = table->GetWidth(posX, posY, page->GetPage());
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
  double posX = info[0].As<Number>();
  double posY = info[1].As<Number>();
  Page* page = Page::Unwrap(info[2].As<Object>());
  double w = table->GetHeight(posX, posY, page->GetPage());
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
  double ws[info[0].As<Array>().Length()];
  auto array = info[0].As<Array>();
  for (uint32_t i = 0; i < array.Length(); i++) {
    double index = array.Get(i).As<Number>();
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
  double ws[info[0].As<Array>().Length()];
  auto array = info[0].As<Array>();
  for (uint32_t i = 0; i < array.Length(); i++) {
    double index = array.Get(i).As<Number>();
    ws[i] = index;
  }
  table->SetRowHeights(ws);
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
  void* d = static_cast<void*>(doc->GetDocument());
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
      // rgb color
      double r, g, b;
      r = js.Get(i).As<Number>();
      g = js.Get(++i).As<Number>();
      b = js.Get(++i).As<Number>();
      color = new PdfColor(r, g, b);
    } else if (js.Length() == 4) {
      double c, m, y, k;
      // cmyk color
      c = js.Get(i).As<Number>();
      m = js.Get(++i).As<Number>();
      y = js.Get(++i).As<Number>();
      k = js.Get(++i).As<Number>();
      color = new PdfColor(c, m, y, k);
    } else if (js.Length() == 1) {
      double gs;
      // gray scale
      gs = js.Get(i).As<Number>();
      color = new PdfColor(gs);
    }
  } catch (PdfError& err) {
  }
  return color;
}

void
SimpleTable::GetColor(PdfColor& color, Array& js)
{
  uint32_t n = 1;
  for (auto& i : color.ToArray()) {
    js.Set(n, i.GetNumber());
    ++n;
  }
}
}
