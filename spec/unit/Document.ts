import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo, nopodofo as npdf} from '../../'
import {join} from "path";
import {readFileSync} from "fs";
import Document = nopodofo.Document;
import Base = nopodofo.Base;

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

    @AsyncTest("Insert pages from other document")
    public async insertTest(t: string) {
        const doc = this.subject
        const startingCount = doc.getPageCount()
        const other = new Document()
        await new Promise(resolve => {
            other.load(join(__dirname, '../test-documents/test.pdf'), e => {
                if(e) Expect.fail(e.message)
                return resolve()
            })
        })
        doc.insertExistingPage(other, 0, 1)
        Expect(doc.getPageCount()).toBeGreaterThan(startingCount)
        return Promise.resolve()
    }

    @AsyncTest("Appending Documents")
    @Timeout(1000)
    @TestCase([join(__dirname, '../test-documents/test.pdf')])
    @TestCase([join(__dirname, '../test-documents/test.pdf'), join(__dirname, '../test-documents/test.pdf')])
    public async appendingSpec(append: string[]) {

        const doc = this.subject
        return new Promise(async (resolve) => {

            const originalPageCount = doc.getPageCount()
            let children = await Promise.all<Document>(append.map(i => new Promise(resolve => {
                const child = new Document()
                child.load(i, err => err ? Expect.fail(err.message) : resolve(child))
            })))
            if (children.length === 1) {
                console.log('Append single Document')
                doc.append(children[0])
            } else {
                console.log('Append Array of Documents')
                doc.append(children)
            }
            const total = children.reduce((accum, item) => accum += item.getPageCount(), originalPageCount)
            Expect(doc.getPageCount()).toEqual(total)
            return resolve()
        })
    }

    @AsyncTest("Get Document Fonts")
    public async fontsTest() {
        const doc = this.subject
        const fonts = doc.listFonts()
        Expect(fonts.length).toBeGreaterThan(0)
        const font = doc.getFont(fonts[0].name)
        Expect(font).toBeDefined()
    }
}
