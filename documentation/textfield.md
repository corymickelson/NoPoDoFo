# API Documentation for TextField

- [API Documentation for TextField](#api-documentation-for-textfield)
  - [NoPoDoFo TextField](#nopodofo-textfield)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [text](#text)
    - [maxLen](#maxlen)
    - [multiLine](#multiline)
    - [passwordField](#passwordfield)
    - [fileField](#filefield)
    - [spellCheckEnabled](#spellcheckenabled)
    - [scrollEnabled](#scrollenabled)
    - [combs](#combs)
    - [richText](#richtext)
  - [Methods](#methods)

## NoPoDoFo TextField

A text field widget type [Annotation](./annotations.md). This field is responsible for handling user text input.
This class is capable of handling a variety of text type inputs including: password fields, multiline text, and rich text.
TextField is derived from the [Field](./field.md) class. Please see the [Appearance Stream Cookbook](./cookbook/ap.md) for
examples of how to set the appearance stream for a text field.

```typescript
class TextField extends Field {
  constructor(page: Page, fieldIndex: number)
  constructor(page: Page, annotation: Annotation, form: Form, opts?: Object)

  text: string
  maxLen: number
  multiLine: boolean
  passwordField: boolean
  fileField: boolean
  spellCheckEnabled: boolean
  scrollEnabled: boolean
  combs: boolean
  richText: boolean
}
```

## Constructors
---------------

```typescript
constructor(page: Page, fieldIndex: number)
```

Create a instance from an existing TextField

```typescript
constructor(page: Page, annotation: Annotation, form: Form, opts?: Object)
```

Create a new instance of a TextField

## Properties
-------------

### text

Get and set the text value of the text field

### maxLen

Define a maximum length limit for the [text](#text) property

### multiLine

Toggle multiline option

### passwordField

Toggle password field text masking option

### fileField

Toggle file field option. A file field describes the path to a file

### spellCheckEnabled

Toggle spell checking of the [text](#text) property

### scrollEnabled

Enable scrolling for overflow text

### combs

Toggle combs

### richText

Toggle rich text

## Methods
---------------