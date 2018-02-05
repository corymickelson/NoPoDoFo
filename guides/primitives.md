# NoPoDoFo PDF Primitive Data Types

NoPoDoFo exposes pdf primitives for low level document manipulations.
Please familiarize yourself with the [PDF specification](http://wwwimages.adobe.com/content/dam/acom/en/devnet/pdf/pdf_reference_archive/pdf_reference_1-7.pdf). 

## PDF Data types

NoPoDoFo provides handles to the following PDF data types:
 - PdfObject as [Obj](#NoPoDoFo.Obj)
 - PdfIndirectObject as [Ref](#NoPoDoFo.Ref)
 - PdfStream as [Stream](#NoPoDoFo.Stream)
 
PdfArray, PdfString, PdfNull, PdfNumber, PdfName, PdfDictionary and PdfBool are all returned as their native Javascript counterpart and are variants of NoPoDoFo.Obj.

### NoPoDoFo.Obj
The Body of a PDF is composed of collection of PdfObjects, these may be in the format of an inline object or an indirect object, NoPoDoFo exposes indirect objects as [Ref](#NoPoDoFo.Ref). 
An Object is defined by an Object number and Generation number, Object and Generation numbers are read only properties ( for now, new document creation is a work in progress ).
Objects typically contain a dictionary, but any Pdf data type is possible.
The type of the Object is exposed via `Obj.type`.

``` typescript
interface NObj {
    reference: number
    owner: number
    length: number
    stream: Stream
    type: PDType
    hasStream():boolean
    getOffset(key: string): Promise<number>
    write(output: string, cb: Function): void
    flateCompressStream(): void
    delayedStreamLoad(): void
    asBool(): boolean
    asString(): string
    asName(): string
    asReal(): number
    asNumber(): number
    asArray(): NArray
    asObject(): { [key: string]: Obj }
    asReference(): Ref
    asBuffer(): Buffer
}
```
For more on NoPoDoFo.Obj see [here](../docs/classes/_object_.obj.html)

### NoPoDoFo.Ref
The NoPoDoF.Ref type is loosely a type of pointer to an NoPoDoFo.Obj. 
``` typescript
// ... assuming the document has already been loaded
let body = doc.getObjects()
    .filter(i => i.type === 'Dictionary')
    .filter(i => {
        let o = i.asDictionary().toObject()
        return Object.keys(o).filter(k => k.type === 'Reference').length > 0
    })
    .map(i => i.asReference().deref()) // find all the ref's in dictionary objects and 'deref' to NoPoDoFo.Obj
```

### NoPoDoFo.Stream
A stream object, like a string object, is a sequence of bytes. However, a PDF application can read a stream incrementally, while a string must be read in its entirety. Furthermore, a stream can be of unlimited length, whereas a string is subject to an implementation limit. For this reason, objects with potentially large amounts of data, such as images and page descriptions, are represented as streams.
NoPoDoF.Stream provides access to Stream Object via `Obj.stream`. 
``` typescript
// ... assuming the body has been parsed and the object we are working with is a dictionary
if(o.type === 'Dictionary' && o.hasStream()) {
    const data = o.stream // node.js buffer
}
```

### NoPoDoFo.Obj.asArray
See [proxied](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/proxied.md)
### NoPoDoFo.Obj.asObject
See [proxied](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/proxied.md)
