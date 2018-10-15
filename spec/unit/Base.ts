import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo} from '../../'
import Document = nopodofo.Document;
import StreamDocument = nopodofo.StreamDocument;
import Rect = nopodofo.Rect;
import {join} from "path";
import Object = nopodofo.Object;
import Base = nopodofo.Base;

@TestFixture("Document & StreamDocument Base Methods")
export class BaseSpec {
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
                this.stream.createPages([new Rect(0, 0, 612, 792),
                    new Rect(0, 0, 612, 792),
                    new Rect(0, 0, 612, 792),
                    new Rect(0, 0, 612, 792)])
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
    @AsyncTest("Document Body Resolves all Objects")
    @TestCase('mem')
    @TestCase('stream')
    public async documentBodyResolutionSpec(t: string) {
        const doc = (this as any)[t]
        return Promise.resolve(Expect(doc.body.filter((i :Object) => i.type === 'Reference' || Array.isArray(i)).length).toBe(0))
    }

    @AsyncTest("Page Getter")
    @TestCase('mem', 0)
    @TestCase('stream', 0)
    @TestCase('mem', -1, true)
    @TestCase('stream', -1, true)
    @TestCase('mem', 10, true)
    @TestCase('stream', 10, true)
    @TestCase('mem', 2)
    @TestCase('stream', 2)
    public async getPage(t: string, idx: number, fails: boolean = false) {

        const doc = (this as any)[t]
        if (fails) {
            return Promise.resolve(Expect(() => doc.getPage(idx)).toThrowError(RangeError, "Page index out of range"))
        } else {
            return Promise.resolve(Expect(doc.getPage(idx)).toBeDefined())
        }
    }

    @AsyncTest("Page Setter, Create new page(s)")
    @TestCase('mem', [new Rect(0, 0, 0, 0)])
    @TestCase('stream', [new Rect(0, 0, 0, 0)])
    @TestCase('mem', [new Rect(0, 0, 0, 0), new Rect(0, 0, 0, 0)])
    @TestCase('stream', [new Rect(0, 0, 0, 0), new Rect(0, 0, 0, 0)])
    public async setPage(t: string, args: Rect[]) {
        const doc = (this as any)[t]
        return Promise.resolve(() => {
            if (args.length === 1) {
                let page = doc.createPage(args[0])
                Expect(page).toBeDefined()
                Expect(typeof page).toBe('object')
            } else {
                let pages = doc.createPages(args)
                Expect(pages).toEqual(doc.getPageCount())
            }
        })
    }


}