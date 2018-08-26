# API Documentation for Document

* [Properties](#properties)
    * [form](#form)
    * [body](#body)
    * [version](#version)
    * [pageMode](#pageMode)
    * [pageLayout](#pageLayout)
    * [printingScale](#printingScale)
    * [language](#language)
    * [info](#info)
    * [password](#password)
    * [encrypt](#encrypt)
    * [trailer](#trailer)
    * [catalog](#catalog)

* [Methods](#methods)
    * [getPageCount](#getPageCount)
    * [getPage](#getPage)
    * [hideToolbar](#hideToolbar)
    * [hideMenubar](#hideMenubar)
    * [hideWindowUI](#hideWindowUI)
    * [fitWindow](#fitWindow)
    * [centerWindow](#centerWindow)
    * [displayDocTitle](#displayDocTitle)
    * [useFullScreen](#useFullScreen)
    * [attachFile](#attachFile)
    * [insertExistingPage](#insertExistingPage)
    * [insertPage](#insertPage)
    * [append](#append)
    * [isLinearized](#isLinearized)
    * [getWriteMode](#getWriteMode)
    * [isAllowed](#isAllowed)
    * [createFont](#createFont)
    * [getOutlines](#getOutlines)
    * [getObject](#getObject)
    * [getNames](#getNames)
    * [createXObject](#createXObject)
    * [createPage](#createPage)
    * [createPages](#createPages)
    * [getAttachment](#getAttachment)
    * [load](#load)
    * [splicePages](#splicePages)
    * [insertPages](#insertPages)
    * [write](#write)
    * [getFont](#getFont)
    * [hasSignatures](#hasSignatures)
    * [getSignatures](#getSignatures)
    * [gc](#gc)

## NoPoDoFo Document

The NoPoDoFo Document class is a wrapper around PoDoFo::PdfMemDocument. This is the core class for reading and manipulating pdf documents.
This class was designed to allow easy access to the pdf object structure and should be used whenever you want to change the structure of
the pdf document. If you are only <i>creating</i> a pdf file use the [StreamDocument](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/stream_document.md) class.

Document utilizes PoDoFo's load on demand policy. When a PDF is loaded through `Document`.[load](#load), document objects are read into memory
at their first point of access. If an object is never used, it will never be loaded into memory, allowing for the ingestion and modification
of large documents with a small memory footprint.

```typescript
class Document extends Base {
    constructor()

    encrypt: Encrypt
    readonly trailer: Object
    readonly catalog: Object

    setPassword(pwd:string): void
    load(file: string | Buffer,
        opts: {
        forUpdate?: boolean,
        password?: string
        },
        cb: Callback<void>): void
    load(file: string | Buffer, cb: Callback<void>): void
    splicePages(startIndex: number, count: number): void
    insertPages(fromDoc: Document, startIndex: number, count: number): number
    write(destination: Callback<Buffer> | string, cb?: Callback<string>): void
    getFont(name: string): Font
    listFonts(): { id: string, name: string }[]
    gc(file: string, pwd: string, output: string, cb: Callback<string | Buffer>): void
    hasSignatures(): boolean
    getSignatures(): SignatureField[]
}
```
## Properties

### catalog
Readonly property returning an [Object](./object.md). The catalog is the root of the documents object tree; every object in a document
is reachable from the catalog.

### trailer
Readonly property returning an [Object](./object.md). The trailer resides at the end of the document, containing document level information necessary
to parse the document. 

### encrypt
If the document is encrypted an [Encrypt](./encrypt.md) object is returned otherwise null.

### info
Readonly NPDFInfo.

### language
Set the Document viewer preference. Set the language of the Document.

### printingScale
The page scaling option displayed in the print dialog, valid values are: 'None', and 'AppDefault'

### pageLayout
Specifies the page layout as NPDFPageLayout when the document is opened.

### pageMode
Specifies the page mode as NPDFPageMode when the document is opened.

### version
Pdf specification version as NPDFVersion.

### body
The Document body (readonly) is an array of all [Objects](./object.md) accessible from the Document catalog(#catalog).

### form
Form is a readonly property that will either return [Form](./form.md) or null if the Document does not have an AcroForm dictionary.

## Methods

### setPassword
Set the password for the document.

### getPageCount

```typescript
getPageCount(): number
```

Get the number of pages in the Document. A Document must have at least one page.

### getPage

```typescript
getPage(n: number): IPage
```
Get the [Page](./page.md). If n is < 0 or greater than page count a RangeError will be thrown.

### hideToolbar

```typescript
hideToolbar(): void
```
Toggle view option hideToolbar.

### hideMenubar

```typescript
hideMenubar(): void
```
Toggle view option hideMenubar.

### hideWindowUI

```typescript
hideWindowUI(): void
```
Toggle view option hideWindowUI.

### fitWindow

```typescript
fitWindow(): void
```
Toggle view option fitWindow.

### centerWindow

```typescript
centerWindow(): void
```
Toggle view option centerWindow.

### displayDocTitle

```typescript
displayDocTitle(): void
```

Toggle view option displayDocTitle.

### useFullScreen

```typescript
useFullScreen(): void
```

Toggle view option useFullScreen.

### attachFile

```typescript
attachFile(file: string): void
```

Attach a file, as a [FileSpec](./filespec.md) to the Document. The file will be embedded into the Document.

### insertExistingPage

```typescript
insertExistingPage(memDoc: Document, index: number, insertIndex: number): number
```

Insert the page of one Document into another. The document accepting the new page may be a Document or StreamDocument.
However the Document that the page originates from must be a Document.

### insertPage

```typescript
insertPage(rect: Rect, index: number): Page
```

Insert a new page into the Document pages tree. Returns the new page. Note pages are 0-based index.
If the index is < 0 the page will be insert at index 0, or as the first page in the document. If the index is
greater than the current page count a RangeError is thrown.

### append

```typescript
append(doc: Document): void
```

Append a Document to another Document. The Document to be appended to must be a Document, StreamDocument's are not supported for this operation.

### isLinearized

```typescript
isLinearized(): boolean
```
Check if the Document is linearized, returns a boolean.

### getWriteMode

```typescript
getWriteMode(): NPDFWriteMode
```

Get the write mode as NPDFWriteMode for the Document.

### isAllowed

```typescript
isAllowed(perm: ProtectionOption): boolean
```

Check if a PDF protection permission ProtectionOption is allowed, returns a boolean.

### createFont

```typescript
createFont(opts: NPDFCreateFontOpts): Font
```

Create / Get a PDF Font object. If the build you have does not include fontconfig (Windows users) you must include the fontFile (path to font file on disk) in NPDFCreateFontOpts.

### getObject

```typescript
getObject(ref: Ref): Obj
```

Get an object by Ref (indirect object reference). NoPoDoFo will always try to resolve a reference, some methods may however return a Ref instead of an IObj, this happens when the IObj in question
is not available by the parent object, or parent vector. This is primarily seen in a nested array structure.

#### Example

```typescript
const ar: IArray = someObject.getArray()
if(ar.at(0) instanceof nopodofo.Object) {
    // do something with the object
} else {
    let ref = ar.at(0)
    let obj = doc.getObject(ref)
    // do something with the object
}
```

### getNames

```typescript
getNames(create: boolean): nopodofo.Object|null
```

Get the document names tree, if a names tree does not exist null is returned.

### createXObject

```typescript
createXObject(rect: Rect): XObject
```

Creates a new XObject, returns the newly created XObject.

### createPage

```typescript
createPage(rect: Rect): Page
```

Create a new PDF Page, returns the newly created page.

### createPages

```typescript
createPages(rects: Rect[]): number
```
Creates as many pages as rects provided, returns the new page count of the document.

### getAttachment

```typescript
getAttachment(uri: string): FileSpec
```

Get an attached file, if the file is not found null is returned.

### load

```typescript
load(file: string | Buffer,
    opts: {
        forUpdate?: boolean,
        password?: string
    },
    cb: Callback<void>): void

load(file: string | Buffer, cb: Callback<void>): void
```

Load a PDF document into memory, remember Document loads objects on demand. This method accepts document in the form of
a buffer or as a file path. If the document requires a password, the password may be provided in the opts object. If the 
document is password protected and a password is not provided a `Password Required` error will be thrown. If forUpdate is
set to true, updates to the document will be written as an incremental update. An incremental update appends the changes
to the end of the document instead of re-writing the document.

#### Examples

__Loading a document from disk__
```typescript
import {npdf} from 'nopodofo'
const doc = new npdf.Document()
doc.load('/file/path.pdf', e => {
    if(e) {
        // Load password on error
        if(e.message.includes('Password required')) {
            // handle error or set password if password is known
            doc.password = 'secret'
        }
    }
    // do something with the document
})
```

__Loading a document from Buffer__
```typescript
import {npdf} from 'nopodofo'
import {S3} from 'aws-sdk'
const doc = new npdf.Document()
// getting a document from S3
const {Body: buffer} = await new S3().getObject({Bucket, Key}).promise()
doc.load(buffer, e => {
    if(e) {/* handle error */}
    // do something with document
})
```

### splicePages

```typescript
splicePages(startIndex: number, count: number): void
```

Deletes one or more pages from the document by removing the pages reference from the pages tree. 
This does NOT remove the page object as the page object may be used by other objects in the document.
Parameter startIndex is the index of the page to start at, pages are 0-based indexed, and parameter count is the number
of pages from the startIndex to delete.

### insertPages

```typescript
insertPages(fromDoc: Document, startIndex: number, count: number): number
```

Copies one or more pages from another pdf to this document. This function copies the entire document to the target document and then
deletes pages that are not of interest. This is a much faster process, but without object garbage collection the document may result in a much larger
than necessary document.

### write

```typescript
write(destination: Callback<Buffer> | string, cb?: Callback<string>): void
```

Write the file and any modifications made to the file to disk or a nodejs buffer. If destination is a string (file path), write will write
the document to this path, if destination is not provided a buffer will be returned in the callback.

### hasSignatures

```typescript
hasSignatures(): boolean
```

Iterate each page of the document for signature fields, returns true if signature field is found.

### getSignatures

```typescript
getSignatures(): ISignatureField[]
```
Iterates each page of the document for signature fields and returns an array of all signature fields found.

See [ISignatureField](https://corymickelson.github.io/NoPoDoFo/interfaces/_field_.isignaturefield.html)

### gc

```typescript
gc(file: string, pwd: string, output: string, cb: Callback<string|Buffer>): void
```

gc is a static method on the Document class. This method reads the document into memory, and re-writes the document 
in [NPDFWriteMode](https://corymickelson.github.io/NoPoDoFo/modules/_base_document_.npdfwritemode.html) `Compact` removing any objects that are not touched from the document [catalog](#catalog).
Parameters file is required, if the document is password protected you may pass the password as the second parameter, otherwise null. If an output
is provided the new document will be written to the output location, otherwise a nodejs buffer will be returned in the callback.