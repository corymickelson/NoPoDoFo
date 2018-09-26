# API Documentation for ListBox

- [API Documentation for ListBox](#api-documentation-for-listbox)
  - [NoPoDoFo ListBox](#nopodofo-listbox)
  - [Constructors](#constructors)
  - [Properties](#properties)
  - [Methods](#methods)

## NoPoDoFo ListBox

ListBox is derived from [ListField](listfield.md)

```typescript
class ListBox extends ListField {
  new(page: Page, fieldIndex: number): ListBox
  new(form: Form, annotation: Annotation): ListBox
}
```

## Constructors
------------

```typescript
new(page: Page, fieldIndex: number): ListBox
```

Copy constructor, used by [Page.getField](./page.md#getfield)

```typescript
new(form: Form, annotation: Annotation): ListBox
```

Create a new ListBox field, to be added to the provided [Form](./form.md), the [Annotation](./annotations.md) must be of type `Widget`

## Properties
--------------

## Methods
-------------