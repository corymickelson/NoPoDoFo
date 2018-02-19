//
// Created by oak on 2/19/18.
//

#ifndef NPDF_LISTBOX_H
#define NPDF_LISTBOX_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {

class ListBox: public Napi::ObjectWrap<ListBox> {
public:
  explicit ListBox(const Napi::CallbackInfo &);
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env&, Napi::Object&);

private:
  std::unique_ptr<PoDoFo::PdfListBox> self;
};

}

#endif //NPDF_LISTBOX_H
