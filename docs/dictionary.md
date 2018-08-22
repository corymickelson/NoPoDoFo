# API Documentation for Dictionary

* [Properties](#properties)
  * [immutable](#immutable)
* [Methods](#methods)
  * [getKey](#getkey)
  * [addKey](#addkey)
  * [getKeys](#getkeys)
  * [hasKey](#haskey)
  * [removeKey](#removekey)
  * [getKeyAs](#getkeyas)
  * [clear](#clear)
  * [write](#write)
  * [writeSync](#writesync)

## NoPoDoFo Dictionary
A Dictionary is one of the primary building blocks of a PDF. The PDF dictionary type data structure is similar to a nodejs object data structure in
that it is simply a collection of key value pairs. NoPoDoFo does not support the creation of new Dictionary objects, actions that require creating
a new Dictionary are handled implicitly by PoDoFo. 
```typescript
class Dictionary {
  dirty: boolean
  immutable: boolean

  getKey(k: string): nopodofo.Object
  addKey(prop: NPDFName|string, value: boolean | number | string | nopodofo.Object): void
  getKeys(): string[]
  hasKey(k: string): boolean
  removeKey(k: string): void
  getKeyAs(k: string, t: NPDFDictionaryKeyType): string | number
  clear(): void
  write(destination: string, cb: (e: Error, i: string) => void): void
  writeSync(destination: string): void
}
```

## Properties

### dirty
This flag is internally by PoDoFo, dirty is set to true if there has been a modification after construction.

### immutable
This property will get or set a corresponding property on the PoDoFo PdfDictionary. When set to true no keys can be edited or changed.

## Methods

### getKey
Get the value of the `k` key. Value is returned as an [Object](./object.md) or [Dictionary](./dictionary.md). NoPoDoFo will always try to 
resolve Ref types to their corresponding [Object](./object.md) but in the instance a value can not be resolved an error will be thrown.

### addKey
Add a new entry to the dictionary, with an NPDFName `k` and a value. If the value is an [Object](./object.md) the object's reference will be
stored instead, for primitive data types a new PoDoFo PdfVariant will be created with the value provided.

### getKeys
Get all the keys in the dictionary.

### hasKey
Check if a key exists in the dictionary. Check is not recursive, if you are trying to check a nested dictionary you will need to call this method at
each level.

### removeKey
Remove an entry from the dictionary.

### getKeyAs
Get the value of the `k` key out of the dictionary as one of NPDFCoerceKeyType types. If the value can not be cast as the type defined an error will be thrown.

### clear
Remove all keys from the dictionary.

### write
Writes the dictionary to disk. Can be useful for debugging.

### writeSync
Writes the dictionary to disk as a blocking operation. Can be useful for debugging.