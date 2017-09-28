//
// Created by red on 9/11/17.
//

#include "Annotation.h"

Annotation::Annotation(const CallbackInfo& info)
  : ObjectWrap(info)
{
  if (info.Length() < 3) {
    throw Napi::Error::New(info.Env(),
                           "Annotation constructor requires three arguments of "
                           "type: Page, NPdfAnnotation, Rect");
  }
  auto pageObj = info[0].As<Object>();
  int npdfAnnotation = info[1].As<Number>();
  auto rectObj = info[2].As<Object>();
  auto page = Page::Unwrap(pageObj);
  auto rect = Rect::Unwrap(rectObj);
  doc = page->GetDocument();
  annot = page->GetPage()->CreateAnnotation(
    static_cast<PoDoFo::EPdfAnnotation>(npdfAnnotation), rect->GetRect());
}

void
Annotation::SetFlags(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsNumber() == false) {
    throw Napi::TypeError::New(
      info.Env(), "SetFlag must be an instance of NpdfAnnotationType");
  }
  int jsValue = info[0].As<Number>();
  auto flag = static_cast<PoDoFo::EPdfAnnotationFlags>(jsValue);
  annot->SetFlags(flag);
}

Napi::Value
Annotation::GetFlags(const CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), annot->GetFlags());
}

Napi::Value
Annotation::HasAppearanceStream(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), annot->HasAppearanceStream());
}

void
Annotation::SetBorderStyle(const CallbackInfo& info)
{
  if (info.Length() != 3) {
    throw Napi::Error::New(info.Env(),
                           "Border requires three arguments of type number."
                           "Number - horizontal corner radius"
                           "Number - vertical corner radius"
                           "Number - width");
  }
  double horizontal = info[0].As<Number>();
  double vertical = info[1].As<Number>();
  double width = info[2].As<Number>();
  annot->SetBorderStyle(horizontal, vertical, width);
}

void
Annotation::SetTitle(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsString() == false) {
    throw Napi::TypeError::New(
      info.Env(), "SetTitle requires a single argument of type string.");
  }
  try {
    string title = info[0].As<String>().Utf8Value();
    annot->SetTitle(PdfString(title));
  } catch (PdfError& err) {
    stringstream msg;
    msg << "PoDoFo PdfError: " << err.GetError() << endl;
    throw Napi::Error::New(info.Env(), msg.str());
  }
}

Napi::Value
Annotation::GetTitle(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), annot->GetTitle().GetString());
}

void
Annotation::SetContent(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsEmpty()) {
    throw Napi::Error::New(info.Env(),
                           "SetContent requires string \"value\" argument.");
  }
  string content = value.As<String>().Utf8Value();
  annot->SetContents(content);
}

Napi::Value
Annotation::GetContent(const CallbackInfo& info)
{
  return Napi::String::New(info.Env(), annot->GetContents().GetString());
}

void
Annotation::SetDestination(const CallbackInfo& info, const Napi::Value& value)
{
  throw Napi::Error::New(info.Env(), "Not implemented.");
}

Napi::Value
Annotation::GetDestination(const CallbackInfo& info)
{
  throw Napi::Error::New(info.Env(), "Not implemented.");
}

Value
Annotation::HasDestination(const CallbackInfo& info)
{
  throw Napi::Error::New(info.Env(), "Not implemented.");
}

void
Annotation::SetAction(const CallbackInfo& info, const Napi::Value& value)
{
  Object actionObj = value.As<Object>();
  int type = actionObj.Get("type").As<Number>();
  string uri = actionObj.Get("uri").As<String>().Utf8Value();
  PoDoFo::EPdfAction flag = static_cast<PoDoFo::EPdfAction>(type);
  PdfAction action(flag, doc);
  action.SetURI(uri);
  annot->SetAction(action);
}

Napi::Value
Annotation::GetAction(const CallbackInfo& info)
{
  PdfAction* currentAction = annot->GetAction();
  if (currentAction->HasScript()) {
    return Napi::String::New(info.Env(),
                             currentAction->GetScript().GetString());
  } else if (currentAction->HasURI()) {
    return Napi::String::New(info.Env(), currentAction->GetURI().GetString());
  } else {
    throw Napi::Error::New(info.Env(), "Action not set");
  }
}

Napi::Value
Annotation::HasAction(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), annot->HasAction());
}

void
Annotation::SetOpen(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsBoolean() == false) {
    throw Napi::Error::New(info.Env(), "Requires Boolean type");
  }
  annot->SetOpen(value.As<Boolean>());
}

Napi::Value
Annotation::GetOpen(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), annot->GetOpen());
}

void
Annotation::SetColor(const CallbackInfo& info, const Napi::Value& value)
{
  if (value.IsArray()) {
    Array jsValue = value.As<Array>();
    int rgb[3];
    for (uint8_t i = 0; i < jsValue.Length(); i++) {
      rgb[i] = jsValue.Get(i).As<Number>();
    }
    annot->SetColor(rgb[0], rgb[1], rgb[2]);
  } else {
    throw Napi::TypeError::New(info.Env(),
                               "Requires RGB color: [Number, Number, Number]");
  }
}

Napi::Value
Annotation::GetColor(const CallbackInfo& info)
{
  Array rgbArray = Napi::Array::New(info.Env());
  PdfArray pdfRgb = annot->GetColor();
  if (pdfRgb.size() != 3) {
  }
  double r = pdfRgb[0].GetNumber();
  double g = pdfRgb[1].GetNumber();
  double b = pdfRgb[2].GetNumber();
  rgbArray.Set(static_cast<uint8_t>(0), Napi::Number::New(info.Env(), r));
  rgbArray.Set(static_cast<uint8_t>(1), Napi::Number::New(info.Env(), g));
  rgbArray.Set(static_cast<uint8_t>(2), Napi::Number::New(info.Env(), b));
  return rgbArray;
}

Napi::Value
Annotation::GetType(const CallbackInfo& info)
{
  string jsType;
  switch (annot->GetType()) {
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_3D: {
      jsType = "3D";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Caret: {
      jsType = "Caret";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Circle: {
      jsType = "Circle";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_FileAttachement: {
      jsType = "FileAttachment";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_FreeText: {
      jsType = "FreeText";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Highlight: {
      jsType = "HighLight";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Ink: {
      jsType = "Ink";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Line: {
      jsType = "Line";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Link: {
      jsType = "Link";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Movie: {
      jsType = "Movie";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_PolyLine: {
      jsType = "PolyLine";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Polygon: {
      jsType = "Polygon";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Popup: {
      jsType = "Popup";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_PrinterMark: {
      jsType = "PrintMark";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_RichMedia: {
      jsType = "RichMedia";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Screen: {
      jsType = "Screen";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Sound: {
      jsType = "Sound";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Square: {
      jsType = "Square";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Squiggly: {
      jsType = "Squiggly";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Stamp: {
      jsType = "Stamp";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_StrikeOut: {
      jsType = "StikeOut";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Text: {
      jsType = "Text";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_TrapNet: {
      jsType = "TrapNet";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Unknown: {
      jsType = "Unknown";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Underline: {
      jsType = "Underline";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Watermark: {
      jsType = "Watermark";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_WebMedia: {
      jsType = "WebMedia";
      break;
    }
    case PoDoFo::EPdfAnnotation::ePdfAnnotation_Widget: {
      jsType = "Widget";
      break;
    }
  }
  return Napi::String::New(info.Env(), jsType);
}

void
Annotation::SetQuadPoints(const CallbackInfo& info, const Napi::Value& value)
{}

Napi::Value
Annotation::GetQuadPoints(const CallbackInfo& info)
{}

void
Annotation::SetFileAttachment(const CallbackInfo& info)
{}

Napi::Value
Annotation::HasFileAttachment(const CallbackInfo& info)
{
  return Napi::Boolean::New(info.Env(), annot->HasFileAttachement());
}
