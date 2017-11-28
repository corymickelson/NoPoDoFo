# NoPoDoFo PDF Primitive Data Types

NoPoDoFo exposes pdf primitives for low level document manipulations.
Please familiarize yourself with the [PDF specification](http://wwwimages.adobe.com/content/dam/acom/en/devnet/pdf/pdf_reference_archive/pdf_reference_1-7.pdf). 

## PDF Data types

NoPoDoFo provides handles to the following PDF data types:
 - PdfObject as [Obj](#NoPoDoFo.Obj)
 - PdfArray as [Array](#NoPoDoFo.Array)
 - PdfDictionary as [Dictionary](#NoPoDoFo.Dictionary)
 - PdfIndirectObject as [Ref](#NoPoDoFo.Ref)
 - PdfStream as [Stream](#NoPoDoFo.Stream)
 
PdfString, PdfNull, PdfNumber, PdfName, and PdfBool are all returned as their native Javascript counterpart.

### NoPoDoFo.Obj
The Body of a PDF is composed of collection of PdfObjects, these may be in the format of an inline object or an indirect object, NoPoDoFo exposes indirect objects as [Ref](#NoPoDoFo.Ref). 
An Object is defined by an Object number and Generation number, Object and Generation numbers are read only properties ( for now, new document creation is a work in progress ).
Objects typically contain a dictionary, but any Pdf data type is possible.
The type of the Object is exposed via `Obj.type`.

``` typescript
// ... assuming the document has already been loaded
let body = doc.getObjects()
let objArrays = body.filter(i => i.type === 'Array')
let objDicts = body.filter(i => i.type === 'Dictionary')
```
For more on NoPoDoFo.Obj see [here](../docs/classes/_object_.obj.html)

### NoPoDoFo.Array
An array object is a one-dimensional collection of objects arranged sequentially. Unlike arrays in many other computer languages, PDF arrays may be heterogeneous; that is, an array’s elements may be any combination of numbers, strings, dictionaries, or any other objects, including other arrays. The number of elements in an array is subject to an implementation limit.
For more on NoPoDoFo.Arr see [arr](../docs/classes/_arr_.arr.html)

``` typescript
// ... assuming the document has already been loaded

// First get a handle to an object of type Array
let obj = doc.getObjects().filter(i => i.type === 'Array')[0]
let pdfArr = obj.asArray() // this is a PdfArray NOT a Javascript array
assert(pdfArr.contiains('Type')) // this is using PoDoFo to query the array
let jsArr = pdfArr.toArray() // the resulting array will NOT persist any changes back to the underlying pdf array
```

### NoPoDoFo.Dictionary
A dictionary object is an associative table containing pairs of objects, known as the dictionary’s entries. The first element of each entry is the key and the second element is the value. The key must be a name (unlike dictionary keys in PostScript, which may be objects of any type). The value can be any kind of object, including another dictionary.
NoPoDoFo Dictionary manipulations should be made using methods defined on the NoPoDoFo.Dictionary class. Calling NoPoDoFo.Dictionary.toObject will cast the Obj to a native Javascript object. Any changes made to the native Javascript object will NOT be persisted back to the underlying PdfObject.

``` typescript
// ... assuming the document has already been loaded
// First get a handle to an object of type Dictionary
let obj = doc.getObjects().filter(i => i.type === 'Dictionary')[0]
let pdfDict = obj.asDictionary()
// Getting and setting key/value
let subType:NoPoDoFo.Obj = obj.hasKey('SubType') ? obj.getKey('SubType'): undefined
obj.addKey('Type', 3) // change persists
obj.removeKey('SubType') // change persists
let jsObj:Object = obj.toObject() // any changes made to jsObj will not persist
```
### NoPoDoFo.Ref
The NoPoDoF.Ref type is a type of pointer to an NoPoDoFo.Obj. 
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

### String

### Number

### Null