import {nopodofo, NPDFAlignment, NPDFVerticalAlignment} from '../../'
import {TestFixture, AsyncTest, Expect, Timeout} from 'alsatian';
import {NDocument} from '../../lib/NDocument';
import {join} from 'path'
import Painter = nopodofo.Painter;


@TestFixture('Bug89-RSS')
export class Bug89 {
    static async generate() {
        const n = await NDocument.from(join(__dirname, '../test-documents/bug89_doc.pdf'))
        const cover = await NDocument.from(join(__dirname, '../test-documents/bug89_cover.pdf'))
        const page = n.getPage(0)
        const painter = new Painter(n.memory as nopodofo.Document)
        const carlito = n.createFont({
            fontName: 'Carlito',
            fileName: join(__dirname, '../test-documents/Carlito-Regular.ttf'),
            embed: true
        })
        painter.setPage(page)
        painter.font = carlito
        for (let i = 0; i < 10; i++) {
            painter.drawText({x: i * 10, y: i * 10}, `TEST ${i}`)
        }
        painter.finishPage()
        n.insertExistingPage(cover.memory as nopodofo.Document, 0, 0)
        await new Promise(resolve => n.write((err, data) => {
            if (err) Expect.fail(err.message)
            Expect(Buffer.isBuffer(data))
            return resolve()
        }))
    }

    @AsyncTest('RSS memory leak')
    @Timeout(50000)
    public async rssMemoryLeak() {
        let lastRSS
        try {
            for (let i = 0; i < 100; i++) {
                await Bug89.generate()
                if (global.hasOwnProperty('gc')) {
                    global.gc()
                }
                const used = process.memoryUsage()
                for (let key in used) {
                    console.log(`Memory usage: ${key} ${Math.round((used as any)[key as string] / 1024 / 1024 * 100) / 100} MB`)
                }
            }
        } catch (e) {
            Expect.fail(e.message)
        }
    }
}

async function pdf() {
    const n = await NDocument.from(join(__dirname, '../test-documents/bug89_doc.pdf'))
    const cover = await NDocument.from(join(__dirname, '../test-documents/bug89_cover.pdf'))
    const page = n.getPage(0)
    const painter = new Painter(n.memory as nopodofo.Document)
    const carlito = n.createFont({
        fontName: 'Carlito',
        fileName: join(__dirname, '../test-documents/Carlito-Regular.ttf'),
        embed: true
    })
    painter.setPage(page)
    painter.font = carlito
    for (let i = 0; i < 10; i++) {
        painter.drawText({x: i * 10, y: i * 10}, `TEST ${i}`)
    }
    painter.finishPage()
    n.insertExistingPage(cover.memory as nopodofo.Document, 0, 0)
    await new Promise(resolve => n.write((err, data) => {
        if (err) Expect.fail(err.message)
        Expect(Buffer.isBuffer(data))
        return resolve()
    }))
}

async function run() {
    for (let i = 0; i < 100; i++) {
        await pdf()
        if (global.hasOwnProperty('gc')) {
            global.gc()
        }
        const used = process.memoryUsage()
        for (let key in used) {
            console.log(`Memory usage: ${key} ${Math.round((used as any)[key as string] / 1024 / 1024 * 100) / 100} MB`)
        }
    }
}

run()
    .then(() => {
        console.log('done')
        const used = process.memoryUsage()
        for (let key in used) {
            console.log(`Memory usage: ${key} ${Math.round((used as any)[key as string] / 1024 / 1024 * 100) / 100} MB`)
        }
    })
    .catch(e => console.error(e))
