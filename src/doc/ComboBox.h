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
    ~ComboBox();

private:
    PoDoFo::PdfComboBox* self;
    PoDoFo::PdfField* field;
};
}



#endif //NPDF_COMBOBOX_H
