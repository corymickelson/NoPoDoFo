#ifndef NPDF_CONTENTSTOKENIZER_H
#define NPDF_CONTENTSTOKENIZER_H

#include "../doc/Document.h"
#include "../doc/Page.h"
#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class ContentsTokenizer : public Napi::ObjectWrap<ContentsTokenizer>
{
public:
  explicit ContentsTokenizer(const Napi::CallbackInfo&);
  ~ContentsTokenizer();
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  Napi::Value ReadAll(const Napi::CallbackInfo&);

private:
  PoDoFo::PdfContentsTokenizer* self;
  Page* page;
  Document* doc;
  void AddText(PoDoFo::PdfFont*, const PoDoFo::PdfString&, Napi::Array& out);
};
}

#endif
