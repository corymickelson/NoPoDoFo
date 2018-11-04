import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture} from 'alsatian'
import {join} from "path";
import {nopodofo, NPDFAnnotation} from '../../'
import Rect = nopodofo.Rect;
import StreamDocument = nopodofo.StreamDocument;
import Document = nopodofo.Document;
import Base = nopodofo.Base;

@TestFixture('Annotations')
export class AnnotationSpec {
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

    @AsyncTest('Create new annotation')
    @TestCase('mem', 'Widget')
    @TestCase('stream', 'Widget')
    @TestCase('mem', '_3D')
    @TestCase('stream', '_3D')
    @TestCase('mem', 'Caret')
    @TestCase('stream', 'Caret')
    @TestCase('mem', 'Circle')
    @TestCase('stream', 'Circle')
    @TestCase('mem', 'FreeText')
    @TestCase('stream', 'FreeText')
    @TestCase('mem', 'Highlight')
    @TestCase('stream', 'Highlight')
    @TestCase('mem', 'Line')
    @TestCase('stream', 'Line')
    @TestCase('mem', 'Link')
    @TestCase('stream', 'Link')
    public async createNew(t: string, s: string) {
        const doc = (this as any)[t] as Base
        const page = doc.getPage(0)
        const subject = page.createAnnotation((NPDFAnnotation as any)[s], new Rect(305, 356, 100, 100))
        Expect(subject).toBeDefined()
        Expect(subject.getType().toLowerCase().replace(/_/gi, '')).toEqual(s.toLowerCase().replace(/_/gi, ''))
    }

    @AsyncTest('Annotation properties')
    @TestCase('mem', 'flags', 'Widget')
    @TestCase('stream', 'flags', 'Widget')
    @TestCase('mem', 'action', 'Widget')
    @TestCase('stream', 'action', 'Widget')
    @TestCase('mem', 'open', 'Widget')
    @TestCase('stream', 'open', 'Widget')
    @TestCase('mem', 'title', 'Widget')
    @TestCase('stream', 'title', 'Widget')
    @TestCase('mem', 'content', 'Widget')
    @TestCase('stream', 'content', 'Widget')
    @TestCase('mem', 'destination', 'Widget')
    @TestCase('stream', 'destination', 'Widget')
    @TestCase('mem', 'quadPoints', 'Widget')
    @TestCase('stream', 'quadPoints', 'Widget')
    @TestCase('mem', 'color', 'Widget')
    @TestCase('stream', 'color', 'Widget')
    @TestCase('mem', 'attachment', 'Widget')
    @TestCase('stream', 'attachment', 'Widget')
    public async propertiesTest(t: string, p: string, s: string) {
        const doc = (this as any)[t] as Base
        const page = doc.getPage(0)
        const subject = page.createAnnotation((NPDFAnnotation as any)[s], new Rect(0, 0, 100, 100))
        Expect((subject as any)[p]).toBeDefined()
    }
}
