# NoPoDoFo Page

Page is one page of the pdf document. It is possible to draw on the page using [NoPoDoFo.painter](https://github.com/corymickelson/NoPoDoFo/blob/master/guides/painter.md). Every document needs at least one page.
For a complete list of page api's see the Api Documentation.

A Page is owned by the document (NoPoDoFo.Document or NoPoDoFo.StreamDocument), when the document goes out of scope, so
do any pages in that document.

### Getting a page
Use the document `.getPage()` method to get a specific page. Pages are 0-based indexed.
```typescript
const doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e) { /* handle error */ }
  let page = doc.getPage(0) // get the first page
  // do something with the page
})
```

### Removing a page
Removing pages can only be done with the `NoPoDoFo.Document` class. Removal does not work with `NoPoDoFo.StreamDocument`.
Use the document `.splicePages()` method to remove one or more pages.
```typescript
const doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e) { /* handle error */ }
  doc.splicePages(0, 1) // removes the first page from the document
})
```

### Creating page(s)
To create a new blank page, use the document `.createPage()` method, to create multiple pages use `.createPages()`.
Creating a page requires a `NoPoDoFo.Rect` to defined the dimensions(MediaBox) of the page. The page is automatically 
added to the documents page tree(appended to the document, will be the last page).
```typescript
const doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e) { /* handle error */ }
  let blank = doc.createPage(new NoPoDoFo.Rect(/*left, bottom, width, height*/, 0, 0, 612, 792)) // this is the equivalent dimensions of the standard 8.5 x 11 US letter paper size
})
```
