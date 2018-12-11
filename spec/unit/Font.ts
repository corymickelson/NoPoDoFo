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
        await new Promise(resolve => {
            this.mem.write((err, data) => err ? Expect.fail(err.message) : resolve())
        })
        this.stream.close()
        return Promise.resolve()
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

    @AsyncTest('Create Subsetting font')
    @TestCase('mem')
    @TestCase('stream')
    public async createSubFont(m: string) {
        const doc: Base = (this as any)[m]
        const firaCodeSub = doc.createFontSubset({
            fontName: 'Fira Code',
            fileName: `${homedir()}/.fonts/f/FiraCode_Regular.ttf`
        })
        const firaCode = doc.createFont({
            fontName: 'Fira Code',
            fileName: `${homedir()}/.fonts/f/FiraCode_Regular.ttf`
        })
        const metricSub = firaCodeSub.getMetrics()
        const metric = firaCode.getMetrics()
        Expect(metric.fontScale).toEqual(metricSub.fontScale)
        Expect(metric.fontSize).toEqual(metricSub.fontSize)
        Expect(metric.charSpace).toEqual(metricSub.charSpace)
        Expect(metric.fontWeight).toEqual(metricSub.fontWeight)
        Expect(metric.lineSpacing).toEqual(metricSub.lineSpacing)
    }

}