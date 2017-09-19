//
// Created by red on 9/10/17.
//

#ifndef NPDF_PDFFIELD_H
#define NPDF_PDFFIELD_H

//#include "TextField.h"
#include "ValidateArguments.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>

using namespace Napi;
using namespace std;
using namespace PoDoFo;

class Field : public Napi::ObjectWrap<Field>
{
public:
  Field(const CallbackInfo& info);
  ~Field() { delete _page; }
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(
      env,
      "Field",
      { InstanceMethod("getType", &Field::GetType),
        InstanceMethod("getFieldName", &Field::GetFieldName),
        InstanceMethod("getAlternateName", &Field::GetAlternateName),
        InstanceMethod("getMappingName", &Field::GetMappingName),
        InstanceMethod("setAlternateName", &Field::SetAlternateName),
        InstanceMethod("setMappingName", &Field::SetMappingName),
        InstanceMethod("setRequired", &Field::SetRequired),
        InstanceMethod("setFieldIndex", &Field::SetFieldIndex),
        InstanceMethod("getFieldIndex", &Field::GetFieldIndex),
        //        InstanceMethod("textField", &Field::TextField),
        InstanceMethod("isRequired", &Field::IsRequired) });
    target.Set("Field", ctor);
  }

  Napi::Value GetType(const CallbackInfo&);
  Napi::Value GetFieldName(const CallbackInfo&);
  Napi::Value GetAlternateName(const CallbackInfo&);
  Napi::Value GetMappingName(const CallbackInfo&);
  void SetAlternateName(const CallbackInfo&);
  void SetMappingName(const CallbackInfo&);
  void SetRequired(const CallbackInfo&);
  Napi::Value IsRequired(const CallbackInfo&);
  void SetFieldIndex(const CallbackInfo&);
  Napi::Value GetFieldIndex(const CallbackInfo&);
  //  Napi::Value TextField(const CallbackInfo&);
  PdfField GetField() { return _page->GetField(fieldIndex); }

private:
  int fieldIndex;
  PdfPage* _page;
};

#endif // NPDF_PDFFIELD_H
