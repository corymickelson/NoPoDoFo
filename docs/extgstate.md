# API Documentation for ExtGState

* [Properties](#properties)
* [Methods](#methods)
  * [setFillOpacity](#setFillOpacity)
  * [setBlendMode](#setBlendMode)
  * [setOverprint](#setOverprint)
  * [setFillOverprint](#setFillOverprint)
  * [setStrokeOpacity](#setStrokeOpacity)
  * [setNoneZeroOverprint](#setNoneZeroOverprint)
  * [setRenderingIntent](#setRenderingIntent)
  * [setFrequency](#setFrequency)

## NoPoDoFo ExtGState
The ExtGState, external graphics state, is primarily responsible for transparency. The simplist type being the level of opacity
of an object, where 1 is completely opaque 0 is invisible.

```typescript
class ExtGState {
  new(doc: Base): ExtGState
  setFillOpacity(v: number): void
  setBlendMode(mode: NPDFBlendMode): void
  setOverprint(v: boolean): void
  setFillOverprint(v: boolean): void
  setStrokeOpacity(v: number): void
  setStrokeOverprint(v: boolean): void
  setNonZeroOverprint(v: boolean): void
  setRenderingIntent(intent: NPDFRenderingIntent): void
  setFrequency(v: number): void
}
```
## Constructors
```typescript
new(doc: Base): ExtGState
```
Create a new ExtGState object owned by the [Document](./document.md) provided. An external graphics state object can be utilized by a [Pages](./page.md)
resource dictionary. Setting this value is accomplished by [Painter.SetExtGState](./painter.md#setextgstate.md).

## Properties

## Methods

### setFillOpacity
```typescript
setFillOpacity(v: number): void
```
Sets the fill opacity, this number must be between 0 and 1

### setBlendMode
```typescript
setBlendMode(mode: NPDFBlendMode): void
```
Set the value as one of NPDFBlendMode

### setOverprint
```typescript
setOverprint(v: boolean): void
```
Enable overprinting for both fill and stoke.

### setFillOverprint
```typescript
setFillOverprint(v: boolean): void
```
Enable/disable fill overprinting.

### setStrokeOpacity
```typescript
setStrokeOpacity(v: number): void
```
Set stroke opacity, this number must be between 0 and 1

### setStrokeOverprint
```typescript
setStrokeOverprint(v: boolean): void
```
Enable/disable stroke overprinting.

### setNonZeroOverprint
```typescript
setNonZeroOverprint(v: boolean): void
```
Enable/disable non zero overprint mode.

### setRenderingIntent
```typescript
setRenderingIntent(intent: NPDFRenderingIntent): void
```
Set the rendering intent as NPDFRenderingIntent.

### setFrequency
```typescript
setFrequency(v: number): void
```
Set the frequency for halftones.