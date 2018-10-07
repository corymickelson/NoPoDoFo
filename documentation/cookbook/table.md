# Table Cookbook

## Create a Table

```typescript
import {nopodofo as npdf} from 'nopodofo'
const doc = new npdf.StreamDocument()
const page = doc.createPage(new npdf.Rect(0, 0, 612, 792))
const painter = new npdf.Painter(doc)
painter.setPage(page)
const mono = doc.createFont({fontName: 'monospace'})
const cols = 10, rows = 10
const table = new npdf.SimpleTable(doc, cols, rows)
table.setFont(mono)
for(let i = 0; i < cols, i++) {
  for(let ii = 0; ii < rows; ii++) {
    table.setText(i, ii, 'Table Cell Text Goes Here!')
  }
}
table.tableWidth = 300
table.tableHeight = 300
table.draw({x: 500, y: 500}, painter)
painter.finishPage()

```

## Apply Table styling

```typescript
const doc = new npdf.StreamDocument()
const painter = new npdf.Painter(doc)
const page = doc.createPage(new npdf.Rect(0, 0, 612, 792))
painter.setPage(page)
const font = doc.createFont({ fontName: 'Carlito', embed: true })
painter.font = font
const table = new npdf.SimpleTable(doc, 5, 5)
const purple = new npdf.Color(0.21, 0.15, 0.34)
const red = new npdf.Color(1.0, 0.0, 0.0)
table.setFont(font)
table.foregroundColor = purple 
table.backgroundColor = red
table.borderEnable(true)
table.enableBackground(true)
table.borderWidth = 3
table.wordWrap = true
for (let c = 0; c < 5; c++) {
    for (let r = 0; r < 5; r++) {
        table.setText(c, r, 'AAAAAAA')
    }
}
table.tableWidth = 200
table.tableHeight = 200
t.doesNotThrow(() => table.draw({ x: 300, y: 300 }, painter))
painter.finishPage()
```