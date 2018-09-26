# API Documentation for ComboBox

- [API Documentation for ComboBox](#api-documentation-for-combobox)
  - [NoPoDoFo ComboBox](#nopodofo-combobox)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [editable](#editable)
  - [Methods](#methods)

## NoPoDoFo ComboBox

```typescript
class ComboBox extends ListField {
  new(page: Page, fieldIndex: number): ComboBox
  new(form: Form, annotation: Annotation): ComboBox

  editable: boolean
}
```

## Constructors
---------------

Create a ComboBox from an existing object. The ComboBox [Annotation](./annotations.md) must already exist in the [Page](./page.md)'s
Annotations [Dictionary](./dictionary.md). This constructor will create the instance given the index of the [Annotation](./annotation.md)
in the [Page](./page.md)'s Annotations [Dictionary](./dictionary.md). If the index value is less than zero or greater than the length
of the [Page](./page.md)'s Annotations [Dictionary](./dictionary.md) (see `annotationsCount`) a Range Error will be thrown.

```typescript
new(page: Page, fieldIndex: number): ComboBox
```

Create a new ComboBox from the [Annotation](./annotations.md) and owned by the [Form](./form.md). The [Annotation](./annotations.md) must be
a `Widget` type annotation.

```typescript
new(form: Form, annotation: Annotation): ComboBox
```

## Properties
---------------

### editable

Toggle combobox editable, default false

## Methods
---------------