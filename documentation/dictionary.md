# API Documentation for Dictionary

- [API Documentation for Dictionary](#api-documentation-for-dictionary)
  - [NoPoDoFo Dictionary](#nopodofo-dictionary)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [dirty](#dirty)
    - [immutable](#immutable)
    - [obj](#obj)
  - [Methods](#methods)
    - [getKeyType](#getkeytype)
    - [getKey](#getkey)
    - [addKey](#addkey)
    - [getKeys](#getkeys)
    - [hasKey](#haskey)
    - [removeKey](#removekey)
    - [getKeyAs](#getkeyas)
    - [clear](#clear)
    - [write](#write)
    - [writeSync](#writesync)
    - [asObject](#asobject)

## NoPoDoFo Dictionary

A Dictionary is one of the primary building blocks of a PDF. The PDF dictionary type data structure is similar to a nodejs object data structure in
that it is simply a collection of key value pairs.

```typescript
class Dictionary {
  dirty: boolean
  immutable: boolean
  readonly obj: nopodofo.Object

  new(): Dictionary

  getKey<T>(k: string, resolveType?: boolean): T
  getKeyType(k:string): NPDFDataType
  addKey(prop: NPDFName|string, value: boolean | number | string | nopodofo.Object | nopodofo.Dictionary | nopodofo.Ref): void
  getKeys(): string[]
  hasKey(k: string): boolean
  removeKey(k: string): void
  getKeyAs(k: string, t: NPDFDictionaryKeyType): string | number
  clear(): void
  write(destination: string, cb: (e: Error, i: string) => void): void
  writeSync(destination: string): void
  asObject(): Object
}
```

## Constructors

A public constructor is available for instantiating a new empty dictionary. This dictionary does not belong to any document, and
therefore must be explicitly added to the document for it to take effect. An example of this can be seen in the [AP Cookbook](./cookbook/ap.md).

```typescript
new(): Dictionary
```

## Properties

--------------

### dirty

This flag is internally by PoDoFo, dirty is set to true if there has been a modification after construction.

### immutable

This property will get or set a corresponding property on the PoDoFo PdfDictionary. When set to true no keys can be edited or changed.

### obj

Readonly property accessor. Get the dictionary [Object](./object.md)

## Methods
------------

### getKeyType

```typescript
getKeyType(k:string): NPDFDataType
```

Get the NPDFDataType of the value of the `k` key.

### getKey

```typescript
getKey<T>(k: string, resolveType?: boolean): T
```

Get the value of the `k` key. Value is returned as `T` or [Ref](./ref.md).
It is good practice to check the type; [getKeyType](#getkeytype) of the object before casting a value to `T`.
NoPoDoFo will always try to resolve Ref types to their corresponding [Object](./object.md) or `T` type, but when the object can not be resolved a [Ref](./ref.md)
to the object is returned. This [Ref](./ref.md) object can be passed to [Document.getObject](./document.md#getobject) to resolve the reference.

### addKey

```typescript
addKey(prop: NPDFName|string, value: boolean | number | string | nopodofo.Object | nopodofo.Dictionary | nopodofo.Ref): void
```

Add a new entry to the dictionary, with an NPDFName `k` and a value. If the value is an [Object](./object.md) the object's reference will be
stored instead, for primitive data types a new PoDoFo PdfVariant will be created with the value provided.

### getKeys

```typescript
getKeys(): string[]
```

Get all the keys in the dictionary.

### hasKey

```typescript
hasKey(k: string): boolean
```

Check if a key exists in the dictionary. Check is not recursive, if you are trying to check a nested dictionary you will need to call this method at
each level.

### removeKey

```typescript
removeKey(k: string): void
```

Remove an entry from the dictionary.

### getKeyAs

```typescript
getKeyAs(k: string, t: NPDFDictionaryKeyType): string | number
```

Get the value of the `k` key out of the dictionary as one of NPDFCoerceKeyType types. If the value can not be cast as the type defined an error will be thrown.

### clear

```typescript
clear(): void
```

Remove all keys from the dictionary.

### write

```typescript
write(destination: string, cb: (e: Error, i: string) => void): void
```

Writes the dictionary to disk. Can be useful for debugging.

### writeSync

```typescript
writeSync(destination: string): void
```

Writes the dictionary to disk as a blocking operation. Can be useful for debugging.

### asObject

```typescript
asObject(): Object
```

Convert the PDF Dictionary into a javascript object. This is a __readonly__ object in context, the resulting object 
will not persist any changes back to the original PDF Dictionary.
The use of the method is best for simply viewing the contents of a dictionary in a familiar environment(Javascript).
