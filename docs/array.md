# API Documentation for Array

* [Properties](#properties)
  * [dirty](#dirty)
  * [length](#length)
  * [immutable](#immutable)

* [Methods](#methods)
  * [at](#at)
  * [pop](#pop)
  * [clear](#clear)
  * [write](#write)

## NoPoDoFo Array
An array object is a one dimensional heterogeneous collection of Object(s). PDF directly supports one dimensional arrays, however arrays of greater
dimension are possible by constructing arrays as elements of arrays; nested array.
NoPoDoFo does not currently support the creation of new PDf Array objects. NoPoDoFo Array's are constructed from a NoPoDoFo [Object](./object.md).

```typescript
export class Array {
  dirty: boolean
  readonly length: number
  immutable: boolean

  at(i: number): Ref | Object
  pop(): Object
  clear(): void
  push(v: Object): void
  write(destination: string): void
}
```

## Properties

### dirty
This flag is internally by PoDoFo, dirty is set to true if there has been a modification after construction.

### length
This is a readonly property returning the current size of the array.

### immutable
This property will get or set a corresponding property on the PoDoFo PdfArray. When set to true no keys can be edited or changed.

## Methods

### at
The [at] method will return the [Object](./object.md) or Ref (indirect object reference) at the index provided as the first parameter. If the index is
less than zero or greater than the array length a Range Error will be thrown.

### pop
This method will act the same as the native javascript array pop method. Pop will remove and return the last item of an array.

### clear
Removes all object in an array.

### push
Add an [Object](./object.md) to the end of the array.

### write
Write the array to the destination provided as the first parameter. This destination must be a valid location on disk.