# API Documentation for Page

* [Properties](#properties)
  * [rotation](#rotation)
  * [trimBox](#trimbox)
  * [number](#number)
  * [width](#width)
  * [height](#height)
  * [contents](#contents)
  * [resources](#resources)
* [Methods](#methods)
  * [getField](#getfield)
  * [getFields](#getfields)
  * [fieldCount](#fieldcount)
  * [getFieldIndex](#getfieldindex)
  * [getMediaBox](#getmediaBox)
  * [getBleedBox](#getbleedBox)
  * [getArtBox](#getartBox)
  * [annotationCount](#annotationcount)
  * [createAnnotation](#createannotation)
  * [createField](#createfield)
  * [deleteField](#deletefield)
  * [flattenFields](#flattenfields)
  * [getAnnotation](#getannotation)
  * [deleteAnnotation](#deleteannotation)

## NoPoDoFo Page

```typescript
class Page {
  rotation: number
  trimBox: Rect
  number: number
  width: number
  height: number
  contents: Object
  resources: Object

  /**
   * Get the field as a field of type T where T is one of: Checkbox, Pushbutton, Textfield, ComboBox, or ListField
   * @param index - field index
   * @see Checkbox
   * @see ListBox
   * @see TextField
   * @see ComboBox
   * @see PushButton
   */
  getField<T extends Field>(index: number): T
  getFields(): Field[]
  fieldCount(): number
  getFieldIndex(fieldName: string): number
  getMediaBox(): Rect
  getBleedBox(): Rect
  getArtBox(): Rect
  annotationCount(): number
  createAnnotation(type: NPDFAnnotation, rect: Rect): Annotation
  createField(type: NPDFFieldType, annot: Annotation, form: Form, opts?: Object): Field
  deleteField(index: number): void
  /**
   * Flattening is the process of taking a fields appearance stream, appending that appearance stream
   * to the page, and then removing the field object and annotation widget, and scrubbing all references 
   * to the field from the document (scrub the page and acroform dictionary)
   */
  flattenFields(): void
  getAnnotation(index: number): Annotation
  deleteAnnotation(index: number): void
}
```

## Constructors

NoPoDoFo does not support a public Page constructor. To create a new Page use [Document.createPage](./document.md#createpage)

## Properties

### rotation

Page rotation accessor; Get and set the page rotation value. This value must be between 0 and 270, and in increments of 90.

### trimBox

TrimBox accessor; Get and set the page's trimbox [Rect](./rect.md). The trimbox is enclosed by the bleedbox, and the trimbox encloses the artbox.

![Page Boxes](./page-boxes.png)

### number

Get the page number, 1-based.

### width

The page width, a standard page is 612 X 792 (width X height)

### height

The page height, a standard page is 612 X 792 (width X height)

### contents

Get the page's contents [Object](./object.md)

### resources

Get the page's resources [Object](./object.md)

## Methods

### getField

```typescript
getField<T extends Field>(index: number): T
```

Get the [Field](./field.md) from the page's [Annotation](./annotations.md) [Array](./array.md)

### getFields

```typescript
getFields(): Field[]
```

Get all the [Fields](./field.md) in the page's [Annotation](./annotations.md) [Array](./array.md)

### fieldCount

```typescript
fieldCount(): number
```

Get the number of [Fields](./field.md) on the page

### getFieldIndex

```typescript
getFieldIndex(fieldName: string): number
```

Get the index of a [Field](./field.md) matching the fieldName provided

### getMediaBox

```typescript
getMediaBox(): Rect
```

Get the media box of the page

### getBleedBox

```typescript
getBleedBox(): Rect
```

Get the bleed box of the page

### getArtBox

```typescript
getArtBox(): Rect
```

Get the art box of the page

### annotationCount

```typescript
annotationCount(): number
```

Get the number of annotations on the page. This is all annotation types, to get only the count of annotations of type `Widget`
use [fieldCount](#fieldcount)

### createAnnotation

```typescript
createAnnotation(type: NPDFAnnotation, rect: Rect): Annotation
```

Create a new [Annotation](./annotations.md). This annotation will be added to this page's annotations [Array](./array.md)

### createField

```typescript
createField(type: NPDFFieldType, annot: Annotation, form: Form, opts?: Object): Field
```

Create a new [Field](./field.md). This field  will be added to this page's annotations [Array](./array.md) and the [AcroForm](./form.md) automatically

### deleteField

```typescript
deleteField(index: number): void
```

Delete the field at the provided index

### flattenField

```typescript
/**
 * Flattening is the process of taking a fields appearance stream, appending that appearance stream
 * to the page, and then removing the field object and annotation widget, and scrubbing all references 
 * to the field from the document (scrub the page and acroform dictionary)
 */
flattenFields(): void
```

### getAnnotation

```typescript
getAnnotation(index: number): Annotation
```

Get the [Annotation](./annotations.md) at the provided index

### deleteAnnotation

```typescript
deleteAnnotation(index: number): void
```

Delete an [Annotation](./annotations.md) at the provided index