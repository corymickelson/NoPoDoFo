# API Documentation for ContentsTokenizer

- [API Documentation for ContentsTokenizer](#api-documentation-for-contentstokenizer)
  - [NoPoDoF ContentsTokenizer](#nopodof-contentstokenizer)
  - [Constructors](#constructors)
  - [Properties](#properties)
  - [Methods](#methods)
    - [readSync](#readsync)
    - [read](#read)

## NoPoDoF ContentsTokenizer

This class is a parser for the content streams in a [Document](./document.md).

```typescript
class ContentsTokenizer {
  new(doc: Base, pageIndex: number): ContentsTokenizer

  readSync(): Iterator<string>
  read(cb: Callback<Buffer>): void
}
```

## Constructors
---------------

Creates a new ContentsTokenizer for a [Document](./document.md) [Page](./page.md). A Range Error will be thrown if the 
`pageIndex` is less than zero or greater than `getPageCount`.

```typescript
new(doc: Document, pageIndex: number): ContentsTokenizer
```

## Properties
------------

## Methods
----------

### readSync

```typescript
readSync(): Iterator<string>
```

Reads a [Page](./page.md)'s contents stream into an array returning an array iterator.

### read

```typescript
read(cb: Callback<Buffer>): void
```

Reads a [Page](./page.md)'s contents stream into an Buffer. Async operation.