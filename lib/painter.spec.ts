import {Painter} from './painter'
import * as test from 'tape'
import {join} from 'path';
import {Document} from './document';
import {CONVERSION} from "./index";
import {existsSync, unlinkSync} from "fs";
import {Rect} from "./rect";


const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    doc = new Document(filePath)

doc.on('ready', e => {
    runAll()
})

function drawText() {
    const page = doc.getPage(0),
        painter = new Painter(page),
        font = doc.createFont({fontName: "Cantarell"})
    font.size = 24

    let metric = font.getMetrics()
    test('font size persists', t => {
        t.assert(metric.fontSize === 24, 'font size persists')
        t.end()
    })

    painter.setColor([0.0, 0.0, 0.0])
    painter.font = font


    test('draw text', t => {
        let x, y
        x = CONVERSION * 10000
        y = page.height - 10000 * CONVERSION
        y -= metric.lineSpacing

        painter.drawText({x, y}, "Test")
        painter.finishPage()
        doc.write((e, d) => {
            if (e) t.fail(e.message)
            else t.pass('need to add text extract to check, but if it did not throw that will have to work for now :)')
            t.end()
        }, outFile)
    })
}

function drawLine() {
    const page = doc.getPage(0),
        painter = new Painter(page),
        font = doc.createFont({fontName: "Cantarell"}),
        metric = font.getMetrics()
    font.size = 16
    painter.font = font
    let x=0, y, l, h, w, i
    let msg = "Grayscale - Colospace"
    h = metric.lineSpacing
    w = font.stringWidth(msg)
    y = page.height - 10000 * CONVERSION

    painter.drawText({x: 12000 * CONVERSION, y: y - h}, msg)
    let rect = new Rect([12000 * CONVERSION, y - h, w, h])
    painter.rectangle(rect)
    painter.stroke()

    let lineLength = 50000 * CONVERSION

    for (let s = 0; s < 5; s++) {
        x += 10000 * CONVERSION
        painter.setStrokeWidth((s*1000) * CONVERSION)
        painter.setStrokingGrey(s * 10.0)
        painter.drawLine({x,y}, {x,y: y - lineLength})
    }
}

function runTest(test: Function) {
    setImmediate(() => {
        global.gc()
        test()
        if (existsSync(outFile))
            unlinkSync(outFile)
    })
}

export function runAll() {
    [drawText].map(i => runTest(i))
}
