//
// Created by red on 9/7/17.
//

#include "Page.h"
#include "Field.h"

Napi::FunctionReference Page::constructor;

Page::Page(const CallbackInfo &info)
  : ObjectWrap(info)
{
  PoDoFo::PdfPage *page = info[0].As<Napi::External<PoDoFo::PdfPage>>().Data();
  PoDoFo::PdfMemDocument *parent =
    info[1].As<Napi::External<PoDoFo::PdfMemDocument>>().Data();
  _parent = parent;
  _page = page;
  pageNumber = info[2].As<Number>();
}

Napi::Value
Page::GetRotation(const CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), _page->GetRotation());
}
Napi::Value
Page::GetNumFields(const CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), _page->GetNumFields());
}
/**
 * @todo this method is not creating a valid instance of PdfField? Need to fix.
 * @param info
 * @return
 */
Napi::Value
Page::GetField(const CallbackInfo &info)
{
  if (info.Length() != 1 || !info[0].IsNumber())
  {
    throw Napi::Error::New(
      info.Env(), "SetRotation takes a single argument of type number.");
  }
  int n = info[0].As<Number>();
  if (n < 0 || n > _page->GetNumFields())
  {
    throw Napi::Error::New(info.Env(), "Out of Range");
  }
  PdfField field = _page->GetField(n);
  switch (field.GetType())
  {
    case ePdfField_TextField:break;
    case ePdfField_CheckBox: break;
    case ePdfField_ComboBox: break;
    case ePdfField_ListBox: break;
    case ePdfField_PushButton: break;
    case ePdfField_RadioButton: break;
    case ePdfField_Signature: break;
    case ePdfField_Unknown: break;
  }
  return Value();
}

Napi::Value
Page::GetFields(const CallbackInfo &info)
{
  auto fields = Napi::Array::New(info.Env());
  for (uint32_t i = 0; i < _page->GetNumFields(); ++i)
  {
    auto obj = Napi::Object::New(info.Env());
    auto field = _page->GetField(i);
    Page::GetFieldObject(obj, field);
    fields.Set(i, obj);
  }
  return fields;
}

void
Page::GetFieldObject(Napi::Object &obj, PoDoFo::PdfField &field)
{
  string name = field.GetFieldName().GetStringUtf8();
  string alternateName = field.GetAlternateName().GetStringUtf8();
  string mappingName = field.GetMappingName().GetStringUtf8();
  bool required = field.IsRequired();
  bool isWritable = field.IsReadOnly();
  obj.Set("name", name);
  obj.Set("alternateName", alternateName);
  obj.Set("mappingName", mappingName);
  obj.Set("required", required);
  obj.Set("readOnly", isWritable);
  switch (field.GetType())
  {
    case ePdfField_TextField:
    {
      PdfTextField textField(field);
      string fieldValue = textField.GetText().GetStringUtf8();
      long maxLen = textField.GetMaxLen();
      bool multiLine = textField.IsMultiLine();
      obj.Set("value", fieldValue);
      obj.Set("maxLength", &maxLen);
      obj.Set("isMultiLine", multiLine);
      obj.Set("type", "TextField");
      break;
    }
    case ePdfField_CheckBox:
    {
      PdfCheckBox checkBox(field);
      bool checkBoxValue = checkBox.IsChecked();
      string checkBoxCaption = checkBox.GetCaption().GetStringUtf8();
      obj.Set("value", checkBoxValue);
      obj.Set("caption", checkBoxCaption);
      obj.Set("type", "CheckBox");
      break;
    }
    case ePdfField_ComboBox:
    {
      PdfComboBox comboBox(field);
      string comboValue = comboBox.GetItem(comboBox.GetSelectedItem()).GetStringUtf8();
      obj.Set("type", "ComboBox");
      obj.Set("selected", comboValue);
      break;
    }
    case ePdfField_ListBox:
    {
      PdfListBox listBox(field);
      string listValue = listBox.GetItem(listBox.GetSelectedItem()).GetStringUtf8();
      obj.Set("type", "ListBox");
      obj.Set("value", listValue);
      break;
    }
    case ePdfField_PushButton:
    {
      PdfPushButton pushButton(field);
      string pushCaption = pushButton.GetCaption().GetStringUtf8();
      obj.Set("type", "PushButton");
      obj.Set("caption", pushCaption);
      break;
    }
    case ePdfField_RadioButton:
    {
      obj.Set("type", "RadioButton");
      break;
    }
    case ePdfField_Signature:
    {
      obj.Set("type", "Signature");
      break;
    }
    case ePdfField_Unknown:
    {
      obj.Set("type", "Unknown");
      break;
    }
  }
}

void
Page::SetRotation(const CallbackInfo &info)
{
  if (info.Length() != 1 || !info[0].IsNumber())
  {
    throw Napi::Error::New(
      info.Env(), "SetRotation takes a single argument of type number.");
  }
  int rotate = info[0].As<Number>();
  _page->SetRotation(rotate);
}
Napi::Value
Page::GetWidth(const CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), _page->GetPageSize().GetWidth());
}
Napi::Value
Page::GetHeight(const CallbackInfo &info)
{

  return Napi::Number::New(info.Env(), _page->GetPageSize().GetHeight());
}
Napi::Value
Page::GetLeft(const CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), _page->GetPageSize().GetLeft());
}
Napi::Value
Page::GetBottom(const CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), _page->GetPageSize().GetBottom());
}
void
Page::SetWidth(const CallbackInfo &info)
{
  if (info.Length() > 0)
  {
    if (!info[0].IsNumber())
    {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double width = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetWidth(width);
}
void
Page::SetHeight(const CallbackInfo &info)
{
  if (info.Length() > 0)
  {
    if (!info[0].IsNumber())
    {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double height = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetHeight(height);
}
void
Page::SetLeft(const CallbackInfo &info)
{
  if (info.Length() > 0)
  {
    if (!info[0].IsNumber())
    {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double left = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetLeft(left);
}
void
Page::SetBottom(const CallbackInfo &info)
{
  if (info.Length() > 0)
  {
    if (!info[0].IsNumber())
    {
      throw Napi::Error::New(
        info.Env(), "SetWidth requies a single argument of type number");
    }
  }
  double bottom = info[0].As<Number>();
  PoDoFo::PdfRect rect = _page->GetPageSize();
  rect.SetBottom(bottom);
}
void
Page::AddImg(const CallbackInfo &info)
{
  double x, y, width, height;
  string imgFile = info[0].As<String>().Utf8Value();
  x = info[1].As<Number>();
  y = info[2].As<Number>();
  width = info[3].As<Number>();
  height = info[4].As<Number>();

  PoDoFo::PdfPainter painter;
  PoDoFo::PdfImage image(_parent);
  image.LoadFromFile(imgFile.c_str());
  painter.SetPage(_page);
  //  painter.DrawImage(x, y, &image, width, height);
  painter.DrawImage(
    0.0, _page->GetPageSize().GetHeight() - image.GetHeight(), &image);
  painter.FinishPage();
}
void
Page::SetFields(const CallbackInfo &)
{

}
