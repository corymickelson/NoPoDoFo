//
// Created by red on 9/10/17.
//

#ifndef NPDF_LISTFIELD_H
#define NPDF_LISTFIELD_H

#include "../ValidateArguments.h"
#include "Field.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <string>
namespace NoPoDoFo {
class ListField : public Napi::ObjectWrap<ListField>
{
public:
  explicit ListField(const CallbackInfo& info);
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  void InsertItem(const Napi::CallbackInfo&);
  void RemoveItem(const Napi::CallbackInfo&);
  Napi::Value GetItem(const Napi::CallbackInfo&);
  Napi::Value GetItemCount(const Napi::CallbackInfo&);
  void SetSelectedItem(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetSelectedItem(const Napi::CallbackInfo&);

private:
  std::unique_ptr<PoDoFo::PdfListField> list;
};
}
#endif
