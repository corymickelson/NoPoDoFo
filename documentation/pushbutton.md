# API Documentation for PushButton

- [API Documentation for PushButton](#api-documentation-for-pushbutton)
  - [NoPoDoFo PushButton](#nopodofo-pushbutton)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [rollover](#rollover)
    - [rolloverAlternate](#rolloveralternate)
  - [Methods](#methods)

## NoPoDoFo PushButton

```typescript
class PushButton extends Field {
  new(page: Page, fieldIndex: number): PushButton
  new(form: Form, annotation: Annotation): PushButton

  rollover: string
  rolloverAlternate: string
}
```

## Constructors
-------------

```typescript
new(page: Page, fieldIndex: number): PushButton
```

```typescript
new(form: Form, annotation: Annotation): PushButton
```

## Properties
--------------

### rollover
Rollover text

### rolloverAlternate 
Rollover alternate text

## Methods
--------------