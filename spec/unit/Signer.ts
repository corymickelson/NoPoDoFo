import {AsyncTest, Expect, Setup, TestFixture, Timeout} from 'alsatian'
import {nopodofo, nopodofo as npdf, NPDFAnnotation, NPDFAnnotationFlag} from '../../'
import {join} from "path";
import {readFileSync} from "fs";

@TestFixture('Signer')
export class SignerSpec {
    @Setup
    public setup() {
        const config = new nopodofo.Configure()
        config.enableDebugLogging = true;
    }

    @AsyncTest('Load Cert and Key from Memory')
    public async certAndKeyMemory() {
        return new Promise(resolve => {
            const doc = new npdf.Document()
            doc.load(join(__dirname, '../test-documents/test.pdf'), {forUpdate: true}, async (e: Error) => {

                // Create a SignatureField
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

                // Create an instance of Signer
                let signedPath = join(__dirname, "../test-documents/signed.pdf")
                let signer = new npdf.Signer(doc, signedPath)
                signer.signatureField = field
                const certificate = Buffer.from(readFileSync(join(__dirname, '../test-documents/certificate.pem')))
                const pkey = Buffer.from(readFileSync(join(__dirname, '../test-documents/key.pem')))
                signer.loadCertificateAndKey(certificate, pkey, (e, l) => {
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
                })

            })
        })
    }

    @AsyncTest('Load Cert and Key from Disk')
    public async test() {
        return new Promise(resolve => {
            const doc = new npdf.Document()
            doc.load(join(__dirname, '../test-documents/test.pdf'), {forUpdate: true}, async (e: Error) => {

                // Create a SignatureField
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

                // Create an instance of Signer
                let signedPath = join(__dirname, "../test-documents/signed.pdf")
                let signer = new npdf.Signer(doc, signedPath)
                signer.signatureField = field
                // const signingData = await new Promise((resolve, reject) => {
                //     signer.getSigningContent((e: Error, d: String) => {
                //         if (e) return reject(e)
                //         else return resolve(d)
                //     })
                // })
                // Expect(signingData).toBeTruthy();

                signer.loadCertificateAndKey(join(__dirname, '../test-documents/certificate.pem'), join(__dirname, '../test-documents/key.pem'), (e, l) => {
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
                })

            })
        })
    }
}
