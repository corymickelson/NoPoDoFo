# API Documentation for Object

## NoPoDoFo Object
```typescript
class Object {
  readonly reference: { object: number, generation: number }
  readonly length: number
  readonly stream: Stream
  readonly type: NPDFDataType
  immutable: boolean

  hasStream(): boolean
  getOffset(key: string): Promise<number>
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

### getOffset
```typescript
getOffset(key: string): Promise<number>
```

### write
```typescript
write(output: string, cb: Function): void
```

### flateCompressStream
```typescript
flateCompressStream(): void
```

### delayedStreamLoad
```typescript
delayedStreamLoad(): void
```

### getBool
```typescript
getBool(): boolean
```

### getDictionary
```typescript
getDictionary(): Dictionary
```

### getString
```typescript
getString(): string
```

### getName
```typescript
getName(): string
```

### getReal
```typescript
getReal(): number
```

### getNumber
```typescript
getNumber(): number
```

### getArray
```typescript
getArray(): Array
```

### getBuffer
```typescript
getBuffer(): Buffer
```

### clear
```typescript
clear(): void
```