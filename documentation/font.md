# API Documentation for Font

- [API Documentation for Font](#api-documentation-for-font)
  - [NoPoDoFo Font](#nopodofo-font)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [object](#object)
    - [size](#size)
    - [scale](#scale)
    - [charSpace](#charspace)
    - [wordSpace](#wordspace)
    - [underline](#underline)
    - [strikeOut](#strikeout)
    - [identifier](#identifier)
  - [Methods](#methods)
    - [isBold](#isbold)
    - [isItalic](#isitalic)
    - [getEncoding](#getencoding)
    - [getMetrics](#getmetrics)
    - [stringWidth](#stringwidth)
    - [write](#write)
    - [embed](#embed)

## NoPoDoFo Font
Before you can draw (see [Painter](./painter.md)) on a page, you must first create and set a font object.
A font object can be reused as often as you want.
```typescript
class Font {
  readonly object: Object
  size: number
  scale: number
  charSpace: number
  wordSpace: number
  underline: boolean
  strikeOut: boolean
  readonly identifier: string
  isBold(): boolean
  isItalic(): boolean
  getEncoding(): Encoding
  getMetrics(): NPDFFontMetrics
  stringWidth(v: string): number
  write(content: string, stream: Stream): void
  embed(): void
  isSubsetting(): boolean
  embedSubsetFont(): void
}
```

## Constructors
-------------

To construct a new Font object use [Document.createFont](./document.md#createfont).

## Properties
--------------

### object
Get the underlying font [Object](./object.md)

### size
Get or set the font size

### scale
Get or set the font scale

### charSpace
Get or set font character spacing

### wordSpace
Get or set font word spacing

### underline
Enable or disable underline

### strikeOut
Enable or disable strikeout

### identifier
Get the font identifier

## Methods
----------

### isBold

```typescript
isBold(): boolean
```
Get bold flag as boolean

### isItalic
```typescript
isItalic(): boolean
```
Get italic flag as boolean

### getEncoding
```typescript
getEncoding(): Encoding
```
Get the font [Encoding](./encoding.md)

### getMetrics

```typescript
getMetrics(): NPDFFontMetrics
```
Get the font metrics as NPDFFontMetrics

### stringWidth
```typescript
stringWidth(v: string): number
```
Calculate the width of a string using the font

### write

```typescript
write(content: string, stream: Stream): void
```
Write contents to a stream

### embed

```typescript
embed(): void
```

Embed the font in the document. It is highly recommended to embed fonts, if a font is not embedded and
the viewer's machine does not have the font installed locally, the contents may not render (some viewers will use a fallback font)

### isSubsetting

```typescript
isSubsetting(): boolean
```

Check if this is a subset font

### embedSubsetFont

```typescript
embedSubsetFont(): void
```

Embed subset-font into the [Page](./page.md). Call [isSubsetting](#issubsetting) to assert this is a subset font before 
embedding as an exception will be thrown if the font is not a subset.