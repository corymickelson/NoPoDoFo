import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo, nopodofo as npdf} from '../../'
import {join} from "path";
import {readFileSync} from "fs";
import Document = nopodofo.Document;

@TestFixture("(Mem)Document")
export class MemDocSpec {
    readonly filePath = join(__dirname, '../test-documents/test.pdf')
    readonly pwdDoc = join(__dirname, '../test-documents/pwd.pdf')
    // @ts-ignore
    public subject: npdf.Document = null


    @AsyncSetup
    public async setup() {
        return new Promise((resolve, reject) => {
            if (!this.subject) this.subject = new npdf.Document()
            this.subject.load(this.filePath, err => {
                if (err) {
                    Expect.fail(err.message)
                    reject(err)
                }
                Expect(this.subject).toBeDefined()
                return resolve()
            })
        })
    }

    @AsyncTeardown
    public async teardown() {
        return new Promise(resolve => {
            if (!this.subject) {
                return Promise.resolve()
            }
            this.subject.write((err, data) => {
                if (err) Expect.fail(err.message);
                Expect(Buffer.isBuffer(data)).toBeTruthy();
                (this.subject as any) = null
                if (global.hasOwnProperty('gc')) {
                    global.gc()
                }
                return resolve()
            })
        })
    }

    @AsyncTest("Create new Document")
    @TestCase(join(__dirname, '../test-documents/test.pdf'))
    @TestCase(readFileSync(join(__dirname, '../test-documents/test.pdf')))
    public async createNewDocumentSpec(src: string | Buffer) {
        let doc = await this.resolveLoad(src)
        Expect(doc).toBeDefined()
        return Promise.resolve()
    }

    public resolveLoad(src:string|Buffer): Promise<Document> {
        return new Promise<Document>((resolve, reject) => {
            new npdf.Document().load(src, (err, data) => err ? reject(err) : resolve(data))
        })
    }

    @AsyncTest("Top-Level Document Objects")
    @TestCase("trailer")
    @TestCase("catalog")
    @TestCase("form")
    @TestCase("body")
    @TestCase("version")
    @TestCase("info")
    public async documentInstanceAccessors(prop: string) {
        return Promise.resolve(Expect((this.subject as any)[prop]).toBeDefined())
    }

    @AsyncTest("Document Body Resolves all Objects")
    public async documentBodyResolutionSpec() {
        // const unresolved = this.subject.body.filter(i => i.type === 'Reference' || Array.isArray(i))
        return Promise.resolve(Expect(this.subject.body.filter(i => i.type === 'Reference' || Array.isArray(i)).length).toBe(0))
    }

    @AsyncTest("Page Getter")
    @TestCase(0)
    @TestCase(-1, true)
    @TestCase(10, true)
    @TestCase(2)
    public async getPage(idx: number, fails: boolean = false) {
        if (fails) {
            return Promise.resolve(Expect(() => this.subject.getPage(idx)).toThrowError(RangeError, "Page index out of range"))
        } else {
            return Promise.resolve(Expect(this.subject.getPage(idx)).toBeDefined())
        }
    }

    @AsyncTest("Page Setter, Create new page(s)")
    // @ts-ignore
    @TestCase([new npdf.Rect(0, 0, 0, 0)])
    // @ts-ignore
    @TestCase([new npdf.Rect(0, 0, 0, 0), new npdf.Rect(0, 0, 0, 0)])
    public async setPage(args: npdf.Rect[]) {
        return Promise.resolve(() => {
            if (args.length === 1) {
                let page = this.subject.createPage(args[0])
                Expect(page).toBeDefined()
                Expect(typeof page).toBe('object')
            } else {
                let pages = this.subject.createPages(args)
                Expect(pages).toEqual(this.subject.getPageCount())
            }
        })
    }

    @AsyncTest("Appending Documents")
    @Timeout(2500)
    @TestCase([join(__dirname, '../test-documents/test.pdf')])
    @TestCase([join(__dirname, '../test-documents/test.pdf'), join(__dirname, '../test-documents/test.pdf')])
    public async appendingSpec(append: string[]) {
        return new Promise(async (resolve) => {

            const originalPageCount = this.subject.getPageCount()
            let children = await Promise.all<npdf.Document>(append.map(i => new Promise(resolve => {
                const child = new npdf.Document()
                child.load(i, err => err ? Expect.fail(err.message) : resolve(child))
            })))
            if (children.length === 1) {
                console.log('Append single Document')
                this.subject.append(children[0])
            } else {
                console.log('Append Array of Documents')
                this.subject.append(children)
            }
            const total = children.reduce((accum, item) => accum += item.getPageCount(), originalPageCount)
            Expect(this.subject.getPageCount()).toEqual(total)
            return resolve()
        })
    }

    @AsyncTest("Page splicing")
    @TestCase(0, 1)
    @TestCase(1, 2)
    @TestCase(0, 22)
    @TestCase(-1, 4)
    public async pageSplicing(start:number, end:number) {
        const startingCount = this.subject.getPageCount()
        if(start < 0 || start + end > startingCount) {
            Expect(() => this.subject.splicePages(start, end)).toThrowError(RangeError, "Pages out of range")
        } else {
            this.subject.splicePages(start, end)
            Expect(this.subject.getPageCount()).toEqual(startingCount - end)
        }
        return Promise.resolve()
    }
}