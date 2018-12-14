import {AsyncTest, Expect, TestFixture} from 'alsatian'
import {nopodofo as npdf, NPDFAnnotation, NPDFAnnotationFlag} from '../../'
import {join} from "path";

@TestFixture('Signer')
export class SignerSpec {
    @AsyncTest('Simple Signer')
    public async test() {
        return new Promise(resolve => {
            const doc = new npdf.Document()
            doc.load(join(__dirname, '../test-documents/test.pdf'), {forUpdate: true}, async (e: Error) => {

                // Create a SignatureField
                const rect = new npdf.Rect(0, 0, 10, 10),
                    page = doc.getPage(1),
                    annot = page.createAnnotation(NPDFAnnotation.Widget, rect)
                annot.flags = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
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