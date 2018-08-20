# API Documentation for ListField

* [Properties](#properties)
  * [selected](#selected)
  * [length](#length)
  * [spellCheckEnabled](#spellCheckEnabled)
  * [sorted](#sorted)
  * [multiSelect](#multiselect)
* [Methods](#methods)
  * [isComboBox](#isComboBox)
  * [insertItem](#insertItem)
  * [removeItem](#removeItem)
  * [getItem](#getItem)

## NoPoDoFo ListField
ListField is the base class to list type acroform fields, including [ComboBox](./combobox.md) and [ListBox](./listbox.md).
Classes derived from ListField present users with multiple options. A user may select one or more options from fields derived
from ListField.

```typescript
abstract class ListField extends Field {
  selected: number
  readonly length: number
  spellCheckEnabled: boolean
  sorted: boolean
  multiSelect: boolean

  isComboBox(): boolean
  insertItem(value: string, displayName: string): void
  removeItem(index: number): void
  getItem(index: number): ListItem
}
```

## Properties

### selected

### length
Readonly property, returns the number of options in the field.

### spellCheckEnabled

### sorted

### multiSelect

## Methods

### isComboBox

### insertItem

### removeItem

### getItem