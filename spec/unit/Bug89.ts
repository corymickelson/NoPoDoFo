import {AsyncTest, Expect, Setup, TestFixture, Timeout} from 'alsatian';
import {NDocument} from '../../lib/NDocument';
import {nopodofo, NPDFLogLevel} from '../../'
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
        const data = await n.write()
        Expect(Buffer.isBuffer(data))
    }

    @Setup
    public setup() {
        const logger = new nopodofo.Log()
        logger.logFile('debug.txt')
        logger.logLevel= NPDFLogLevel.info
        global.gc()
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
                const roundAlloc = (x: number) => Math.round(x / 1024 / 1024 * 100) / 100
                if (i === 0) {
                    startingResidentSetAllocation = roundAlloc(used.rss)
                    startingExternalAllocation = roundAlloc(used.external)
                } else {
                    if (startingResidentSetAllocation && startingResidentSetAllocation * 1.2 <= roundAlloc(used.rss)) {
                        Expect.fail(`Starting RSS ${startingResidentSetAllocation}, max exceeded: ${roundAlloc(used.rss)}`)
                    }
                }
                for (let key in used) {
                    console.log(`Memory usage: ${key} ${roundAlloc((used as any)[key as string])} MB`)
                }
            }
        } catch (e) {
            Expect.fail(e.message)
        }
    }
}

