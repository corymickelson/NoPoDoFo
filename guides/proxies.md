# Proxies
NoPoDoFo uses a Javascript Proxy for a more fluid api. NoPoDoFo proxies the PdfArray and PdfDictionary classes revealing
them as Javascript array's and Javascript object's through NoPoDoFo.Obj class [methods](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/primitives.md).

## What is a proxy?

The Proxy object is used to define custom behavior for fundamental operations (e.g. property lookup, assignment, enumeration, function invocation, etc).

## NoPoDoFo.Obj
`NoPoDoFo.Obj` is the base class for all Pdf data types, for more information see [primitives](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/primitives.md).
`Obj` provides methods for converting the `Obj.type` to it's native Javascript equivalent, except in the case of asArray and asObject which return a Proxy. The reason these two are returned as proxies instead of Javascript array's or object's is for the purpose of allowing modifications to the array or object to persist back down to the native PDF data type while at the same time providing the user with a familiar data structure and api.

### asArray
```typescript
asArray(): Array<Obj>
```
This method returns a JS proxy which can be used as an array. 
The return type implements the [NArray](https://github.com/corymickelson/NoPoDoFo/blob/master/lib/object.ts#L12-L20) interface.
To modify the array, the underlying PdfArray must be mutable, this is can get/set via `NArray.immutable`
```typescript
const obj = document.getObjects()[0] // grab the first obj in the pdf body
//check type
if(obj.type === 'Array') {
    // get the obj as JS array type data structure
    const arr = obj.asArray()
    // check if array is mutable
    if(arr.immutable === false) {
        // operations modifying data are permitted
    }
}
```

### asObject
```typescript
asObject(): {[key:string] : Obj}
```
This method returns a JS proxy which can be used as an object. If the underlying PdfObject is immutable, get/set via the `immutable` property,
and the operation in use is modifying the original data structure a TypeError will be thrown. Currenly creating new PdfObjects is
not supported, any operations that create a new Object will also through a TypeError.


```typescript
const obj = document.getObjects()[0]
if(obj.type === 'Dictionary') {
    const dict = obj.asObject() // JS Object type data structure
}
```