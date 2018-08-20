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

```typescript
class Dictionary {
  dirty: boolean
  immutable: boolean

  getKey(k: string): Object
  addKey(prop: string, value: boolean | number | string | Object): void
  getKeys(): string[]
  hasKey(k: string): boolean
  removeKey(k: string): void
  getKeyAs(k: string, t: NPDFDictionaryKeyType): string | number
  clear(): void
  write(destination: string, cb: (e: Error, i: string) => void): void
  writeSync(destination: string): void
}
```

## Methods

## Properties