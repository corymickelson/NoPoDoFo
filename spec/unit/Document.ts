import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo, nopodofo as npdf} from '../../'
import {join} from "path";
import {readFileSync} from "fs";
import {platform} from 'os'
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
                return resolve()
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

    public resolveLoad(src: string | Buffer): Promise<Document> {
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
    public async pageSplicing(start: number, end: number) {
        const startingCount = this.subject.getPageCount()
        if (start < 0 || start + end > startingCount) {
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
                if (e) Expect.fail(e.message)
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

    @AsyncTest('Insert Existing')
    @Timeout(1000000)
    public async insertExistingTest() {
        const magic = 'INSERT EXISTING PRE-PAINT'
        const prepend = await new Promise<Document>(resolve => {
            const p = new Document()
            p.load(join(__dirname, '../test-documents/test.pdf'), async e => {
                if (e) {
                    Expect.fail(e.message)
                } else {
                    const painter = new nopodofo.Painter(p)
                    painter.setPage(p.getPage(0))
                    const fObj = Object.assign({
                        fontName: 'FiraCode',
                        bold: true
                    }, (o => o === 'win32' ? {fileName: 'C:\\Users\\micke\\WebstormProjects\\lp-welcome-letter-state-machine\\fonts\\Carlito-Regular.ttf'} : null)(platform()))
                    painter.font = p.createFont(fObj)
                    painter.setColor(new npdf.Color(0.9))
                    painter.drawText({x: 0, y: 0}, magic)
                    painter.finishPage()
                    try {

                    let nd = await new Promise<Document>(rr => {
                        p.write((e, d) => {
                        if(e) Expect.fail(e.message)
                        else {
                            const rl = new Document()
                            rl.load(d, (ee, dd) => {
                                if(ee) Expect.fail(ee.message)
                                else {
                                    return rr(dd)
                                }
                            })
                        }
                    })
                    })
                    return resolve(nd)
                    } catch(e) {
                       console.error(e)
                    }
                }
            })
        })

        const added = this.subject.insertExistingPage(prepend, 0, 0)
        Expect(added).toBe(prepend.getPageCount() + 1)

        const found = await new Promise((resolve, reject) => {
            this.subject.write(async (e, d) => {
                const fail = (e: Error) => {
                    // set to null to force skipping the teardown step
                    // @ts-ignore
                    this.subject = null
                    Expect.fail(e.message)
                }
                if (e) {
                    fail(e)
                } else {
                    console.log('size before: ', d.length)
                    d = await new Promise<Buffer>(resolve => Document.gc(d, (ee, dd) => ee ? fail(ee) : resolve(dd as Buffer)))
                    console.log('size after: ', d.length)
                    const rDoc = new Document()
                    rDoc.load(d, e => {
                        if (e) {
                            fail(e)
                        }
                        const parser = new nopodofo.ContentsTokenizer(rDoc, 0)
                        const tokens = parser.readSync()
                        let item = tokens.next()
                        while (!item.done) {
                            if (item.value.includes(magic)) {
                                // set to null to force skipping the teardown step
                                // @ts-ignore
                                this.subject = null
                                return resolve(true)
                            }
                            item = tokens.next()
                        }
                        return resolve(false)
                    })
                }
            })
        })
        if (!found) {
            Expect.fail('Magic string not found')
        }
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
