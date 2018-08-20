# API Documentation for CheckBox

* [Properties](#properties)
  * [checked](#checked)
* [Methods](#methods)

## NoPoDoFo CheckBox
```typescript
class CheckBox extends Field {
  new(page: Page, fieldIndex: number): CheckBox
  new(form: Form, annotation: Annotation): CheckBox

  checked: boolean
}
```

## Constructors
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

### checked
Checked is a getter / setter property for the `` value of this field.