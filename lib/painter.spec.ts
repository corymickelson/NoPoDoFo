import { Painter } from './painter'
import * as test from 'tape'
import { join } from 'path';
import { Document, FontEncoding } from './document';
import { CONVERSION } from "./index";
import { existsSync, unlinkSync } from "fs";
import { Rect } from "./rect";
import { Cell, Table } from "./table";


const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = '/tmp/painter.out.pdf',
    doc = new Document(filePath)

doc.on('ready', e => {
    test('draw text', t => {
        const page = doc.getPage(0),
            painter = new Painter(page),
            font = doc.createFont({ fontName: "monospace", encoding: FontEncoding.Identity })
        font.size = 24
        const metric = font.getMetrics()

        t.test('font size persists', fontSizeTest => {
            fontSizeTest.assert(metric.fontSize === 24, 'font size persists')
            fontSizeTest.end()
        })
        t.test('single line text test', singleLineTextTest => {
            let x, y
            x = CONVERSION * 10000
            y = page.height - 10000 * CONVERSION
            y -= metric.lineSpacing

            painter.drawText({ x, y }, "Test")
            painter.finishPage()
            doc.write((e, d) => {
                if (e) singleLineTextTest.fail(e.message)
                else singleLineTextTest.pass('need to add text extract to check, but if it did not throw that will have to work for now :)')
                singleLineTextTest.end()
            }, outFile)
        })
    })

})

function drawLine() {
    const page = doc.getPage(0),
        painter = new Painter(page),
        font = doc.createFont({ fontName: "monospace" }),
        metric = font.getMetrics()
    font.size = 16
    painter.font = font
    let x = 0, y, l, h, w, i
    let msg = "Grayscale - Colospace"
    h = metric.lineSpacing
    w = font.stringWidth(msg)
    y = page.height - 10000 * CONVERSION

    painter.drawText({ x: 12000 * CONVERSION, y: y - h }, msg)
    let rect = new Rect([12000 * CONVERSION, y - h, w, h])
    painter.rectangle(rect)
    painter.stroke()

    let lineLength = 50000 * CONVERSION

    for (let s = 0; s < 5; s++) {
        x += 10000 * CONVERSION
        painter.setStrokeWidth((s * 1000) * CONVERSION)
        painter.setStrokingGrey(s / 10.0)
        painter.drawLine({ x, y }, { x, y: y - lineLength })
    }

    painter.finishPage()
    test('write lines should not throw', t => {
        doc.write((e, d) => {
            if (e) t.fail(e.message)
            else t.pass('How to test if a line exists on a pdf???')
            t.end()
        }, outFile)
    })
}

function drawMultiLine() {
    const page = doc.getPage(0),
        painter = new Painter(page),
        font = doc.createFont({ fontName: "monospace" }),
        metric = font.getMetrics()
    font.size = 16
    painter.font = font
    let x = 0, y, l, h, w, i
    let msg = "Grayscale - Colospace"
    h = metric.lineSpacing
    w = font.stringWidth(msg)
    y = page.height - 10000 * CONVERSION

    painter.drawText({ x: 12000 * CONVERSION, y: y - h }, msg)
    let rect = new Rect([12000 * CONVERSION, y - h, w, h])
    painter.rectangle(rect)
    painter.stroke()

    let lineLength = 50000 * CONVERSION

    for (let s = 0; s < 5; s++) {
        x += 10000 * CONVERSION
        painter.setStrokeWidth((s * 1000) * CONVERSION)
        painter.setStrokingGrey(s / 10.0)
        painter.drawLine({ x, y }, { x, y: y - lineLength })
    }

    painter.finishPage()
    test('write lines should not throw', t => {
        doc.write((e, d) => {
            if (e instanceof Error) t.fail(e.message)
            else t.pass('How to test if a line exists on a pdf???')
            t.end()
        }, outFile)
    })
}

function drawSimpleTable() {
    const page = doc.getPage(0),
        painter = new Painter(page),
        font = doc.createFont({ fontName: "monospace", encoding: FontEncoding.Identity }),
        table = new Table(doc, 5, 5)
    painter.setColor([0.0, 0.0, 0.0])
    font.size = 16
    painter.font = font
    table.font = font
    table.foregroundColor = [0.0, 0.0, 0.0]
    // table.enableBackground(true)
    for (let c = 0; c < 5; c++) {
        for (let r = 0; r < 5; r++) {
            const cell = new Cell(table, c, r)
            // cell.text = `Column ${c}, row ${r}`
            cell.text = 'A'
        }
    }
    table.tableWidth = 200
    table.tableHeight = 200
    table.draw({ x: 300, y: 300 }, painter)
    // table.enableBackground(true)
    painter.finishPage()
    test('test table painter', t => {
        doc.write((e, d) => {
            if (e instanceof Error) t.fail(e.message)
            t.pass()
            t.end()
        }, '/tmp/table.pdf')
    })

}

function runTest(test: Function) {
    setImmediate(() => {
        global.gc()
        test()
        if (existsSync(outFile))
            unlinkSync(outFile)
    })
}
