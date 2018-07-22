import * as tap from 'tape'
import { join } from 'path'
import { IField, IForm, NPDFAnnotation, NPDFAnnotationFlag, npdf } from "../../dist";

if (!global.gc) {
    global.gc = () => { }
}
tap('Signer', sub => {
    const doc = new npdf.Document()
    doc.load(join(__dirname, '../test-documents/test.pdf'), { forUpdate: true }, async e => {
        sub.test('ISigner Api', t => {

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
                if (e) { t.fail(e.message); return }
                signer.write(l, (e, d) => {
                    if (e) {
                        t.fail(e.message)
                    } else {
                        let signed = new npdf.Document()
                        signed.load(signedPath, e => {
                            if (e instanceof Error) t.fail(e.message)
                            let writtenSignatureField = signed.getPage(1).getFields().filter(i => i instanceof npdf.SignatureField)[0]
                            let docSignatureMode = signed.form.SigFlags
                            t.ok(writtenSignatureField, 'Signature field successfully written to page')
                            t.assert(docSignatureMode === 3, 'AcroForm SigFlags successfully updated')
                            t.end()
                        })
                    }
                })
            })
        })

    })

})


