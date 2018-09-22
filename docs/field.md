# API Documentation for Field

- [API Documentation for Field](#api-documentation-for-field)
  - [NoPoDoFo Field](#nopodofo-field)
  - [Properties](#properties)
    - [readOnly](#readonly)
    - [required](#required)
    - [type](#type)
    - [fieldName](#fieldname)
    - [alternateName](#alternatename)
    - [mappingName](#mappingname)
    - [exported](#exported)
  - [Methods](#methods)
    - [setBackgroundColor](#setbackgroundcolor)
    - [setBorderColor](#setbordercolor)
    - [setHighlightingMode](#sethighlightingmode)
    - [setMouseAction](#setmouseaction)
    - [setPageAction](#setpageaction)

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

    setBackgroundColor(color: Color): void

    setBorderColor(color: Color): void

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
setBackgroundColor(color: Color): void
```
Set the background color as an [Color](./color.md).

### setBorderColor
```typescript
setBorderColor(color: Color): void
```
Set the bordercolor as an [Color](./color.md).

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