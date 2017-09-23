#include "NObject.h"

NObject::NObject(const Napi::CallbackInfo& info)
  : ObjectWrap<NObject>(info)
{
  if (info.Length() == 1) {
    if (info[0].IsString()) {
      string str = info[0].As<String>().Utf8Value();
      char nameDelimiter = str[0];
      if (nameDelimiter == '/') {
        PdfName name(str);
        _obj = new PdfObject(name);
      } else {
        PdfString pdString(str);
        _obj = new PdfObject(pdString);
      }
    }
    if (info[0].IsNumber()) {
      double intValue = info[0].As<Number>();
      _obj = new PdfObject(intValue);
    }
    if (info[0].IsObject()) {
      Napi::Object obj = info[0].As<Object>();
    } else {
      _obj = *info[0].As<Napi::External<PdfObject*>>().Data();
    }
  } else
    _obj = new PdfObject();
}
