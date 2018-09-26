# API Documentation for ListField

- [API Documentation for ListField](#api-documentation-for-listfield)
  - [NoPoDoFo ListField](#nopodofo-listfield)
  - [Properties](#properties)
    - [selected](#selected)
    - [length](#length)
    - [spellCheckEnabled](#spellcheckenabled)
    - [sorted](#sorted)
    - [multiSelect](#multiselect)
  - [Methods](#methods)
    - [isComboBox](#iscombobox)
    - [insertItem](#insertitem)
    - [removeItem](#removeitem)
    - [getItem](#getitem)

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
----------------

### selected
Get or set the selected list item.

Get or set the selected list item.

### length

Readonly property, returns the number of options in the field.

### spellCheckEnabled
Enable or disable spellchecking

Enable or disable spellchecking

### sorted
Enable of disable sorting

Enable of disable sorting

### multiSelect
Enable or sidable multiselect

Enable or sidable multiselect

## Methods
---------------

### isComboBox

```typescript
isComboBox(): boolean
```

If derived field is a ComboBox return true, else false

```typescript
isComboBox(): boolean
```

If derived field is a ComboBox return true, else false

### insertItem
```typescript
insertItem(value: string, displayName: string): void
```
Add an item to the list of values

```typescript
insertItem(value: string, displayName: string): void
```

Add an item to the list of values

### removeItem
```typescript
removeItem(index: number): void
```

Remove an item from the list

```typescript
removeItem(index: number): void
```

Remove an item from the list

### getItem

```typescript
getItem(index: number): ListItem
```

Get an item from the list as a ListItem
