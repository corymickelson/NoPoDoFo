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

### required

### type

### fieldName

### alternateName

### mappingName

### exported

## Methods

### setBackgroundColor

### setBorderColor

### setHighlightingMode

### setMouseAction

### setPageAction