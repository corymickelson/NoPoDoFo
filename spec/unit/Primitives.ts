import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {join} from 'path'
import {nopodofo} from '../../'
import Document = nopodofo.Document;
import StreamDocument = nopodofo.StreamDocument;
import {NDocument} from "../../lib/index";

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
    public async instantiateNew() {
        let stringObj = new nopodofo.Object('')
        Expect(stringObj.type).toBe('String')
        let arrayObj = new nopodofo.Object([])
        Expect(arrayObj.type).toBe('Array')
        let noObj = new nopodofo.Object(0.0)
        Expect(noObj.type).toBe('Real')
        let dObj = new nopodofo.Object()
        Expect(dObj.type).toBe('Dictionary')
    }

    @AsyncTest('NArray and nopodofo.Array equality')
    public async arrayAsArray() {
        let doc = await NDocument.from(join(__dirname, '../test-documents/test.pdf'))
        try {
            let candidates = doc.body.filter(i => i.type === 'Array')
            const subject = candidates[0].getArray()
            Expect(subject[0]).toEqual(subject.at(0))
        } catch (e) {
            Expect.fail(e)
        }
    }
}
