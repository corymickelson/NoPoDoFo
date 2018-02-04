# Proxies
NoPoDoFo uses a Javascript Proxy for a more fluid api. NoPoDoFo proxies the PdfArray and PdfDictionary classes revealing
them as Javascript array's and Javascript object's through NoPoDoFo.Obj class [methods]().

## What is a proxy?

The Proxy object is used to define custom behavior for fundamental operations (e.g. property lookup, assignment, enumeration, function invocation, etc).

## NoPoDoFo.Obj
[NoPoDoFo.Obj]() is the base class for all Pdf data types, for more information see [primitives]().
Obj provides methods for converting the `Obj.type` to it's native Javascript equivalent.
If the data type conincides with a JS data type, a Proxy is returned where the getter / setter is "caught" and new or updated data is persisted back to the underlying PoDoFo Pdf type.
### asArray
```typescript
asArray(): Array<Obj>
```
This method returns a JS proxy which can be used as an array. 
The return type implements the [NArray]() interface.

```typescript
const obj = document.getObjects()[0] // grab the first obj in the pdf body
//check type
if(obj.type === 'Array') {
    // get the obj as JS array type data structure
    const arr = obj.asArray()
}
```

### asObject
```typescript
asObject(): {[key:string] : Obj}
```
This method returns a JS proxy which can be used as an Object. If the underlying PdfObject is immutable, get/set via the `immutable` property
and the operation in use is modifying the original data structure a type error will be thrown. Currenly creating new PdfObjects is
not supported, any operations that create a new Object will also through a type error.


```typescript
const obj = document.getObjects()[0]
if(obj.type === 'Dictionary') {
    const dict = obj.asObject() // JS Object type data structure
}
```