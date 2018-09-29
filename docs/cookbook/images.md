# NoPoDoFo Image Creation and Modification Cookbook

## Get all document images

```typescript
const doc = new npdf.Document()
doc.load('/path/to/document.pdf', (err, data) => {
  if(e) {/* handle error */}
  // iterate the document body, filter type XObject with subtype Image
  // iterating the body can be expensive in terms of javascript / c++ copy overhead
  for(let i = 0; i < doc.body.length; i++) {
    const o = doc.body[i]
    if (o.type === 'Dictionary') {
      let objDict = o.getDictionary(),
          objType = objDict.hasKey('Type') ? objDict.getKey('Type') : null,
          objSubType = objDict.hasKey('SubType') ? objDict.getKey('SubType') : null

      if ((objType && objType.type === 'Name') ||
          (objSubType && objSubType.type === 'Name')) {

          if ((objType && objType.getName() === 'XObject') || (objSubType && objSubType.getName() === 'Image')) {
              let imgObj = objDict.hasKey('Filter') ? objDict.getKey('Filter') : null
              if (imgObj && imgObj.type === 'Array') {
                  const imgObjArr = imgObj.getArray()
                  if (imgObjArr.length === 1) {
                      if ((imgObjArr.at(0) as npdf.Object).type === 'Name') {
                          if ((imgObjArr.at(0) as npdf.Object).getName() === 'DCTDecode') {
                              // image found, extractImg writes the image to disk
                              // the image is available as a buffer on the stream property of the Object
                              // see Object.stream
                              return
                          }
                      }
                  }
              }
              else if (imgObj && imgObj.type === 'Name' && imgObj.getName() === 'DCTDecode') {
                  extractImg(o, true)
                  return
              }
          }
      }
    }
  }
})
function extractImg(obj: npdf.Object, jpg: Boolean) {
    let ext = jpg ? '.jpg' : '.ppm'
    writeFile(`/tmp/${v4()}.${ext}`, obj.stream, err => {
        if (err instanceof Error) { /* handle error */}
    })
}
```

## Get an image from a specific page

Getting images from a page is similar to the above with the exception that instead of iterating the document's body
we will iterate a page's resources dictionary. This method will only work if the image has been set in the page's resources.

```typescript
```

## Set an image

```typescript
import {nopodofo as npdf} from 'nopodofo'
const doc = new npdf.StreamDocument()
const page = doc.createPage(new npdf.Rect(0, 0, 612, 792))
const painter = new npdf.Painter(doc)
const img = new npdf.Image(doc, join(__dirname, '../../spec/test-documents/test.jpg'))
painter.setPage(page)
painter.drawImage(img, 0, page.height - img.height)
painter.finishPage()
```