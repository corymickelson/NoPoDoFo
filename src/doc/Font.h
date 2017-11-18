//
// Created by red on 11/11/17.
//

#ifndef NPDF_FONT_H
#define NPDF_FONT_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class Font : public Napi::ObjectWrap<Font>
{
public:
  explicit Font(const Napi::CallbackInfo& callbackInfo);
  ~Font()
  {
    PoDoFo::podofo_free(font);
    font = nullptr;
  }
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value GetFontSize(const Napi::CallbackInfo&);
  void SetFontSize(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetFontScale(const Napi::CallbackInfo&);
  void SetFontScale(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetFontCharSpace(const Napi::CallbackInfo&);
  void SetFontCharSpace(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetWordSpace(const Napi::CallbackInfo&);
  void SetWordSpace(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsUnderlined(const Napi::CallbackInfo&);
  void SetUnderline(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value IsStrikeOut(const Napi::CallbackInfo&);
  void SetStrikeOut(const Napi::CallbackInfo&, const Napi::Value&);

  Napi::Value GetIdentifier(const Napi::CallbackInfo&);
  Napi::Value GetEncoding(const Napi::CallbackInfo&);
  Napi::Value GetFontMetric(const Napi::CallbackInfo&);
  Napi::Value IsBold(const Napi::CallbackInfo&);
  Napi::Value IsItalic(const Napi::CallbackInfo&);
  Napi::Value StringWidth(const Napi::CallbackInfo&);

  void WriteToStream(const Napi::CallbackInfo&);
  void EmbedFont(const Napi::CallbackInfo&);

  PoDoFo::PdfFont* GetPoDoFoFont() { return font; }

private:
  PoDoFo::PdfFont* font;
};
}
#endif // NPDF_FONT_H
