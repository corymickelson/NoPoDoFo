//
// Created by developer on 1/30/19.
//

#include "FlattenFields.h"
#include "../base/Names.h"
#include "../doc/Page.h"

using namespace PoDoFo;
using std::get;
using std::tuple;
using std::vector;

namespace NoPoDoFo {

FlattenFields::FlattenFields(PoDoFo::PdfPage& pdfPage)
  : page(pdfPage)
{}
PoDoFo::PdfRect
FlattenFields::MinMaxMediaBox(vector<tuple<int, PdfAnnotation>> fields,
                              const PoDoFo::PdfRect& mb) const
{
  PdfRect minMax(0.0, 0.0, 0.0, 0.0);
  for (auto& field : fields) {
    auto annot = get<1>(field);
    minMax.SetLeft(std::min(minMax.GetLeft(), annot.GetRect().GetLeft()));
    minMax.SetBottom(std::min(minMax.GetBottom(), annot.GetRect().GetBottom()));
    minMax.SetWidth(std::max(minMax.GetWidth(), annot.GetRect().GetWidth()));
    minMax.SetHeight(std::max(minMax.GetHeight(), annot.GetRect().GetHeight()));
  }
  minMax.SetLeft(std::min(minMax.GetLeft(), mb.GetLeft()));
  minMax.SetBottom(std::min(minMax.GetBottom(), mb.GetBottom()));
  minMax.SetWidth(std::max(minMax.GetWidth(), mb.GetWidth()));
  minMax.SetHeight(std::max(minMax.GetHeight(), mb.GetHeight()));
  return minMax;
}
bool
FlattenFields::RectEmpty(const PdfRect& rect) const
{
  return rect.GetHeight() == 0.0 && rect.GetWidth() == 0.0 &&
         rect.GetBottom() == 0.0 && rect.GetLeft() == 0.0;
}
FlattenFieldsResponse
FlattenFields::Flatten() const
{
  FlattenFieldsResponse resp;
  PdfVariant mBox;
  PdfVariant cBox;
  PdfPainter painter;
  painter.SetPage(&page);
  int n = 0;
  auto annots = page.GetObject()->GetIndirectKey(Name::ANNOTS);
  if (!annots || !annots->IsArray()) {
    resp.err = "Failed to get Page Annotations Array";
    return resp;
  }
  vector<tuple<int, PdfAnnotation>> fields;
  auto it = annots->GetArray().begin();
  while (it != annots->GetArray().end()) {
    auto o = page.GetObject()->GetOwner()->GetObject(it->GetReference());
    PdfAnnotation a(o, &page);
    if (a.GetType() == ePdfAnnotation_Widget) {
      if (a.HasAppearanceStream()) {
        fields.emplace_back(n, a);
        --n; // keep n in sync with index as fields are deleted
      }
      ++n;
    }
    ++it;
  }
  // Set Page Box Sizes
  PdfRect originalMediaBox = page.GetMediaBox();
  if (RectEmpty(originalMediaBox)) {
    originalMediaBox = PdfRect(0.0, 0.0, 612.0, 792.0);
  }
  PdfRect originalCropBox = page.GetCropBox();
  if (RectEmpty(originalCropBox)) {
    originalCropBox = originalMediaBox;
  }
  if (page.GetObject()->GetDictionary().HasKey(Name::MEDIA_BOX)) {
    page.GetObject()->GetDictionary().RemoveKey(Name::MEDIA_BOX);
  }
  if (page.GetObject()->GetDictionary().HasKey(Name::CROP_BOX)) {
    page.GetObject()->GetDictionary().RemoveKey(Name::CROP_BOX);
  }
  originalMediaBox.ToVariant(mBox);
  originalCropBox.ToVariant(cBox);
  page.GetObject()->GetDictionary().AddKey(PdfName(Name::MEDIA_BOX), mBox);
  page.GetObject()->GetDictionary().AddKey(PdfName(Name::CROP_BOX), cBox);

  PdfRect mergeMediaBox = MinMaxMediaBox(fields, originalMediaBox);

  PdfXObject xObj(mergeMediaBox,
                  page.GetObject()->GetOwner()->GetParentDocument());
  PdfDictionary pageXObj;
  if (!page.GetObject()->GetDictionary().HasKey(Name::XOBJECT)) {
    page.GetObject()->GetDictionary().AddKey(PdfName(Name::XOBJECT),
                                             PdfDictionary());
  }
  pageXObj =
    page.GetObject()->MustGetIndirectKey(Name::XOBJECT)->GetDictionary();

  while (!fields.empty()) { // erase ALL fields added to fields array
    PdfObject* appearance = get<1>(fields.at(0))
                              .GetObject()
                              ->MustGetIndirectKey(Name::AP)
                              ->MustGetIndirectKey(Name::N);
    PdfXObject xApp(appearance);
    painter.DrawXObject(get<1>(fields.at(0)).GetRect().GetLeft(),
                        get<1>(fields.at(0)).GetRect().GetBottom(),
                        &xApp);
    PdfObject* fieldObj = page.GetField(get<0>(fields.at(0))).GetFieldObject();
    Page::DeleteFormField(page,
                          *fieldObj,
                          *page.GetObject()
                             ->GetOwner()
                             ->GetParentDocument()
                             ->GetAcroForm(false)
                             ->GetObject()
                             ->MustGetIndirectKey(Name::FIELDS));
    page.DeleteAnnotation(fieldObj->Reference());
    fields.erase(fields.begin());
  }
  painter.FinishPage();
}
}
