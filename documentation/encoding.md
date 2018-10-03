# API Documentation for Encoding

- [API Documentation for Encoding](#api-documentation-for-encoding)
  - [NoPoDoFo Encoding](#nopodofo-encoding)
  - [Properties](#properties)
  - [Methods](#methods)
    - [addToDictionary](#addtodictionary)
    - [convertToUnicode](#converttounicode)
    - [convertToEncoding](#converttoencoding)

## NoPoDoFo Encoding

The document or page contents encoding, must be one of NPDFFontEncoding. This class does not expose a constructor. The creation of 
an instance of Encoding is handled by the [Font](./font.md) class.

```typescript
class Encoding {
  addToDictionary(target: Object): void
  convertToUnicode(content: string, font: Font): string
  convertToEncoding(content: string, font: Font): Buffer
}
  ```

## Properties
---------------

## Methods
----------

### addToDictionary

```typescript
addToDictionary(target: Object): void
```

Add the encoding instance to the [Object](./object.md) provided. Object must be a [Dictionary](./dictionary.md) type.

### convertToUnicode

```typescript
convertToUnicode(content: string, font: Font): string
```

Converts the contents to unicode using in the provided [Font](./font.md).

### convertToEncoding

```typescript
convertToEncoding(content: string, font: Font): Buffer
```

Converts the contents with the [Encoding](./encoding.md) using in the provided [Font](./font.md).