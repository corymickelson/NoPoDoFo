# NoPoDoFo Parsers


### ContentsTokenizer

The NoPoDoFo ContentsTokenizer class is used for extracting text from a page. 
```typescript
const doc:Document = new Document()
doc.load('path/to/pdf', e => {
    if(e instanceof Error) {
        //handle error
    }
    // get that page for extracting contents
    const page = doc.getPage(1)
    // instantiate the ContentsTokenizer parser
    const contents = new ContentsTokenizer(page)
    // extract text to array
    const pageText = contents.readAllContent()
    // do something with the contents
})
```
