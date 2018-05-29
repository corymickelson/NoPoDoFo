import {npdf, CONVERSION, IPainter, NPDFFontEncoding, Cell, Table} from '../../dist'
import * as tap from 'tape'
import {join} from 'path';

const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = '/tmp/painter.out.pdf',
    doc = new npdf.Document()

tap('IPainter', t => {
    t.test('Document [MemDocument]', t => {
        doc.load(filePath, (err: Error) => {
            if (err) t.fail(err.message)
            else {
                let painter = new npdf.Painter(doc)
                let page = doc.getPage(0)
                painter.setPage(page)
                let font = doc.createFont({
                    fontName: 'monospace',
                    bold: false,
                    embed: false,
                    encoding: NPDFFontEncoding.WinAnsi,
                    italic: true
                })
                painter.font = font
                const metric = font.getMetrics()
                let x: number, y: number
                x = CONVERSION * 10000
                y = page.height - 10000 * CONVERSION
                y -= metric.lineSpacing
                t.doesNotThrow(() => painter.drawText({x, y}, 'Test'))
                x = 0
                let l: number, h: number, w: number, i: number
                let msg = "Grayscale - Colospace"
                h = metric.lineSpacing
                w = font.stringWidth(msg)
                y = page.height - 10000 * CONVERSION

                t.doesNotThrow(() => painter.drawText({x: 12000 * CONVERSION, y: y - h}, msg))
                let rect = new npdf.Rect(12000 * CONVERSION, y - h, w, h)
                painter.rectangle(rect)
                t.doesNotThrow(() => painter.stroke())

                let lineLength = 50000 * CONVERSION

                for (let s = 0; s < 5; s++) {
                    x += 10000 * CONVERSION
                    painter.setStrokeWidth((s * 1000) * CONVERSION)
                    painter.setStrokingGrey(s / 10.0)
                    t.doesNotThrow(() => painter.drawLine({x, y}, {x, y: y - lineLength}))
                }

                const table = new Table(doc, 5, 5)
                painter.setColor([0.0, 0.0, 0.0])
                table.font = font
                table.foregroundColor = [0.0, 0.0, 0.0]
                for (let c = 0; c < 5; c++) {
                    for (let r = 0; r < 5; r++) {
                        const cell = new Cell(table, c, r)
                        cell.text = 'A'
                    }
                }
                table.tableWidth = 200
                table.tableHeight = 200
                t.doesNotThrow(() => table.draw({x: 300, y: 300}, painter))

                painter.finishPage()
                doc.write(outFile, (e: Error, d: any) => {
                    if (e instanceof Error) t.fail()
                    t.end()
                })
            }
        })
    })

})
// tap('IPainter', sub => {
//     const filePath = join(__dirname, '../test-documents/test.pdf'),
//         outFile = '/tmp/painter.out.pdf',
//         doc = new npdf.Document()
//
//
//     doc.on('ready', pdf => {
//         if (pdf instanceof Error) throw Error("doc ready error")
//         const page = pdf.getPage(0),
//             painter = new Painter(doc, page),
//             font = pdf.createFont({fontName: 'monospace', encoding: NPDFFontEncoding.Identity})
//
//         // TODO: Add correct assertions, will require parsing final doc for contents / resources objects
//         sub.test('paint', t => {
//
//             font.size = 16
//             painter.font = font
//             const metric = font.getMetrics()
//             let x: number, y: number
//             x = CONVERSION * 10000
//             y = page.height - 10000 * CONVERSION
//             y -= metric.lineSpacing
//             t.doesNotThrow(() => painter.drawText({x, y}, 'Test'))
//             x = 0
//             let l: number, h: number, w: number, i: number
//             let msg = "Grayscale - Colospace"
//             h = metric.lineSpacing
//             w = font.stringWidth(msg)
//             y = page.height - 10000 * CONVERSION
//
//             t.doesNotThrow(() => painter.drawText({x: 12000 * CONVERSION, y: y - h}, msg))
//             let rect = new Rect([12000 * CONVERSION, y - h, w, h])
//             painter.rectangle(rect)
//             t.doesNotThrow(() => painter.stroke())
//
//             let lineLength = 50000 * CONVERSION
//
//             for (let s = 0; s < 5; s++) {
//                 x += 10000 * CONVERSION
//                 painter.setStrokeWidth((s * 1000) * CONVERSION)
//                 painter.setStrokingGrey(s / 10.0)
//                 t.doesNotThrow(() => painter.drawLine({x, y}, {x, y: y - lineLength}))
//             }
//
//             const table = new Table(doc, 5, 5)
//             painter.setColor([0.0, 0.0, 0.0])
//             table.font = font
//             table.foregroundColor = [0.0, 0.0, 0.0]
//             for (let c = 0; c < 5; c++) {
//                 for (let r = 0; r < 5; r++) {
//                     const cell = new Cell(table, c, r)
//                     cell.text = 'A'
//                 }
//             }
//             table.tableWidth = 200
//             table.tableHeight = 200
//             t.doesNotThrow(() => table.draw({x: 300, y: 300}, painter))
//
//             painter.finishPage()
//             pdf.write(outFile, (e: Error, d: any) => {
//                 if (e instanceof Error) t.fail()
//                 t.end()
//             })
//         })
//
//     })
//
// })
