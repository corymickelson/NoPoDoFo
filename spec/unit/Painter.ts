import {AsyncTest, Expect, Test, TestFixture} from 'alsatian'
import {nopodofo as npdf, CONVERSION, NPDFFontEncoding} from '../../'
import {join} from 'path';

if (!global.gc) {
    global.gc = () => {
    }
}

@TestFixture('Painter')
export class PainterSpec {
    private readonly filePath = join(__dirname, '../test-documents/test.pdf')
    private readonly outFile = '/tmp/painter.out.pdf'

    @Test('Stream Painter setPage')
    public streamPainterSetPageTest() {
        const doc = new npdf.StreamDocument(join(__dirname, '../tmp/streamPainterSetPage.pdf'))
        const painter = new npdf.Painter(doc)
        const page = doc.createPage(new npdf.Rect(0, 0, 612, 792))
        // Expect(() => painter.addText('TEST')).toThrow()
        painter.setPage(page)
        // painter.addText('TEST')
    }

    @Test('Paint Simple Table')
    public simpleTablePainterTest() {
        const doc = new npdf.StreamDocument()
        const painter = new npdf.Painter(doc)
        const page = doc.createPage(new npdf.Rect(0, 0, 612, 792))
        painter.setPage(page)
        const font = doc.createFont({fontName: 'Carlito', embed: true})
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
        table.draw({x: 300, y: 300}, painter)
        painter.finishPage()
        let output = doc.close()
        Expect(Buffer.isBuffer(output)).toBeTruthy()
    }

    @AsyncTest('Painter drawing apis')
    public async painter() {
        let doc = new npdf.Document()
        return new Promise(resolve => {
            doc.load(this.filePath, (err: Error) => {
                if (err) Expect.fail(err.message)
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
                    painter.drawText({x, y}, 'SingleLineText')
                    x = 0
                    let l: number, h: number, w: number, i: number
                    let msg = "Grayscale - Colospace"
                    h = metric.lineSpacing
                    w = font.stringWidth(msg)
                    y = page.height - 10000 * CONVERSION

                    painter.drawText({x: 12000 * CONVERSION, y: y - h}, msg)
                    let rect = new npdf.Rect(12000 * CONVERSION, y - h, w, h)
                    painter.rectangle(rect)
                    painter.stroke()

                    let lineLength = 50000 * CONVERSION

                    for (let s = 0; s < 5; s++) {
                        x += 10000 * CONVERSION
                        painter.setStrokeWidth((s * 1000) * CONVERSION)
                        painter.setStrokingGrey(s / 10.0)
                        painter.drawLine({x, y}, {x, y: y - lineLength})
                    }

                    const table = new npdf.SimpleTable(doc, 5, 5)
                    table.setFont(font)
                    for (let c = 0; c < 5; c++) {
                        for (let r = 0; r < 5; r++) {
                            table.setText(c, r, 'A')
                        }
                    }
                    table.tableWidth = 200
                    table.tableHeight = 200
                    table.draw({x: 300, y: 300}, painter)

                    let multiLineContainer = new npdf.Rect(300, 300, 150, 50)
                    painter.drawMultiLineText(multiLineContainer, 'MULTILINE\nTEST')
                    painter.finishPage()
                    doc.write(this.outFile, (e: Error, d: any) => {
                        if (e instanceof Error) Expect.fail(e.message)
                        else {
                            let subject = new npdf.Document()
                            subject.load(this.outFile, e => {
                                if (e) Expect.fail(e.message)
                                else {
                                    let contentsParser = new npdf.ContentsTokenizer(doc, 0)
                                    let contents = contentsParser.readSync();
                                    let it = contents.next()
                                    let expects = ['MULTILINE', 'TEST', 'SingleLineText']
                                    let found = 0
                                    while (it.done === false) {
                                        for (let i = 0; i < expects.length; i++) {
                                            if (it.value.includes(expects[i])) {
                                                found++
                                            }
                                        }
                                        it = contents.next()
                                    }
                                    Expect(found).toBe(3)
                                    return resolve()
                                }
                            })
                        }
                    })
                }
            })
        })
    }
}