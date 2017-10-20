# Document

The Document constructor creates a new [PdfMemDocument](http://podofo.sourceforge.net/doc/html/classPoDoFo_1_1PdfMemDocument.html#ae72a4141ed85e8abda6a368b220854fa).
Document is the core class for reading and modifying existing pdf documents. 

## Syntax

```typescript 
new Document(path:string): Document
```

#### Parameters

``` path ``` : 
File path to pdf file.

#### Return value

``` Document ```: NoPoDoFo Document object

#### throws

``` Error ```:  Password required exception, or file not found exception

``` typescript
let document: Document
try {
    document = new Document('/foo/bar/file.pdf')
} catch(e) {
    if(e.message.startsWith('Password required'))
        document.password = 'secret'
}
```

## Description

The Document class does not immediatley load the entire pdf document into memory, when an object is needed it is loaded on demand.

## Methods

#### <u>getPageCount</u>
Gets the page count of the loaded document.
</br>
<b><i>Returns</b></i>: number
</br>
<b><i>Throws</i></b>: void
``` typescript
let doc = new Document('/foo/bar/file.pdf')
let pCount:number = doc.getPageCount()
```
#### <u>getPage</u>
Get a page instance from the currently loaded document
</br>
<b><i>Returns</i></b> Page
</br>
<b><i>Throws</i></b>: out of range exception
```typescript
Document.getPage(index:number): Page
```
``` typescript
let doc = new Document('/foo/bar/file.pdf')
let page = doc.getPage(0)
```
#### <u>getObjects</u>
Get Pdf body as an array of [Obj](). For lookup only, modifying array will not persist to underlying pdf.
</br>
<b><i>Returns</i></b>: void
</br>
<b><i>Throws</i></b>: Password required exception, file not found exception
```typescript
let doc = new Document('/foo/bar/file.pdf')
let docObjs:Array<Obj> = doc.getObjects()
```
#### <u>mergeDocument</u>
Concatenates a pdf document to the end of the currently loaded document
</br>
<b><i>Returns</i></b>: void
</br>
<b><i>Throws</b></i>: Password required, file not found
```typescript
let doc = new Document('/foo/bar/file.pdf')
doc.mergeDocument('/some/other/file.pdf')
doc.write('/dest/output.pdf')
```
#### <u>deletePage</u>
Remove page object
</br>
<b><i>Returns</i></b>:void
</br>
<b><i>Throws</i></b>: index out of range
```typescript
let doc = new Document('/foo/bar/file.pdf')
doc.deletePage(pageIndex:number)
```
#### <u>getVersion</u>
Get Pdf version</br>
<b><i>Returns</i></b>: number 
</br>
<b><i>Throws</i></b>: void
```typescript
let doc = new Document('/foo/bar/file.pdf')
doc.getVersion()
```
#### <u>isLinearized</u>
Is document linearized
</br>
<b><i>Returns</i></b>: boolean
</br>
<b><i>Throws</i></b>: void
```typescript
let doc = new Document('/foo/bar/file.pdf')
let linearized:boolean = doc.isLinearized()
```
#### <u>write</u>
Any modifications to the document <i>will not be persisted</i> until the document is written
</br>
<b><i>Returns</i></b>: void
</br>
<b><i>Throws</i></b>: void
```typescript
let doc = new Document('/foo/bar/file.pdf')
doc.write('/dest/file.pdf')
```
#### <u>createEncrypt</u>

<b><i>Returns</i></b>
<b><i>Throws</i></b>
#### <u>getTrailer</u>
Gets the trailer object
</br>
<b><i>Returns</i></b>: Obj
</br>
<b><i>Throws</i></b>: Exception
```typescript
let doc = new Document('/foo/bar/file.pdf')
let trailer:Obj = doc.getTrailer()
```
#### <u>freeObjMem</u>

<b><i>Returns</i></b>
<b><i>Throws</i></b>