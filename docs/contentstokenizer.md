# API Documentation for ContentsTokenizer

* [Properties](#properties)
* [Methods](#methods)
  * [reader](#reader)

## NoPoDoF ContentsTokenizer
This class is a parser for the content streams in a [Document](./document.md).
```typescript
class ContentsTokenizer {
  new(doc: Base, pageIndex: number): ContentsTokenizer
  reader(): Iterator<string>
}
```

## Constructors

Creates a new ContentsTokenizer for a [Document](./document.md) [Page](./page.md). A Range Error will be thrown if the 
`pageIndex` is less than zero or greater than `getPageCount`.
```typescript
new(doc: Document, pageIndex: number): ContentsTokenizer
```

## Properties

## Methods

### reader
Reads a [Page](./page.md)'s contents stream into an array returning an array iterator.


