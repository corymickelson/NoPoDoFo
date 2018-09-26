# API Documentation for Form

- [API Documentation for Form](#api-documentation-for-form)
  - [NoPoDoFo Form](#nopodofo-form)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [needAppearances](#needappearances)
    - [dictionary](#dictionary)
    - [DA](#da)
    - [DR](#dr)
    - [CO](#co)
    - [SigFlags](#sigflags)
  - [Methods](#methods)

## NoPoDoFo Form

A Form or AcroForm is a [Dictionary](./dictionary.md) containing all the [Annotation](./annotations.md) for the [Document](./document.md).
NoPoDoFo supports creating, modifying, and filling interactive forms (AcroForm). 
To access a documents acroform use the `Document.form` accessor. 
You can use the acroform's dictionary to access the fields array, but the preferred way is through the page.
All annotations are indexed and easily available via `getField`, `getFields`, `getAnnotation` and `getAnnotations`.  
Please see the [spec](https://www.adobe.com/content/dam/acom/en/devnet/pdf/pdfs/PDF32000_2008.pdf) section 12.7 for more information

```typescript
class Form {
  needAppearances: boolean
  readonly dictionary: Dictionary
  DA?: string
  DR?: Dictionary
  CO?: Dictionary
  SigFlags?: NPDFSigFlags
}
```

## Constructors
----------------

## Properties
-----------------

### needAppearances
Enable or disable the use of the default appearance stream. 
An acroforms default appearance stream is 12pt black arial. You can override the defaults on the annotation directly.

### dictionary
Get the underlying [Dictionary](./dictionary.md)

### DA
Get or set the default appearance value. The DA value is used for formatting the text annotation values. Note
the `AP` key if present will take precedence over the DA value.

### DR
The `DR` value is the resource [Dictionary](./dictionary.md) for the form, this dictionary contains the default resources used
by the form [Field](./field.md) [Annotations](./annotations.md).

### CO
The `CO` [Dictionary](./dictionary.md) contains additional [Actions](./action.md) defined by [Annotations](./annotations.md). An 
action can be a calculation or re-calculation of a [Fields](./field.md) value or a Javascript script to execute when the annotations
action is triggered.

### SigFlags
Get or set SigFlags flags as one of NPDFSigFlags.

## Methods
---------------