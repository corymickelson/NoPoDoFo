# API Documentation for Object

- [API Documentation for Object](#api-documentation-for-object)
  - [NoPoDoFo Object](#nopodofo-object)
  - [## Constructors](#constructors)
  - [## Properties](#properties)
    - [reference](#reference)
    - [length](#length)
    - [stream](#stream)
    - [type](#type)
    - [immutable](#immutable)
  - [## Methods](#methods)
    - [hasStream](#hasstream)
    - [getOffset](#getoffset)
    - [write](#write)
    - [flateCompressStream](#flatecompressstream)
    - [delayedStreamLoad](#delayedstreamload)
    - [getBool](#getbool)
    - [getDictionary](#getdictionary)
    - [getString](#getstring)
    - [getName](#getname)
    - [getReal](#getreal)
    - [getNumber](#getnumber)
    - [getArray](#getarray)
    - [getBuffer](#getbuffer)
    - [clear](#clear)
    - [resolveIndirectKey](#resolveindirectkey)

## NoPoDoFo Object

```typescript
class Object {
  readonly reference: Ref
  readonly length: number
  readonly stream: Stream
  readonly type: NPDFDataType
  immutable: boolean
  
  constructor()
  constructor(s: string)
  constructor(a: string[] | number[])
  constructor(d: number)

  hasStream(): boolean
  getOffset(key: string, cb: Callback<number>): void
  write(output: string, cb: Function): void
  flateCompressStream(): void
  delayedStreamLoad(): void
  getBool(): boolean
  getDictionary(): Dictionary
  getString(): string
  getName(): string
  getReal(): number
  getNumber(): number
  getArray(): Array
  getRawData(): Buffer
  clear(): void
  resolveIndirectKey(key: string): nopodofo.Object
}
```

## Constructors
--------------

```typescript
constructor()
```

The default constructor, calling NoPoDoFo.Object() will create a new instance of a PoDoFo::PdfObject as type [Dictionary](./dictionary.md)

```typescript
constructor(s: string)
```

Creates a new instance of NoPoDoFo::Object as type string.

```typescript
constructor(a: string[] |  number[])
```

Create a new instance of NoPoDoFo::Object with type of array. Currently only supports single dimension, homogeneous arrays.

```typescript
constructor(n: number)
```

Create a new instance of NoPoDoFo::Object as type number.

## Properties
--------------


### reference
Get the reference of the object. If the object is not an indirect object a reference is not available. This would
signify that the object is an inline object.

### length

Get the byte length of the object

### stream

If the object has a stream, get the stream as a Nodejs Buffer

### type

Get the type of object as one of NPDFDataType

### immutable

Get or set immutability

## Methods
-------------

### hasStream
```typescript
hasStream(): boolean
```

Check if the Object has a stream

### getOffset

```typescript
getOffset(key: string, cb: Callback<number>): void
```

Get the byte offset of the object. This method is expensive as it has to calculate the offset of the object if the object was written to disk.

### write

```typescript
write(output: string, cb: Function): void
```

Write the object to disk, useful for debugging

### flateCompressStream

```typescript
flateCompressStream(): void
```

Compress the object stream using the FlateDecode algorithm. Entries to the filter [Dictionary](./dictionary.md) will be added if necessary.

### delayedStreamLoad

```typescript
delayedStreamLoad(): void
```

Dynamically load the object and any associated stream if the stream is not already loaded

### getBool

```typescript
getBool(): boolean
```

Get the object as a boolean value

### getDictionary

```typescript
getDictionary(): Dictionary
```

Get the object as a PDF [Dictionary](./dictionary.md) value. This is a copy or readonly instance of the dictionary
(this is currently a limitation of the lower level api).
Any changes made to this dictionary will not be persisted back to the document. Higher level api's are available for
manipulating dictionary values (ex. [Form](./form.md) [DA, DR, CO, and SigFlags]). If there is a use case that you
need that is not yet exposed please submit an issue.

### getString

```typescript
getString(): string
```

Get the object as a string value

### getName

```typescript
getName(): string
```

Get the object as a name (string) value

### getReal

```typescript
getReal(): number
```

Get the object as a real (number) value

### getNumber

```typescript
getNumber(): number
```

Get the object as a number value

### getArray

```typescript
getArray(): Array
```

Get the object as an [Array](./array.md) value

### getBuffer

```typescript
getBuffer(): Buffer
```

Get the object as a Buffer value, __data__ is copied into a nodejs buffer.

### clear

```typescript
clear(): void
```

Clear the object from memory and all internal variables.

### resolveIndirectKey

```typescript
resolveIndirectKey(key: string): nopodofo.Object
```

Resolve a [Dictionary](./dictionary.md) key. The object type must be a Dictionary. If the key could not be resolved
an error will be thrown.
