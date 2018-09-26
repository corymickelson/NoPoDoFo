# API Documentation for SimpleTable

- [API Documentation for SimpleTable](#api-documentation-for-simpletable)
  - [NoPoDoFo SimpleTable](#nopodofo-simpletable)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [borderWidth](#borderwidth)
    - [foregroundColor](#foregroundcolor)
    - [backgroundColor](#backgroundcolor)
    - [alignment](#alignment)
    - [wordWrap](#wordwrap)
    - [tableWidth](#tablewidth)
    - [tableHeight](#tableheight)
    - [autoPageBreak](#autopagebreak)
  - [Methods](#methods)
    - [getText](#gettext)
    - [setText](#settext)
    - [getFont](#getfont)
    - [setFont](#setfont)
    - [getVerticalAlignment](#getverticalalignment)
    - [borderEnable](#borderenable)
    - [hasBorders](#hasborders)
    - [getImage](#getimage)
    - [hasImage](#hasimage)
    - [hasBackgroundColor](#hasbackgroundcolor)
    - [enableBackground](#enablebackground)
    - [getBorderColor](#getbordercolor)
    - [draw](#draw)
    - [columnCount](#columncount)
    - [rowCount](#rowcount)
    - [setColumnWidths](#setcolumnwidths)
    - [setColumnWidth](#setcolumnwidth)
    - [setRowHeight](#setrowheight)
    - [setRowHeights](#setrowheights)

## NoPoDoFo SimpleTable

The SimpleTable class is a wrapper around PoDoFo PdfSimpleTableModel. The PdfSimpleTableModel is described as:
An abstract interface of a model that can provide data and formatting information to a PdfTable.
The NoPoDoFo wrapper manages the underlying PdfTable for you. Please see the [Table Cookbook](./cookbook/table.md) for examples.

```typescript
class SimpleTable {
  new(doc:Base, cols:number, rows: number): SimpleTable
  borderWidth: number
  foregroundColor: Color
  backgroundColor: Color
  alignment: string
  wordWrap: boolean
  tableWidth: number
  tableHeight: number
  autoPageBreak: boolean
  getText(col:number, row:number): string
  setText(col: number, row: number, text: string): void
  getFont(col:number, row:number): Font
  setFont(font: Font): void
  getVerticalAlignment(col: number, row: number): string
  borderEnable(v: boolean): void
  hasBorders(): boolean
  getImage(col: number, row: number): Buffer
  hasImage(col: number, row: number): boolean
  hasBackgroundColor(col: number, row: number): boolean
  enableBackground(v: boolean): void
  getBorderColor(col: number, row: number): Color
  draw(point: NPDFPoint, painter: Painter): void
  columnCount(): number
  rowCount(): number
  setColumnWidths(n: number[]): void
  setColumnWidth(n: number): void
  setRowHeight(n: number): void
  setRowHeights(n: number[]): void
}
```

## Constructors
-------------------

```typescript
new(doc:Document, cols:number, rows: number): SimpleTable
```

Create a new table model with n columns(cols) and n rows(row)

## Properties
--------------

### borderWidth

Table border width as double

### foregroundColor

Table foreground color

### backgroundColor

Table background color

### alignment

The horizonal alignment of the contents in all table cells. Valid alignment values are: "LEFT", "CENTER", "RIGHT"

### wordWrap

Enable or disable word-wrapping

### tableWidth

Table width

### tableHeight

Table height

### autoPageBreak

Enable or disable automatic page breaking.
AutoPageBreak automatically creates a new page and continues drawing the table on the new page,
if there is not enough space on the current page. The newly created page will be set as the current page
on the painter used to draw and will be created using the same size as the old page.

## Methods
----------------

### getText

```typescript
getText(col:number, row:number): string
```

Get the text of the table cell at the column and row provided.

### setText

```typescript
setText(col: number, row: number, text: string): void
```

Set the text of the table cell at the column and row provided.

### getFont

```typescript
getFont(col:number, row:number): Font
```

Get the font of the table cell at the column and row provided.

### setFont

```typescript
setFont(font: Font): void
```

Set the font of the table cell at the column and row provided.

### getVerticalAlignment 

```typescript
getVerticalAlignment(col: number, row: number): string
```

Get the vertical alignment of the table cell at the column and row provided.

### borderEnable

```typescript
borderEnable(v: boolean): void
```

Enable or disable displaying the table's border

### hasBorders

```typescript
hasBorders(): boolean
```

Table has or has not a painted border

### getImage

```typescript
getImage(col: number, row: number): Buffer
```

Get the image of the table cell at the column and row provided

### hasImage

```typescript
hasImage(col: number, row: number): boolean
```

Boolean value; table has or has not an image at the column and row provided

### hasBackgroundColor

```typescript
hasBackgroundColor(col: number, row: number): boolean
```

Boolean value; the table cell at the column and row provided has background coloring enabled

### enableBackground

```typescript
enableBackground(v: boolean): void
```

Enable background coloring for the table cell at the column and row provided

### getBorderColor

```typescript
getBorderColor(col: number, row: number): Color
```

Get the border color for the table cell at the column and row provided

### draw

```typescript
draw(point: NPDFPoint, painter: Painter): void
```

Draw the table to the page at the NPDFPoint coordinates provided

### columnCount

```typescript
columnCount(): number
```

Get the number of columns in the table

### rowCount

```typescript
rowCount(): number
```

Get the number of rows in the table

### setColumnWidths

```typescript
setColumnWidths(n: number[]): void
```

Set the width of the columns (n)

### setColumnWidth

```typescript
setColumnWidth(n: number): void
```

Set the width of column n

### setRowHeight

```typescript
setRowHeight(n: number): void
```

Set the row hieght of row n

### setRowHeights

```typescript
setRowHeights(n: number[]): void
```

Set the row hieghts of rows n