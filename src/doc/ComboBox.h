//
// Created by red on 20/12/17.
//

#ifndef NPDF_COMBOBOX_H
#define NPDF_COMBOBOX_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {
class ComboBox : public Napi::ObjectWrap<ComboBox> {
public:
    explicit ComboBox(const Napi::CallbackInfo &);
  static Napi::FunctionReference constructor;
  static void Initialize(Napi::Env&, Napi::Object&);

private:
    std::unique_ptr<PoDoFo::PdfComboBox> self;
};
}



#endif //NPDF_COMBOBOX_H
