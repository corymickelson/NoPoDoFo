//
// Created by developer on 1/30/19.
//

#ifndef NOPODOFO_FLATTENFIELDS_H
#define NOPODOFO_FLATTENFIELDS_H

#include <podofo/podofo.h>

namespace NoPoDoFo {
typedef struct FlattenFieldsResponse {
  std::string err;
  int fieldsAffected;
  time_t start;
  time_t end;
}FlattenFieldsResponse;
class FlattenFields
{
public:
  explicit FlattenFields(PoDoFo::PdfPage&);
  FlattenFieldsResponse Flatten() const;
protected:
  PoDoFo::PdfRect MinMaxMediaBox(
    std::vector<std::tuple<int, PoDoFo::PdfAnnotation>> fields,
    const PoDoFo::PdfRect& mb) const;
  bool RectEmpty(const PoDoFo::PdfRect&) const;
private:
  PoDoFo::PdfPage& page;
};

}
#endif // NOPODOFO_FLATTENFIELDS_H
