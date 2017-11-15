import {Painter} from './painter'
import * as test from 'tape'
import {join} from 'path';
import {Document} from './document';
import {CONVERSION} from "./index";


const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    doc = new Document(filePath)

doc.on('ready', e => {
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
        }, "/tmp/test.pdf")
    })

})

