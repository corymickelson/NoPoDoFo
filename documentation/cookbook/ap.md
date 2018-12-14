# Appearance Stream (AP) Cookbook

## Text Field Appearance Stream

To set the appearance of an interactive form field; the Portable Document Format offers two means of defining how
an interactive value may be shown by a pdf reader. The first and easiest is to utilize the default appearance.
The default appearance for NoPoDoFo is 12pt black ariel. To set your own custom text field styles please follow the
below template.

```typescript
const doc = new nopodofo.Document()
doc.load('/path/to/doc.pdf', e => {
  if (e) {
    // handle error
  }
  // get the text field(s)
  const p1 = doc.getPage(0)
  const value = 'TESTING'
  const tfield = p1.getField<nopodofo.TextField>(0)
  const firaCode = doc.createFont({
    fontName: 'FiraCode',
    fileName: '/path/to/font.ttf',
    embed: true
  })
  /**
   * The function will create a new xobject and save the xobject to the fields dictionary as the value to the AP key
   */
  const applyAp = (field: nopodofo.TextField, value: string, font: nopodofo.Font) => {
    field.text = value
    const painter = new nopodofo.Painter(doc)
    const xRect = new nopodofo.Rect(0, 0, field.widgetAnnotation.rect.width, field.widgetAnnotation.rect.height)
    const xobj = doc.createXObject(xRect)
    painter.setPage(xobj)
    painter.setClipRect(xRect)
    painter.save()
    const black = new nopodofo.Color(1.0, 1.0, 1.0)
    painter.setColor(black)
    painter.restore()
    font.size = 12
    painter.font = font
    painter.beginText({x: 0, y: 5})
    painter.setStrokeWidth(20)
    painter.addText(value)
    painter.endText()
    painter.finishPage()
    field.readOnly = true
    const apDict = new nopodofo.Dictionary()
    apDict.addKey(NPDFName.N, xobj.reference)
    field.AP = apDict
    const daStr = `0 0 0 rg /${font.identifier} tf`
    field.DA = daStr
  }
})
```