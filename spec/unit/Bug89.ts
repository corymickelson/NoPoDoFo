import {TestFixture, AsyncTest, Expect, Timeout, Setup} from 'alsatian';
import {NDocument} from '../../lib/NDocument';
import {nopodofo} from '../../'
import {join} from 'path'

@TestFixture('Bug89-RSS')
export class Bug89 {
    static async generate() {
        const n = await NDocument.from(join(__dirname, '../test-documents/bug89_doc.pdf'))
        const cover = await NDocument.from(join(__dirname, '../test-documents/bug89_cover.pdf'))
        const page = n.getPage(0)
        const carlito = n.createFont({
            fontName: 'Carlito',
            fileName: join(__dirname, '../test-documents/Carlito-Regular.ttf'),
            embed: true
        })
        n.painter.setPage(page)
        n.painter.font = carlito
        for (let i = 0; i < 100; i++) {
            n.painter.drawText({x: i * 10, y: i * 10}, `TEST ${i}`)
        }
        n.painter.finishPage()
        n.insertExistingPage(cover, 0, 0)
        await new Promise(resolve => n.write((err, data) => {
            if (err) Expect.fail(err.message)
            Expect(Buffer.isBuffer(data))
            return resolve()
        }))
    }

    @Setup
    public setup() {
        const config = new nopodofo.Configure()
        config.enableDebugLogging = true
    }

    @AsyncTest('RSS memory leak')
    @Timeout(50000)
    public async rssMemoryLeak() {
        let startingResidentSetAllocation
        let startingExternalAllocation
        try {
            for (let i = 0; i < 100; i++) {
                await Bug89.generate()
                if (global.hasOwnProperty('gc')) {
                    global.gc()
                }
                const used = process.memoryUsage()
                if (i === 0) {
                    startingResidentSetAllocation = used.rss
                    startingExternalAllocation = used.external
                } else {
                    if (startingResidentSetAllocation && startingResidentSetAllocation * 1.25 <= used.rss) {
                        Expect.fail('RSS doubled')
                    }
                }
                for (let key in used) {
                    console.log(`Memory usage: ${key} ${Math.round((used as any)[key as string] / 1024 / 1024 * 100) / 100} MB`)
                }
            }
        } catch (e) {
            Expect.fail(e.message)
        }
    }
}

