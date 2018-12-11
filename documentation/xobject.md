# API Documentation for XObject

- [API Documentation for XObject](#api-documentation-for-xobject)
  - [NoPoDoFo XObject](#nopodofo-xobject)
  - [## Constructors](#constructors)
  - [## Properties](#properties)
    - [contents](#contents)
    - [contentsForAppending](#contentsforappending)
    - [resources](#resources)
    - [pageMediaBox](#pagemediabox)
  - [## Methods](#methods)

## NoPoDoFo XObject

An XObject is an object which can hold a set of instructions, these instructions are typically draw operations.
XObject are reusable, and can be applied to an Object by setting the xobject as the value of an appearance stream.
Creating an xobject is done via `BaseDocument.createXObject`

```typescript
class XObject {
    readonly contents: Object
    readonly contentsForAppending: Object
    readonly resources: Object
    readonly pageMediaBox: Rect
}
```

## Constructors
-------------

## Properties
--------------

### contents

Readonly property. Get the contents of the XObject as an [Object](./object.md)

### contentsForAppending

Readonly property.

### resources

Readonly property. Get the XObjects resources dictionary.

### pageMediaBox

Readonly property. Get the XObject's media box. See [Page](./page.md) for a diagram of page boxes.

## Methods
-----------