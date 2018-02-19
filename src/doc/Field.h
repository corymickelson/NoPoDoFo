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
  explicit Field(const CallbackInfo& info);
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
  std::shared_ptr<PoDoFo::PdfField> GetField() { return field; }

private:
  std::shared_ptr<PoDoFo::PdfField> field;
};
}
#endif // NPDF_PDFFIELD_H
