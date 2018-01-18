import { Painter } from './painter'
import * as test from 'tape'
import { join } from 'path';
import { Document, FontEncoding } from './document';
import { CONVERSION } from "./index";
import { existsSync, unlinkSync } from "fs";
import { Rect } from "./rect";
import { Cell, Table } from "./table";
import { CONNREFUSED } from 'dns';


const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = '/tmp/painter.out.pdf',
    doc = new Document(filePath)

doc.on('ready', pdf => {
    if (pdf instanceof Error) throw Error("doc ready error")
    test('painter', t => {
        const page = pdf.getPage(0),
            painter = new Painter(doc, page),
            font = pdf.createFont({ fontName: 'monospace', encoding: FontEncoding.Identity })
        font.size = 16
        painter.font = font
        const metric = font.getMetrics()
        let x: number, y: number
        x = CONVERSION * 10000
        y = page.height - 10000 * CONVERSION
        y -= metric.lineSpacing
        t.doesNotThrow(() => painter.drawText({ x, y }, 'Test'))
        x = 0
        let l: number, h: number, w: number, i: number
        let msg = "Grayscale - Colospace"
        h = metric.lineSpacing
        w = font.stringWidth(msg)
        y = page.height - 10000 * CONVERSION

        t.doesNotThrow(() => painter.drawText({ x: 12000 * CONVERSION, y: y - h }, msg))
        let rect = new Rect([12000 * CONVERSION, y - h, w, h])
        painter.rectangle(rect)
        t.doesNotThrow(() => painter.stroke())

        let lineLength = 50000 * CONVERSION

        for (let s = 0; s < 5; s++) {
            x += 10000 * CONVERSION
            painter.setStrokeWidth((s * 1000) * CONVERSION)
            painter.setStrokingGrey(s / 10.0)
            t.doesNotThrow(() => painter.drawLine({ x, y }, { x, y: y - lineLength }))
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
        t.doesNotThrow(() => table.draw({ x: 300, y: 300 }, painter))

        painter.finishPage()
        pdf.write((e:Error, d:any) => {
            if(e instanceof Error) t.fail()
            t.end()
        }, outFile)
    })

})

