import * as tap from 'tape'
import {join} from 'path'
import {IField, signature, IForm, NPDFAnnotation, NPDFAnnotationFlag, npdf} from "../../dist";

if(!global.gc) {
    global.gc = () => {}
}
tap('Signer', sub => {
    const doc = new npdf.Document()
    doc.load(join(__dirname, '../test-documents/test.pdf'), {forUpdate: true}, async e => {
        sub.test('Sign Sync', async standard => {
            standard.plan(3)
            if (e instanceof Error) throw e
            try {
                doc.form.SigFlags = 3
                doc.form.needAppearances = false
                const rect = new npdf.Rect(0, 0, 10, 10),
                    page = doc.getPage(1),
                    annot = page.createAnnotation(NPDFAnnotation.Widget, rect)
                annot.flags = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
                const field = new npdf.SignatureField(annot, doc),
                    signatureData = await signature(join(__dirname, '../test-documents/certificate.pem'), join(__dirname, '../test-documents/key.pem'))
                standard.ok(signatureData)
                field.setReason('test')
                field.setLocation('here')
                field.setCreator('me')
                field.setFieldName('signer.sign')
                field.setDate()

                let signedPath = "/tmp/signed.pdf"
                let signer = new npdf.Signer(doc, signedPath)
                signer.setField(field)
                signer.sign(signatureData, (err, data) => {
                    if (err) standard.fail(err.message)
                    else {
                        let signed = new npdf.Document()
                        signed.load(signedPath, e => {
                            if (e instanceof Error) standard.fail(e.message)
                            standard.assert(signed.getPage(1).getFields().filter(i => i instanceof npdf.SignatureField).length === 1)
                            standard.assert(signed.form.SigFlags === 3)
                            standard.end()
                        })
                    }
                })

            }
            catch
                (e) {
                console.error(e)
            }
        })

    })

})


