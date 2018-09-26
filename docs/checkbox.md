# API Documentation for CheckBox

- [API Documentation for CheckBox](#api-documentation-for-checkbox)
  - [NoPoDoFo CheckBox](#nopodofo-checkbox)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [checked](#checked)

## NoPoDoFo CheckBox

A Checkbox is a field that toggles between two states, open and closed. The oopen(on) and closed(off) state of the field has a direct correlation to the
on and off states of the `AP` (appearance stream) of the [Annotation](./annotations.).

```typescript
class CheckBox extends Field {
  new(page: Page, fieldIndex: number): CheckBox
  new(form: Form, annotation: Annotation): CheckBox

  checked: boolean
}
```

## Constructors
--------------

Construct a CheckBox from an existing object. The CheckBox [Annotation](./annotations.md) must already exist in the [Page](./page.md)'s
Annotations [Dictionary](./dictionary.md). This constructor will create the instance given the index of the [Annotation](./annotation.md)
in the [Page](./page.md)'s Annotations [Dictionary](./dictionary.md). If the index value is less than zero or greater than the length
of the [Page](./page.md)'s Annotations [Dictionary](./dictionary.md) (see `annotationsCount`) a Range Error will be thrown.

```typescript
new(page: Page, fieldIndex: number): CheckBox
```

Create a new CheckBox from an [Annotation](./annotations.md) and owned by the [Form](./form.md). The [Annotation](./annotations.md) must be
a `Widget` type annotation.

```typescript
new(form: Form, annotation: Annotation): CheckBox
```

## Properties
--------------

### checked
Toggle the `AS` property of the checkbox annotations dictionary; `AS` values maybe one of `ON` or `OFF`.