import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture} from 'alsatian'
import {join} from 'path'
import {nopodofo} from '../../'
import Document = nopodofo.Document;
import StreamDocument = nopodofo.StreamDocument;

@TestFixture("NoPoDoFo PDF Primitives")
export class PrimitiveSpec {
    // @ts-ignore
    private mem: Document
    // @ts-ignore
    private stream: StreamDocument

    @AsyncSetup
    public async setup() {
        this.mem = new Document()
        await new Promise((resolve) => {
            this.mem.load(join(__dirname, '../test-documents/test.pdf'), err => {
                if (err) {
                    Expect.fail(err.message)
                }
                return resolve()
            })
        })
        this.stream = new StreamDocument()
    }

    @AsyncTeardown
    public async teardown() {
        try {
            await new Promise((resolve) => {
                this.mem.write((err, data: Buffer) => {
                    if (err) {
                        Expect.fail(err.message)
                    }
                    if (Buffer.isBuffer(data) && data.length > 0) {
                        return resolve()
                    }
                })
            })
            let streamBuffer = this.stream.close()
            if ((Buffer.isBuffer(streamBuffer) || typeof streamBuffer === 'string') && streamBuffer.length > 0) {
                return Promise.resolve()
            }
        } catch (e) {
            Expect.fail(e)
        }
    }

    @AsyncTest('Create new')
    @TestCase('mem')
    @TestCase('stream')
    public async instantiateNew(m: string) {
        let doc: Document = (this as any)[m]
        let stringObj = new nopodofo.Object('')
        Expect(stringObj.type).toBe('String')
        let arrayObj = new nopodofo.Object([])
        Expect(arrayObj.type).toBe('Array')
        let noObj = new nopodofo.Object(0.0)
        Expect(noObj.type).toBe('Real')
    }
}
