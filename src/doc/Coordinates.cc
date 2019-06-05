//
// Created by cmickelson on 6/3/19.
//

#include "Coordinates.h"
#include "../base/Names.h"
#include "Rect.h"
using namespace PoDoFo;
using std::vector;

namespace NoPoDoFo {
/**
 * @brief
 * @param annotRect
 * @param apDict - this dictionary is the AP N stream
 * @return
 */
std::unique_ptr<PdfArray>
Coordinates::ResolveMatrix(const PoDoFo::PdfRect& annotRect, const PdfObject& apDict)
{
  if (!apDict.IsDictionary()) {
    // handle error
  }
  PdfRect* apRect;
  PdfArray* apMatrix;
  auto outArray = std::unique_ptr<PdfArray>();
  if (apDict.GetDictionary().HasKey(Name::RECT)) {
    auto rect = apDict.MustGetIndirectKey(Name::RECT);
    apRect = new PdfRect(rect->GetArray());
  } else if (apDict.GetDictionary().HasKey(Name::BBOX)) {
    auto rect = apDict.MustGetIndirectKey(Name::BBOX);
    apRect = new PdfRect(rect->GetArray());
  } else {
    // handle error
    throw std::exception();
  }

  if (apDict.GetDictionary().HasKey(Name::MATRIX)) {
    apMatrix = &apDict.MustGetIndirectKey(Name::MATRIX)->GetArray();
  } else {
    // handle error
    throw std::exception();
  }
  if (Rect::IsEmpty(*apRect)) {
    outArray->push_back(PdfVariant(1.0));
    outArray->push_back(PdfVariant(0.0));
    outArray->push_back(PdfVariant(0.0));
    outArray->push_back(PdfVariant(1.0));
    outArray->push_back(PdfVariant(0.0));
    outArray->push_back(PdfVariant(0.0));
    return outArray;
  }
  vector<std::pair<double, double>> points{
    std::pair<double, double>(apRect->GetLeft(),
                              apRect->GetBottom() + apRect->GetHeight()),
    std::pair<double, double>(apRect->GetLeft(), apRect->GetBottom()),
    std::pair<double, double>(apRect->GetLeft() + apRect->GetWidth(),
                              apRect->GetBottom() + apRect->GetHeight()),
    std::pair<double, double>(apRect->GetLeft() + apRect->GetWidth(),
                              apRect->GetBottom())
  };
  for (auto& point : points) {
    const auto p = point;
    point = std::pair<double, double>(
      (*apMatrix)[0].GetReal() * p.first + (*apMatrix)[2].GetReal() * p.second +
        (*apMatrix)[4].GetReal(),
      (*apMatrix)[1].GetReal() * p.first + (*apMatrix)[3].GetReal() * p.second +
        (*apMatrix)[5].GetReal());
  }
  double right = points[0].first;
  double left = points[0].first;
  double top = points[0].second;
  double bottom = points[0].second;
  for (size_t i = 0; i < 4; i++) {
    right = std::max(right, points[i].first);
    left = std::min(left, points[i].first);
    top = std::max(top, points[i].second);
    bottom = std::min(bottom, points[i].second);
  }
  PdfRect tRect(left, bottom, right - left, top - bottom);

  outArray->push_back(PdfVariant(annotRect.GetWidth() / tRect.GetWidth()));
  outArray->push_back(PdfVariant(0.0));
  outArray->push_back(PdfVariant(0.0));
  outArray->push_back(PdfVariant(annotRect.GetHeight() / tRect.GetHeight()));
  outArray->push_back(PdfVariant(annotRect.GetLeft() - tRect.GetLeft() * (*outArray)[0].GetReal()));
  outArray->push_back(PdfVariant(annotRect.GetBottom() - tRect.GetBottom() * (*outArray)[3].GetReal()));
  return outArray;
}
}
