import {AsyncTest, Expect, Setup, TestFixture, Timeout} from 'alsatian'
import {nopodofo, nopodofo as npdf, NPDFAnnotation, NPDFAnnotationFlag} from '../../'
import {join} from "path";
import {readFileSync} from "fs";

@TestFixture('Signer')
export class SignerSpec {
    @Setup
    public setup() {
        const config = new nopodofo.Configure()
        config.logFile('debug.txt')
        config.enableDebugLogging = true;
    }

    public loadDocument(): Promise<npdf.Document> {
        return new Promise(resolve => {
            const doc = new npdf.Document()
            doc.load(join(__dirname, '../test-documents/test.pdf'), {forUpdate: true}, async (e: Error) => {
                if (e) Expect.fail(e.message)
                return resolve(doc)
            })
        })
    }

    @AsyncTest('Load Cert and Key from Memory')
    @Timeout(100000)
    public async certAndKeyMemory() {
        const doc = await this.loadDocument()
        // Create an instance of Signer
        const rect = new npdf.Rect(0, 0, 10, 10),
            page = doc.getPage(1),
            annot = page.createAnnotation(NPDFAnnotation.Widget, rect)
        annot.flags = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
        // add image annotation.
        const field = new npdf.SignatureField(annot, doc)
        field.setReason('test')
        field.setLocation('here')
        field.setCreator('me')
        field.setFieldName('signer.sign')
        field.setDate()
        let signedPath = join(__dirname, "../test-documents/signed.pdf")
        let signer = new npdf.Signer(doc, signedPath)
        signer.signatureField = field
        const certificate = Buffer.from(readFileSync(join(__dirname, '../test-documents/certificate.pem')))
        const pkey = Buffer.from(readFileSync(join(__dirname, '../test-documents/key.pem')))
        console.log(certificate.length + pkey.length)
        await new Promise((resolve, reject) => signer.loadCertificateAndKey(certificate, {pKey: pkey}, (e, l) => {
            if (e) {
                Expect.fail(e.message);
                return
            }
            signer.write(l, (e, d) => {
                if (e) {
                    Expect.fail(e.message)
                } else {
                    let signed = new npdf.Document()
                    signed.load(signedPath, (e: Error) => {
                        if (e instanceof Error) Expect.fail(e.message)
                        let writtenSignatureField = signed.getPage(1).getFields().filter((i: any) => i instanceof npdf.SignatureField)[0]
                        let docSignatureMode = signed.form.SigFlags
                        Expect(writtenSignatureField).toBeDefined()
                        Expect(docSignatureMode).toBe(3)
                        return resolve()
                    })
                }
            })
        }))
    }

    @AsyncTest('Load Cert and Key from Disk')
    public async test() {
        const doc = await this.loadDocument()
        const rect = new npdf.Rect(0, 0, 10, 10),
            page = doc.getPage(1),
            annot = page.createAnnotation(NPDFAnnotation.Widget, rect)
        annot.flags = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
        // add image annotation.
        const field = new npdf.SignatureField(annot, doc)
        field.setReason('test')
        field.setLocation('here')
        field.setCreator('me')
        field.setFieldName('signer.sign')
        field.setDate()
        let signedPath = join(__dirname, "../test-documents/signed.pdf")
        let signer = new npdf.Signer(doc, signedPath)
        signer.signatureField = field


        await new Promise(resolve => signer.loadCertificateAndKey(join(__dirname, '../test-documents/certificate.pem'), {pKey: join(__dirname, '../test-documents/key.pem')}, (e, l) => {
            if (e) {
                Expect.fail(e.message);
                return
            }
            signer.write(l, (e, d) => {
                if (e) {
                    Expect.fail(e.message)
                } else {
                    let signed = new npdf.Document()
                    signed.load(signedPath, (e: Error) => {
                        if (e instanceof Error) Expect.fail(e.message)
                        let writtenSignatureField = signed.getPage(1).getFields().filter((i: any) => i instanceof npdf.SignatureField)[0]
                        let docSignatureMode = signed.form.SigFlags
                        Expect(writtenSignatureField).toBeDefined()
                        Expect(docSignatureMode).toBe(3)
                        return resolve()
                    })
                }
            })
        }))
    }
}
