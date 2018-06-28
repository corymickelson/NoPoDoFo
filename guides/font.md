# NoPoDoFo Font

Windows user, please ensure environment variable FONTCONFIG_FILE has been set, see setup guide for details.

The Font class is a wrapper around PoDoFo::PdfFont, PdfFont is represents a PDF Font dictionary.

### Getting a Font
To get an existing font use the `Document.getFont` method. This will iterate the document for 
Object's of type Font and return the font object with the matching font name or id provide to `getFont`.
You may also create a font with `createFont`. CreateFont uses fontconfig for creating the Font Object used by the document.
Once you have a handle on a Font object assign it to an instance of Painter.


```typescript
let doc = new npdf.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e) { /* handle error */ }
  const page = doc.getPage(0)
  const painter = new npdf.Painter(doc)
  painter.setPage(page)
  const docFonts = doc.listFonts()
  const font = doc.getFont(docFonts[0].id)
  font.size = 11
  painter.font = font
})
```