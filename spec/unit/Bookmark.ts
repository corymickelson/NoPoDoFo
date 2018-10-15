import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture} from 'alsatian'
import {nopodofo, NPDFActions, NPDFDestinationFit} from '../../'
import {join} from "path";
import Document = nopodofo.Document;
import StreamDocument = nopodofo.StreamDocument;
import Rect = nopodofo.Rect;
import Destination = nopodofo.Destination;
import Action = nopodofo.Action;
import Outline = nopodofo.Outline;

@TestFixture('Bookmarks')
export class BookmarkSpec {
    public mem: Document = new Document()
    public stream: StreamDocument = new StreamDocument()

    @AsyncSetup
    public async setup() {
        return new Promise((resolve) => {
            this.mem = new Document()
            this.stream = new StreamDocument()
            this.mem.load(join(__dirname, '../test-documents/test.pdf'), err => {
                if (err) Expect.fail(err.message)
                if (!this.stream) this.stream = new StreamDocument()
                this.stream.createPage(new Rect(0, 0, 612, 792))
                return resolve()
            })
        })
    }

    @AsyncTeardown
    public async teardown() {
        return new Promise((resolve, reject) => {
            this.mem.write(async (err, data) => {
                if (err) Expect.fail(err.message)
                this.stream.close();
                return resolve()
            })
        })
    }

    @AsyncTest('Outline Creater')
    @TestCase('mem', true)
    @TestCase('mem', false)
    @TestCase('stream', true)
    public async outlineCreator(t: string, c: boolean) {
        let o = (this as any)[t].getOutlines(c)
        if (c) Expect(o).toBeDefined()
        else Expect(o).toBeNull()
    }

    @AsyncTest('Outline as Bookmark')
    @TestCase('mem')
    @TestCase('stream')
    public async createBookmark(t: string) {
        const page = (this as any)[t].getPage(0)
        const bookmark = (this as any)[t].getOutlines(true, 'test') as Outline
        let child = new Destination(page, NPDFDestinationFit.Fit)
        let item = bookmark.createChild('first', child) as Outline
        Expect(item).toBeDefined()
        let action = new Action(this.mem, NPDFActions.GoTo)
        let next = item.createNext('next-action', action)
        Expect(next).toBeDefined()
        return Promise.resolve()
    }

    @AsyncTest('Outline Getter')
    @TestCase('mem')
    @TestCase('stream')
    public async postWriteSpec(t: string) {
        await new Promise(async resolve => {
            const page = (this as any)[t].getPage(0)
            const bookmark = (this as any)[t].getOutlines(true, 'test') as Outline
            let child = new Destination(page, NPDFDestinationFit.Fit)
            let item = bookmark.createChild('first', child) as Outline
            let action = new Action((this as any)[t], NPDFActions.GoTo)
            let next = item.createNext('next-action', action)
            await new Promise(async resolve => {
                let data: Buffer = await new Promise<Buffer>(async resolve => {
                    if (t === 'mem') {
                        let d = await new Promise<Buffer>(resolve => {
                                ((this as any)[t] as Document).write((err, d) => {
                                    if (err) Expect.fail(err.message)
                                    return resolve(d)
                                })
                            })
                        return resolve(d)
                    } else {
                        return resolve(((this as any)[t] as StreamDocument).close() as Buffer)
                    }
                })
                const doc = new Document()
                doc.load(data, err => {
                    if (err) Expect.fail(err.message)
                    let outline = doc.getOutlines(false) as Outline
                    Expect(outline).toBeDefined()
                    Expect((outline.first as Outline).title).toMatch(/test/)
                    Expect((outline.last as Outline).title).toMatch(/next-action/)
                    return resolve()
                })
            })
            return resolve()
        })
        return Promise.resolve()
    }

}