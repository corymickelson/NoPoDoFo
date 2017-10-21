# Page
The pdf page object. The page object may only be instantiated at this point in time through the Document class.

## Syntax
```typescript
let doc = new Document('/foo/bar/file.pdf')
let page = doc.getPage(pageIndex:number): Page
```
#### parameters
```pageIndex```: zero based page index
#### returns
```page```: new Page instance
#### throws
```Error```: index out of range
## Properties
#### <u>height</u> 
Page height
</br>
<b><i>Get</i></b>: number
</br>
<b><i>Set</i></b>: number
```typescript
let page = doc.getPage(0)
assert(page.height === 791.968)
```
#### <u>number</u> 
Get page number, one based index
</br>
<b><i>Get</i></b>: number
</br>
<b><i>Set</i></b>: void (does nothing), page can not be reorder via this property 
```typescript
let index = 0
let page = doc.getPage(index)
assert(page.number === index - 1)
```
#### <u>rotation</u> 
Get and set the rotation value of page
</br>
<b><i>Get</i></b>: number
</br>
<b><i>Set</i></b>: number
```typescript
let page = doc.getPage(0)
assert(page.rotation === 0)
page.rotation = 90
doc.write('/dest/out.pdf')
let doc = new Document('/dest/out.pdf')
let nPage = doc.getPage(0)
assert(nPage === 90)
```
#### <u>trimbox</u> 
The trimbox is basically the size of the page after all other cutting operations have been processed. It's the final page size.
</br>
<b><i>Get</i></b>: Rect
</br>
<b><i>Set</i></b>: Rect
```typescript
let page = doc.getPage(0)
let box = page.trimbox
assert(page.height === box.height)
assert(page.width === box.width)
```
#### <u>width</u> 
Get and set page width
</br>
<b><i>Get</i></b>: number
</br>
<b><i>Set</i></b>: number
```typescript
let page = doc.getPage(0)
assert(page.width === 611.976)
```
## Methods
#### <u>getNumFields</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getFieldsInfo</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getField</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getField</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getFieldIndex</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getContents</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getResources</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getMediaBox</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getBleedBox</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getArtBox</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getNumAnnots</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>createAnnotation</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getAnnotation</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>getAnnotations</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
#### <u>deleteAnnotation</u> 
</br>
<b><i>Returns</i></b>:
</br>
<b><i>Throws</i></b>:
