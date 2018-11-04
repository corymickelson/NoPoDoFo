import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo} from '../../'
import {join} from "path";
import Document = nopodofo.Document;
import Base = nopodofo.Base;
import Rect = nopodofo.Rect;
import StreamDocument = nopodofo.StreamDocument;

@TestFixture('Page Spec')
export class pageSpec {
    // @ts-ignore
    public mem: Document
    // @ts-ignore
    public stream: StreamDocument

    @AsyncSetup
    public async setup() {
        this.mem = new Document()
        await new Promise(resolve => {
            this.mem.load(join(__dirname, '../test-documents/test.pdf'), err => {
                if (err) Expect.fail(err.message)
                return resolve()
            })
        })
        this.stream = new StreamDocument()
        this.stream.createPage(new Rect(0, 0, 611, 791))
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

    @AsyncTest('properties')
    @TestCase('mem')
    @TestCase('stream')
    public async props(t: string) {
        const doc = (this as any)[t] as Base
        const page = doc.getPage(0)
        Expect(page.number).toEqual(1)
        Expect(page.width).toBeGreaterThan(610)
        Expect(page.height).toBeGreaterThan(790)
        Expect(page.trimBox.bottom).toBeDefined()
        Expect(page.trimBox.height).toBeDefined()
        Expect(page.trimBox.left).toBeDefined()
        Expect(page.trimBox.width).toBeDefined()
        Expect(page.contents).toBeDefined()
        Expect(page.resources).toBeDefined()
        if (t === 'mem') {
            Expect(page.annotationCount()).toBeGreaterThan(0)
            Expect(page.fieldCount()).toBeGreaterThan(0)
        }
    }

    @AsyncTest('Rotation')
    @TestCase('mem')
    @TestCase('stream')
    public async rotations(t: string) {
        return new Promise(async resolve => {
            const doc = (this as any)[t] as Base
            const page = doc.getPage(0)
            page.rotation = 180
            let data: Buffer
            if (t === 'mem') {
                data = await
                    new Promise<Buffer>((resolve, reject) => {
                        (doc as Document).write((err, data) => {
                            if (err) Expect.fail(err.message)
                            return resolve(data)
                        })
                    })
            } else {
                data = (doc as StreamDocument).close() as Buffer
            }
            await
                new Promise((resolve, reject) => {
                    let child = new Document()
                    child.load(data, e => {
                        if (e) Expect.fail(e.message)
                        let subject = child.getPage(0)
                        Expect(subject.rotation).toEqual(180)
                        return resolve()
                    })
                })
            return resolve()
        })
    }

}