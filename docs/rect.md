# API Documentation for Rect

- [API Documentation for Rect](#api-documentation-for-rect)
  - [NoPoDoFo Rect](#nopodofo-rect)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [left](#left)
    - [bottom](#bottom)
    - [width](#width)
    - [height](#height)
  - [Methods](#methods)
    - [intersect](#intersect)

## NoPoDoFo Rect

A Rect (rectangle) represents coordinates (x, y), and width and height of a section on a [Page](./page.md) 

```typescript
class Rect {
  new(): Rect
  new(left: number, bottom: number, width: number, height: number): Rect

  left: number
  bottom: number
  width: number
  height: number
  intersect(rect: Rect): void
}
```

## Constructors
---------------

```typescript
new(): Rect
```

Create a new instance of a NoPoDoFo Rect with left, bottom, width and height set to 0

```typescript
new(left: number, bottom: number, width: number, height: number): Rect
```

Create a new instance of a NoPoDoFo Rect with values provided in constructor.

## Properties
----------------

### left

Rectable left value

### bottom

Rectangle bottom value

### width

Rectangle width

### height

Rectangle height

## Methods
----------

### intersect

```typescript
intersect(rect: Rect): void
```

Intersect with another Rect.