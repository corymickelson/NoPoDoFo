# API Documentation for Painter

- [API Documentation for Painter](#api-documentation-for-painter)
  - [NoPoDoFo Painter](#nopodofo-painter)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [tabWidth](#tabwidth)
    - [canvas](#canvas)
    - [font](#font)
    - [precision](#precision)
  - [Methods](#methods)
    - [setPage](#setpage)
    - [setColor](#setcolor)
    - [setStrokeWidth](#setstrokewidth)
    - [setGrey](#setgrey)
    - [setStrokingGrey](#setstrokinggrey)
    - [setColorCMYK](#setcolorcmyk)
    - [setStrokingColorCMYK](#setstrokingcolorcmyk)
    - [setStrokeStyle](#setstrokestyle)
    - [setLineCapStyle](#setlinecapstyle)
    - [setLineJoinStyle](#setlinejoinstyle)
    - [setClipRect](#setcliprect)
    - [setMiterLimit](#setmiterlimit)
    - [rectangle](#rectangle)
    - [ellipse](#ellipse)
    - [circle](#circle)
    - [closePath](#closepath)
    - [lineTo](#lineto)
    - [moveTo](#moveto)
    - [cubicBezierTo](#cubicbezierto)
    - [horizontalLineTo](#horizontallineto)
    - [verticalLineTo](#verticallineto)
    - [smoothCurveTo](#smoothcurveto)
    - [quadCurveTo](#quadcurveto)
    - [arcTo](#arcto)
    - [close](#close)
    - [stroke](#stroke)
    - [fill](#fill)
    - [strokeAndFill](#strokeandfill)
    - [endPath](#endpath)
    - [clip](#clip)
    - [save](#save)
    - [restore](#restore)
    - [setExtGState](#setextgstate)
    - [getCurrentPath](#getcurrentpath)
    - [drawLine](#drawline)
    - [drawText](#drawtext)
    - [drawTextAligned](#drawtextaligned)
    - [drawMultiLineText](#drawmultilinetext)
    - [getMultiLineText](#getmultilinetext)
    - [bt](#bt)
    - [et](#et)
    - [addText](#addtext)
    - [moveTextPosition](#movetextposition)
    - [drawGlyph](#drawglyph)
    - [finishPage](#finishpage)
    - [drawImage](#drawimage)


## NoPoDoFo Painter

```typescript
class Painter {
  new(doc: Base): Painter

  tabWidth: number
  readonly canvas: Stream
  font?: Font
  precision: number

  setPage(page: Page | XObject): void
  setColor(rgb: NPDFrgb): void
  setStrokeWidth(w: number): void
  setGrey(v: number): void
  setStrokingGrey(v: number): void
  setColorCMYK(cmyk: NPDFcmyk): void
  setStrokingColorCMYK(cmyk: NPDFcmyk): void
  setStrokeStyle(style: NPDFStokeStyle): void
  setLineCapStyle(style: NPDFLineCapStyle): void
  setLineJoinStyle(style: NPDFLineJoinStyle): void
  setClipRect(rect: Rect): void
  setMiterLimit(v: number): void
  rectangle(rect: Rect): void
  ellipse(points: NPDFPoint & { width: number, height: number }): void
  circle(points: NPDFPoint & { radius: number }): void
  closePath(): void
  lineTo(point: NPDFPoint): void
  moveTo(point: NPDFPoint): void
  cubicBezierTo(p1: NPDFPoint, p2: NPDFPoint, p3: NPDFPoint): void
  horizontalLineTo(v: number): void
  verticalLineTo(v: number): void
  smoothCurveTo(p1: NPDFPoint, p2: NPDFPoint): void
  quadCurveTo(p1: NPDFPoint, p2: NPDFPoint): void
  arcTo(p1: NPDFPoint, p2: NPDFPoint, rotation: number, large?: boolean, sweep?: boolean): void
  close(): void
  stroke(): void
  fill(): void
  strokeAndFill(): void
  endPath(): void
  clip(): void
  save(): void
  restore(): void
  setExtGState(state: ExtGState): void
  getCurrentPath(): string
  drawLine(p1: NPDFPoint, p2: NPDFPoint): void
  drawText(point: NPDFPoint, text: string): void
  drawTextAligned(point: NPDFPoint & { width: number }, text: string, alignment: NPDFAlignment): void
  drawMultiLineText(rect: Rect, value: string, alignment?: NPDFAlignment, vertical?: NPDFVerticalAlignment): void
  getMultiLineText(width: number, text: string, skipSpaces?: boolean): Array
  bt(point: NPDFPoint): void
  et(): void
  addText(text: string): void
  moveTextPosition(point: NPDFPoint): void
  drawGlyph(point: NPDFPoint, glyph: string): void
  finishPage(): void
  /**
   *
   * @param {Image} img - an instance of Image
   * @param {number} x - x coordinate (bottom left position of image)
   * @param {number} y - y coordinate (bottom position of image)
   * @param {{width:number, heigth:number}} scale - optional scaling
   */
  drawImage(img: Image, x: number, y: number, scale?: { width: number, height: number }): void

}
```

## Constructors
----------------

```typescript
new(doc: Base): Painter
```

Construct a new Painter object. To begin using the painter you must first set the page which the painter will apply painting/drawing operation on.

## Properties
---------------

### tabWidth

Set the tab width for the DrawText operation. Every tab '\t' is replaced with nTabWidth spaces before drawing text. Default is a value of 4

### canvas

Readonly property, get the page's canvas which the painter is to draw on.

### font

Get and set the [Font](./font.md) the painter will use to draw text.

### precision

Get and set the floating point precision for drawing operations.

## Methods
------------

### setPage

```typescript
setPage(page: Page | XObject): void
```

Set the a [Page](./page.md) or [XObject](./xobject.md) which the painter will draw on, paint/draw operations are applied to the contents [Object](./object.md).
[FinishPage](#finishpage) must be called to finalize any draw operations applied to the page/xobject.

### setColor

```typescript
setColor(rgb: NPDFrgb): void
```

Set the color as NPDFrgb to be used in all non-stroking operations, this color is also used for text drawn to the page.

### setStrokeWidth

```typescript
setStrokeWidth(w: number): void
```

Set the line width of stroking operations.

### setGrey
```typescript
setGrey(v: number): void
```

Set grayscale color value, this must be between 0 and 1.

### setStrokingGrey
```typescript
setStrokingGrey(v: number): void
```
Set the color for all following stroking operations in grayscale colorspace. This operation used the 'G' PDF operator.

### setColorCMYK
```typescript
setColorCMYK(cmyk: NPDFcmyk): void
```

Set the NPDFcmyk color value

### setStrokingColorCMYK
```typescript
setStrokingColorCMYK(cmyk: NPDFcmyk): void
```

Set stroking color NPDFcmyk value.

### setStrokeStyle
```typescript
setStrokeStyle(style: NPDFStokeStyle): void
```
Set the stoke style as one of NPDFStrokeStyle.

### setLineCapStyle
```typescript
setLineCapStyle(style: NPDFLineCapStyle): void
```

Set the line cap style as one of NPDFLineCapStyle for stroking operations.

### setLineJoinStyle
```typescript
setLineJoinStyle(style: NPDFLineJoinStyle): void
```

Set the line join style as one of NPDFLineJoinStyle for stroking operations.

### setClipRect

```typescript
setClipRect(rect: Rect): void
```
Set the graphic clipping rect.

### setMiterLimit

```typescript
setMiterLimit(v: number): void
```

Set the miter limit for stroking operations.

### rectangle
```typescript
rectangle(rect: Rect): void
```

Add a rectangle to te current path.

### ellipse

```typescript
ellipse(points: NPDFPoint & { width: number, height: number }): void
```

Add an ellipse to the current path.

### circle

```typescript
circle(points: NPDFPoint & { radius: number }): void
```

Add a circle to the current path.

### closePath

```typescript
closePath(): void
```

Close the current path.

### lineTo
```typescript
lineTo(point: NPDFPoint): void
```
Append a line segement to the current path.

### moveTo
```typescript
moveTo(point: NPDFPoint): void
```
Begin a new path.

### cubicBezierTo
```typescript
cubicBezierTo(p1: NPDFPoint, p2: NPDFPoint, p3: NPDFPoint): void
```
Append a cubic bezier to the current path

### horizontalLineTo
```typescript
horizontalLineTo(v: number): void
```

Append a horizontal line to the current path

### verticalLineTo
```typescript
verticalLineTo(v: number): void
```

Append a vertical line to the current path

### smoothCurveTo
```typescript
smoothCurveTo(p1: NPDFPoint, p2: NPDFPoint): void
```
Append a smooth curve to the current path

### quadCurveTo
```typescript
quadCurveTo(p1: NPDFPoint, p2: NPDFPoint): void
```
Append a quad curve to the current path

### arcTo
```typescript
arcTo(p1: NPDFPoint, p2: NPDFPoint, rotation: number, large?: boolean, sweep?: boolean): void
```
Append an arc to the current path

### close
```typescript
close(): void
```
Close the current path

### stroke
```typescript
stroke(): void
```
Stroke the current path.

### fill
```typescript
fill(): void
```
Fill the current path

### strokeAndFill
```typescript
strokeAndFill(): void
```
Stroke and fill the current path.

### endPath
```typescript
endPath(): void
```
End the current path without filling or stroking.

### clip
```typescript
clip(): void
```
Clip the current path.

### save
```typescript
save(): void
```
Save the current graphics settings onto the graphics stack.

### restore
```typescript
restore(): void
```
Restore graphics settings from the graphics stack.

### setExtGState
```typescript
setExtGState(state: ExtGState): void
```
Set active [ExtGState](./extgstate.md) object.

### getCurrentPath
```typescript
getCurrentPath(): string
```
Get the current path as a string

### drawLine
```typescript
drawLine(p1: NPDFPoint, p2: NPDFPoint): void
```
Draw a line from NPDFPoint p1 to NPDFPoint p2

### drawText
```typescript
drawText(point: NPDFPoint, text: string): void
```

Draw text at the coordinates defined by the NPDFPoint

### drawTextAligned
```typescript
drawTextAligned(point: NPDFPoint & { width: number }, text: string, alignment: NPDFAlignment): void
```

Draw text at the coordinates NPDFPoint, align text as one of NPDFAlignment. Alignment is measured by the width of the container, 
as such width is required.

### drawMultiLineText
```typescript
drawMultiLineText(rect: Rect, value: string, alignment?: NPDFAlignment, vertical?: NPDFVerticalAlignment): void
```
Draw multiline text segment. Text is aligned vertically as one of NPDFVerticalAlignment and horizontally as one of NPDFAlignment
within the [Rect](./rect.md) provided.

### getMultiLineText
```typescript
getMultiLineText(width: number, text: string, skipSpaces?: boolean): Array
```

Get multiline text

### bt
```typescript
bt(point: NPDFPoint): void
```

Begin text. The recommended method for adding simple text is [drawText](#drawtext), [drawMultiLineText](#drawmultilinetext), [drawTextAligned](#drawtextaligned).

### et
```typescript
et(): void
```

End text. The recommended method for adding simple text is [drawText](#drawtext), [drawMultiLineText](#drawmultilinetext), [drawTextAligned](#drawtextaligned).

### addText
```typescript
addText(text: string): void
```

Add text. The recommended method for adding simple text is [drawText](#drawtext), [drawMultiLineText](#drawmultilinetext), [drawTextAligned](#drawtextaligned).

### moveTextPosition
```typescript
moveTextPosition(point: NPDFPoint): void
```

Move position for text drawing, begin text [bt](#bt) must be called first. This method should only be used for advanced text positioning.
The recommended method for adding simple text is [drawText](#drawtext), [drawMultiLineText](#drawmultilinetext), [drawTextAligned](#drawtextaligned).

### drawGlyph
```typescript
drawGlyph(point: NPDFPoint, glyph: string): void
```

Draw a single glyph on a page using the set [Font](./font.md)

### finishPage
```typescript
finishPage(): void
```
Finish drawing on the page. This has to be called whenever drawing operations on the page are complete.

### drawImage
```typescript
drawImage(img: Image, x: number, y: number, scale?: { width: number, height: number }): void
```

Draw an image to the current path.