import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo, NPDFName} from '../../'
import Document = nopodofo.Document;
import StreamDocument = nopodofo.StreamDocument;
import {join} from "path";
import Base = nopodofo.Base;
import Object = nopodofo.Object;
import Dictionary = nopodofo.Dictionary;
import Rect = nopodofo.Rect;

@TestFixture("File Spec")
export class FileSpecPreWrite {
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
        this.stream.createPage(new Rect(0, 0, 612, 792))
        return Promise.resolve()
    }

    @AsyncTeardown
    public async teardown() {
        await new Promise(resolve => {
            this.mem.write( (err, data) => err ? Expect.fail(err.message) : resolve())
        })

        this.stream.close()

        // await new Promise(resolve => {
        //     const data = this.stream.close()
        //     const check = new Document()
        //     check.load(data, err => err ? Expect.fail(err.message) : resolve())
        // })
        return Promise.resolve()
    }

    @AsyncTest("Attach file (post-write)")
    @Timeout(500000)
    public async pdfWAttachment() {
        return new Promise(resolve => {
            const doc = new Document()
            doc.load(join(__dirname, '../test-documents/test.pdf'), err => {
                doc.attachFile(join(__dirname, '../test-documents/scratch.txt'))
                Expect(doc.getNames(false)).toBeDefined()
                doc.write(join(__dirname, '../test-documents/attachment.pdf'), (err, data) => {
                    if (err) Expect.fail(err.message)
                    const child = new Document()
                    child.load(join(__dirname, '../test-documents/attachment.pdf'), err => {
                        if (err) Expect.fail(err.message)
                        const spec = child.getAttachment('scratch.txt')
                        Expect(spec).toBeDefined()
                        Expect((spec.getContents() as  Buffer).toString()).toEqual('TEST file :)\n')
                        resolve()
                    })
                })
            })
        })
    }

    @AsyncTest("Create Outline Object")
    @TestCase('stream')
    @TestCase('mem')
    public async createOutline(src: string) {
        return Promise.resolve(Expect(((this as any)[src] as Base).getOutlines(true)).toBeDefined())
    }

    @AsyncTest("Attach file (pre-write)")
    // @ts-ignore
    @TestCase('mem', join(__dirname, '../test-documents/scratch.txt'))
    // @ts-ignore
    @TestCase('stream', join(__dirname, '../test-documents/scratch.txt'))
    // @ts-ignore
    @TestCase('stream', join(__dirname, '../test-documents/scratchX.txt'), true)
    public async attachFile(x: string, attachment: string, failure: boolean = false) {
        let src = ((this as any)[x] as Base)
        if (failure) {
            Expect(() => src.attachFile(attachment)).toThrowError(Error, "File: " + attachment + " not found")
        } else {
            src.attachFile(attachment)
            Expect(src.getNames(false)).toBeDefined()
            Expect((src.getNames(false) as Object).getDictionary().getKeys().includes(NPDFName.EMBEDDED_FILES)).toBeTruthy()
            let embeddedFiles = (src.getNames(false) as Object).getDictionary().getKey<Dictionary>(NPDFName.EMBEDDED_FILES).getKey<nopodofo.Array>(NPDFName.KIDS)
            Expect(((embeddedFiles.at(0) as Object).getDictionary().getKey<nopodofo.Array>(NPDFName.NAMES).at(0) as Object).getString().includes('scratch2Etxt'))
        }
    }

}

