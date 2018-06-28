# NoPoDoFo Painter

This class wraps PoDoFo::PdfPainter which provides an easy to use painter object which allows you to draw on a PDF page object.
During all drawing operations, you are still able to access the stream of the object you are drawing on directly see [NoPoDoFo.Obj.stream](https://github.com/corymickelson/NoPoDoFo).
All functions that take coordinates expect these to be in PDF User Units. Keep in mind that PDF has its coordinate system origin at the bottom left corner.

For a complete list of Painter api's see the Api Documentation.

The Painter operates on a Page, you must set the Page the Painter will be painting on before calling any other operations.
After all painting operations are complete call Painter `.finishPage()`.
```typescript
let doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e) {/*handle error*/}
  let page = doc.getPage(0)
  let painter = new NoPoDoFo.Painter(doc)
  painter.setPage(page)
  // do something with the painter
  painter.finishPage()
})
```

### Add Text
To add text to a page use Painter `drawText`, `drawTextAligned`, or `drawMultiLineText`. Operations `drawTextAligned` and `drawMultiLineText` 
require [NPDFAlignment](https://corymickelson.github.io/NoPoDoFo/enums/_painter_.npdfalignment.html) and [NPDFVerticalAlignment](https://corymickelson.github.io/NoPoDoFo/enums/_painter_.npdfverticalalignment.html) (for multiline text only) arguments. Adding text also requires a font has been set, 
see [NoPoDoFo.Font](https://github.com/corymickelson/NoPoDoFo/blob/master/guides/parsers.md).

```typescript
let doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e) {/*handle error*/}
  const page = doc.getPage(0)
  const painter = new NoPoDoFo.Painter(doc)
  const font = doc.getFont('Courier')
  const value = 'This is a test'
  painter.setPage(page)
  painter.font = font
  let x = 0, y = 0 // set to bottom left corner of page
  painter.drawText({x, y}, value)
  // do something with the painter
  painter.finishPage()
})
```

### Add Image
To add an image to a page use the Painter `drawImage` method. You will need to first load the image with `NoPoDoFo.Image`.
```typescript
let doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e) {/*handle error*/}
  const page = doc.getPage(0)
  const painter = new NoPoDoFo.Painter(doc)
  const img = new NoPoDoFo.Image(doc, 'path/to/img.png')
  painter.setPage(page)
  let bottom = 0, left = 0
  painter.drawImage(img, bottom, left)
  painter.finishPage()
})
```