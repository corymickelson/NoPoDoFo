import {AsyncSetup, AsyncTeardown, AsyncTest, Expect, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo, NPDFName} from '../../'
import Document = nopodofo.Document;
import StreamDocument = nopodofo.StreamDocument;
import {join} from "path";
import Base = nopodofo.Base;
import Object = nopodofo.Object;
import Rect = nopodofo.Rect;
import Ref = nopodofo.Ref;

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
            this.mem.write((err, data) => err ? Expect.fail(err.message) : resolve())
        })
        this.stream.close()
        return Promise.resolve()
    }

    @AsyncTest("Attach file read as FileSpec")
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
                        Expect((spec.getContents() as Buffer).toString()).toContain('TEST file :)')
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
            let srcNamesDict = (src.getNames(false) as Object).getDictionary().getKeys()
            Expect(srcNamesDict.includes(NPDFName.EMBEDDED_FILES)).toBeTruthy()
            let embeddedFiles = (src.getNames(false) as Object).getDictionary()
            let embeddedFilesRef = embeddedFiles.getKey<Ref>(NPDFName.EMBEDDED_FILES)
            if (embeddedFilesRef instanceof Ref) {
                embeddedFiles = src.getObject(embeddedFilesRef).getDictionary()
            }
            let embeddedFilesArray = embeddedFiles.getKey<nopodofo.Array>(NPDFName.KIDS)
            if(embeddedFilesArray instanceof Ref) {
                embeddedFilesArray = src.getObject(embeddedFilesArray as Ref).getArray()
            }
            let files = embeddedFilesArray.at(0)
            if(files instanceof Ref) {
                files = src.getObject(files)
            }
            let fileNames = (files as Object).getDictionary().getKey<nopodofo.Array>(NPDFName.NAMES)
            if(fileNames instanceof Ref) {
                fileNames = src.getObject(fileNames).getArray()
            }
            if(fileNames.at(0) instanceof Ref) {
                let name0 = src.getObject(fileNames.at(0) as Ref)
                Expect(name0.getString().includes('scratch')).toBeTruthy()
            } else {
                let name0 = (fileNames.at(0) as Object).getString()
                Expect(name0.includes('scratch')).toBeTruthy()
            }
        }
    }

}

