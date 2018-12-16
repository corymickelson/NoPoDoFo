# API Documentation for Ref

- [API Documentation for Ref](#api-documentation-for-ref)
  - [NoPoDoFo Ref](#nopodofo-ref)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [gennum](#gennum)
    - [objnum](#objnum)
  - [Methods](#methods)

## NoPoDoFo Ref

A Ref in NoPoDoFo is a simple class encapsulating the PoDoFo PdfReference class. A Ref(erence) is any object
in a pdf file that is labeled as an __indirect object__.  An indirect object provides a unique id for the object
consisting of two parts, an object number, and a generation number. The Ref type is a commonly seen return type when
using NoPoDoFo low level api's. 
The Ref (indirect object) can be resolved by the parent [document](./document.md).

```typescript
class Ref {
    readonly gennum: number
    readonly objnum: number
}
```

## Constructors

NoPoDoFo does not expose a public constructor for Ref. A Ref instance can be obtained from [Obj.reference](./object.md#reference)

## Properties

### gennum

The reference generation number. Readonly property.

### objnum

The reference object number. Readonly property.

## Methods