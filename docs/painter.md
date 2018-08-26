# API Documentation for Painter

* [Properties](#properties)
  * [tabWidth](#tabwidth)
  * [canvas](#canvas)
  * [font](#font)
  * [precision](#precision)
* [Methods](#methods)
  * [setPage](#setpage)
  * [setColor](#setcolor)
  * [setStrokeWidth](#setstrokewidth)
  * [setGrey](#setgrey)
  * [setStrokingGrey](#setstrokinggrey)
  * [setColorCMYK](#setcolorcmyk)
  * [setStrokingColorCMYK](#setstrokingcolorcmyk)
  * [setStrokeStyle](#setstrokestyle)
  * [setLineCapStyle](#setlinecapstyle)
  * [setLineJoinStyle](#setlinejoinstyle)
  * [setClipRect](#setcliprect)
  * [setMiterLimit](#setmiterlimit)
  * [rectangle](#rectangle)
  * [ellipse](#ellipse)
  * [circle](#circle)
  * [closePath](#closepath)
  * [lineTo](#lineto)
  * [moveTo](#moveto)
  * [cubicBezierTo](#cubicbezierto)
  * [horizontalLineTo](#horizontallineto)
  * [verticalLineTo](#verticallineto)
  * [smoothCurveTo](#smoothcurveto)
  * [quadCurveTo](#quadcurveto)
  * [arcTo](#arcto)
  * [close](#close)
  * [stroke](#stroke)
  * [fill](#fill)
  * [strokeAndFill](#strokeandfill)
  * [endPath](#endpath)
  * [clip](#clip)
  * [save](#save)
  * [restore](#restore)
  * [setExtGState](#setextgstate)
  * [getCurrentPath](#getcurrentpath)
  * [drawLine](#drawline)
  * [drawText](#drawtext)
  * [drawTextAligned](#drawtextaligned)
  * [drawMultiLineText](#drawmultilinetext)
  * [getMultiLineText](#getmultilinetext)
  * [bt](#bt)
  * [et](#et)
  * [addText](#addtext)
  * [moveTextPosition](#movetextposition)
  * [drawGlyph](#drawglyph)
  * [finishPage](#finishpage)
  * [drawImage](#drawimage)


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

## Properties

## Methods