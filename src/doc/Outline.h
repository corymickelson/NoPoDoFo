//
// Created by cory on 4/29/18.
//

#ifndef NPDF_OUTLINE_H
#define NPDF_OUTLINE_H

#include <podofo/podofo.h
#include "BaseDocument.h"
#include <napi.h>

namespace NoPoDoFo {

/**
 * @note PdfOutlineItem Wrapper, PdfOutline::CreateRoot exposed through BaseDocument.
 */
class Outline : public Napi::ObjectWrap<Outline>
{
public:
  explicit Outline(const Napi::CallbackInfo& callbackInfo);
  ~Outline();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value CreateChild(const Napi::CallbackInfo&);
  Napi::Value CreateNext(const Napi::CallbackInfo&);
  Napi::Value InsertChild(const Napi::CallbackInfo&);
  Napi::Value Pref(const Napi::CallbackInfo&);
  Napi::Value Next(const Napi::CallbackInfo&);
  Napi::Value First(const Napi::CallbackInfo&);
  Napi::Value Last(const Napi::CallbackInfo&);
  Napi::Value GetParent(const Napi::CallbackInfo&);
  Napi::Value Erase(const Napi::CallbackInfo&);
  Napi::Value GetDestination(const Napi::CallbackInfo&);
  void SetDestination(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetAction(const Napi::CallbackInfo&);
  void SetAction(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetTitle(const Napi::CallbackInfo&);
  void SetTitle(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetTextFormat(const Napi::CallbackInfo&);
  void SetTextFormat(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetTextColor(const Napi::CallbackInfo&);
  void SetTextColor(const Napi::CallbackInfo&, const Napi::Value&);
  PoDoFo::PdfOutlines* GetOutline() { return outlines; }

private:
  PoDoFo::PdfOutlines* outlines;
};
}
#endif // NPDF_OUTLINE_H
