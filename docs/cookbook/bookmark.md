# Bookmarks

A bookmark is a combination of an [Outline](../outline.md) and one of either an [Action](../action.md) or [Destination](../destination.md). A Bookmark in a PDF document
is a link list type data structure, adding links to pages, external sources or scripts. 

## Creating a new Bookmark

```typescript
  let doc = new npdf.Document()
  doc.load('/path/to/file', e => {
    if (e) {/*handle error*/}
    else {
      let outline: IOutline = doc.getOutlines(true, 'Test') as any
      let dest = new npdf.Destination(doc.getPage(0), NPDFDestinationFit.Fit)
      let first = outline.createChild('First', dest);
      let sec = new npdf.Destination(doc.getPage(1), NPDFDestinationFit.Fit)
      first.createNext('Second', sec)
      doc.write((err, data) => {
        if (err) {/*handle error*/}
        else {
          let inn = new npdf.Document()
          inn.load(data, e => {
            let outlines: IOutline = inn.getOutlines() as any
            // walk bookmark chain with Outline next, fist, last, prev.
          })
        }
      })
    }
  })
```