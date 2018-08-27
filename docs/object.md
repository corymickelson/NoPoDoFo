# API Documentation for Object

## NoPoDoFo Object
<<<<<<< HEAD
=======

>>>>>>> 8b7823f8d1343ca4542b7d08cd31d5bbdfbf7ebb
```typescript
class Object {
  readonly reference: { object: number, generation: number }
  readonly length: number
  readonly stream: Stream
  readonly type: NPDFDataType
  immutable: boolean

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
  getBuffer(): Buffer
  clear(): void
}
```

## Constructors
NoPoDoFo does not support the creation of new objects by the Object class, however many of the methods used in NoPoDoFo
will return or expose an accessor to retrieve the underlying PDF object data type.

## Properties

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

Get the object as a PDF [Dictionary](./dictionary.md) value

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

Get the object as a Buffer value

### clear

```typescript
clear(): void
```

Clear the object from memory and all internal variables.
