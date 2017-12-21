//
// Created by red on 20/12/17.
//

#include "ComboBox.h"
#include "../ValidateArguments.h"
#include "Field.h"

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

ComboBox::ComboBox(const Napi::CallbackInfo& info)
  : ObjectWrap(info)
{
    AssertFunctionArgs(info, 1, {napi_object});
    Field* nField = Field::Unwrap(info[0].As<Object>());
    PdfField root = nField->GetField();
    field = &root;
}
}
