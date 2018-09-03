# API Documentation for Rect

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

```typescript
new(): Rect
```
```typescript
new(left: number, bottom: number, width: number, height: number): Rect
```

## Properties

### left

Rectable left value

### bottom

Rectangle bottom value

### width

Rectangle width

### height

Rectangle height

## Methods

### intersect