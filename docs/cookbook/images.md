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
            objType = objDict.hasKey('Type') ? objDict.getKeyType('Type') : null,
            objSubType = objDict.hasKey('SubType') ? objDict.getKeyType('SubType') : null

        if ((objType && objType === 'Name') ||
            (objSubType && objSubType === 'Name')) {

            if ((objType && objDict.getKey<String>('Type') === 'XObject') || (objSubType && objDict.getKey<String>('SubType') === 'Image')) {
                if (objDict.hasKey('Filter')) {
                    if (objDict.getKeyType('Filter') === 'Array') {
                        const imgObjArr = objDict.getKey<npdf.Array>('Filter')
                        if (imgObjArr.length === 1) {
                            if ((imgObjArr.at(0) as npdf.Object).type === 'Name') {
                                if ((imgObjArr.at(0) as npdf.Object).getName() === 'DCTDecode') {
                                    extractImg(o, true)
                                    return
                                }
                            }
                        }
                    } else if (objDict.getKeyType('Filter') === 'Name' && objDict.getKey<String>('Filter') === 'DCTDecode') {
                        extractImg(o, true)
                        return
                    }
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
const doc = new npdf.Document()
doc.load('/path/to/doc', e => {
    if (e instanceof Error) t.fail()
    const page = doc.getPage(0)
    let resource = page.resources.getDictionary().getKey<npdf.Dictionary>('XObject')
    let xKey = resource.getKeys()
    let xob = resource.getKey<npdf.Dictionary>('XOb4')
    if(xob.getKey<String>(NPDFName.SUBTYPE) === 'Image') {
        if(xob.hasKey('Filter') && xob.getKey<String>('Filter') === 'DCTDecode') {
            extractImg(xob.obj, true)
        } else {
            extractImg(xob.obj, true)
        }
    }
})
function extractImg(obj: npdf.Object, jpg: Boolean) {
    // img data is accessable via npdf.Object.stream
}
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