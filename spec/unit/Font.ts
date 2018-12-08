import {Expect, AsyncTest, TestFixture, TestCase, AsyncSetup, AsyncTeardown} from 'alsatian'
import {nopodofo} from '../../'
import {join} from "path"
import {homedir} from 'os'
import Base = nopodofo.Base;

@TestFixture('Fonts')
export class FontSpec {
    // @ts-ignore
    private mem: nopodofo.Document
    // @ts-ignore
    private stream: nopodofo.StreamDocument

    @AsyncSetup
    public async setup() {
        this.mem = new nopodofo.Document()
        await new Promise(resolve => {
            this.mem.load(join(__dirname, '../test-documents/test.pdf'), err => {
                if (err) Expect.fail(err.message)
                return resolve()
            })
        })
        this.stream = new nopodofo.StreamDocument()
        this.stream.createPage(new nopodofo.Rect(0, 0, 612, 792))
        return Promise.resolve()
    }

    @AsyncTeardown
    public async teardown() {

    }

    @AsyncTest('Create Font')
    @TestCase('mem')
    @TestCase('stream')
    public async createFont(m: string) {
        const doc: Base = (this as any)[m]
        const firaCode = doc.createFont({
            fontName: 'Fira Code',
            fileName: `${homedir()}/.fonts/f/FiraCode_Regular.ttf`
        })
        const metric = firaCode.getMetrics()
        Expect(metric.fileName).toBe(`${homedir()}/.fonts/f/FiraCode_Regular.ttf`)
        Expect(metric.fontSize).toBe(12)
    }

}