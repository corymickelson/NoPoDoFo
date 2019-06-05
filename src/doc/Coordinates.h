//
// Created by cmickelson on 6/3/19.
//

#ifndef COORDINATES_H
#define COORDINATES_H

#include <podofo/podofo.h>

namespace NoPoDoFo {

class Coordinates
{
  static std::unique_ptr<PoDoFo::PdfArray> ResolveMatrix(
    const PoDoFo::PdfRect& annotRect,
    const PoDoFo::PdfObject& apDict);
};

}

#endif // COORDINATES_H
