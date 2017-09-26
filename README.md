# N_Pdf


NPdf, a native node module for reading, parsing, modifying, and creating pdf documents with [PoDoFo](http://podofo.sourceforge.net/index.html).

<span style="background-color:#FFFF00">Still in early development.</span>

# New Features!

 - Add an image
 - Rotate pages
 - Get / Set Form Fields
 - Delete Page
 - Merge Documents
 - Get Pdf Document info ( isLinearized, page count, has password, ... more to come)

### Installation

NPdf requires:
 - [PoDoFo](http://podofo.sourceforge.net/index.html)
 - [cmake-js](https://www.npmjs.com/package/cmake-js)
 - [Node.js](https://nodejs.org/) v8+ (NPdf is built with [N-Api](https://nodejs.org/dist/latest-v8.x/docs/api/n-api.html))
 - [Boost](http://www.boost.org/) FileSystem module only

For usage in AWS Lambda, PoDoFo prebuilt binaries are available [here](https://github.com/corymickelson/CommonPdf_PoDoFo)

To install Npdf run 
`npm i -S n_pdf`

### Usage

TypeDocs available [here](https://corymickelson.github.io/npdf/index.html)


Getting started
``` javascript
import {...} from 'npdf'
```
NPdf includes a definition file for developer happiness. If your editor does not detect the type file import `dist/index.d.ts` into your editor of choice.

For Webstorm go to settings > language & frameworks > javascript > libraries and add and select the definitions file.

### Document

NPdf wraps [PoDoFo::PdfMemDocument](http://podofo.sourceforge.net/doc/html/classPoDoFo_1_1PdfMemDocument.html#ae72a4141ed85e8abda6a368b220854fa) class as Document. 
Document is the core class for reading and modifying existing pdf documents. 

Loading a document
``` javascript
import {Document} from 'npdf'
const pdf:Document
try {
    pdf = new Document('/path/to/pdf') // if invalid password is thrown set password
} catch(e) {
    if(e.message === 'Password required to modify this document') {
        pdf.password = 'secret'
    }
}
```

At this point the document has been loaded into memory and we can now make modifications to the document and write back to disk.

### Page

To start let's get a page, and rotate it 90 degrees and then save back to disk. 

Note: The page definition is defined as an interface, not a concrete class.

``` javascript
import {Document, IPage} from 'npdf'
const pdf = new Document('/path/to/pdf')
/** Pages are zero indexed */
const pageCount = doc.getPageCount()
for(let i = 0; i < pageCount; i++) {
    let page:IPage = doc.getPage(i)
    page.setRotation(90) // value must be increment of 90 (valid:0,90,180, 270)
}
doc.write('/destination')
```

And that's all, we've loaded a pdf, iterated through it's pages setting the rotate value to 90 and then written the modified file back to disk.

Check a page for fields
``` javascript
import {Document, IPage} from 'npdf'
const pdf = new Document('/path/to/pdf')
const pageCount = doc.getPageCount()
for(let i = 0; i < pageCount; i++) {
    let page:IPage = doc.getPage(i)
    let fieldCount = page.getNumFields()
    if(fieldCount > 0) {
        let fieldsSummary:Array<IFieldInfo> = page.getFields()
    }
}
```



