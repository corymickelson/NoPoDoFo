//
// Created by red on 9/10/17.
//

#ifndef NPDF_PDFFIELD_H
#define NPDF_PDFFIELD_H

#include "../ValidateArguments.h"
#include "Page.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>

namespace NoPoDoFo {
class Field : public Napi::ObjectWrap<Field>
{
public:
  Field(const CallbackInfo& info);
  ~Field();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);

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
  PoDoFo::PdfField GetField() { return _page->GetPage()->GetField(fieldIndex); }

private:
  int fieldIndex;
  Page* _page;
};
}
#endif // NPDF_PDFFIELD_H
