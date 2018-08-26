# API Documentation for Field

* [Properties](#properties)
  * [readOnly](#readonly)
  * [required](#required)
  * [type](#type)
  * [fieldName](#fieldname)
  * [alternateName](#alternateName)
  * [mappingName](#mappingName)
  * [exported](#exported)
* [Methods](#methods)
  * [setBackgroundColor](#setBackgroundColor)
  * [setBorderColor](#setBorderColor)
  * [setHighlightingMode](#setHighlightingMode)
  * [setMouseAction](#setMouseAction)
  * [setPageAction](#setPageAction)

## NoPoDoFo Field
All acroform fields with the exception of SignatureField inherit Field.
```typescript
  abstract class Field {
    readOnly: boolean
    required: boolean
    readonly type: NPDFFieldType
    fieldName: string
    alternateName?: string
    mappingName?: string
    exported?: boolean

    setBackgroundColor(color: NPDFColor): void

    setBorderColor(color: NPDFColor): void

    setHighlightingMode(mode: NPDFHighlightingMode): void

    setMouseAction(on: NPDFMouseEvent, action: Action): void

    setPageAction(on: NPDFPageEvent, action: Action): void
  }

```

## Properties

### readOnly
Enable/Disable readonly property. If set the user may not change the value of the field.

### required
Enable/Disable required property. If set the field must have a value at the time it is exported by a SubmitForm action.

### type
Reaonly property, returns the value as one of NPDFFieldType.

### fieldName
Get or set the name of the field. A fieldName is not required.

### alternateName
Get or set an alternate name for the field.

### mappingName
Get or set a mapping name; a mapping name is used when exporting the fields data.

### exported
Enable/Disable exporting.

## Methods

### setBackgroundColor
```typescript
setBackgroundColor(color: NPDFColor): void
```
Set the background color as an NPDFColor.

### setBorderColor
```typescript
setBorderColor(color: NPDFColor): void
```
Set the bordercolor as an NPDFColor.

### setHighlightingMode
```typescript
setHighlightingMode(mode: NPDFHighlightingMode): void
```
Set highlighting mode as one of NPDFHighlightingMode

### setMouseAction
```typescript
setMouseAction(on: NPDFMouseEvent, action: Action): void
```

### setPageAction
```typescript
setPageAction(on: NPDFPageEvent, action: Action): void
```