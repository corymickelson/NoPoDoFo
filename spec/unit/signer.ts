import * as tap from 'tape'
import {join} from 'path'
import {IField, signature, IForm, NPDFAnnotation, NPDFAnnotationFlag, npdf} from "../../dist";


tap('Signer', sub => {
    const doc = new npdf.Document()
    doc.load(join(__dirname, '../test-documents/test.pdf'), {forUpdate: true}, async e => {
        sub.test('Sign Sync', async standard => {
            standard.plan(2)
            if (e instanceof Error) throw e
            try {
                if ((doc.form as IForm).dictionary.hasKey('SigFlags') ||
                    (doc.form as IForm).dictionary.getKey('SigFlags').type !== 'Number' ||
                    (doc.form as IForm).dictionary.getKey('SigFlags').getNumber() !== 3) {
                    (doc.form as IForm).dictionary.removeKey('SigFlags');
                    (doc.form as IForm).dictionary.addKey('SigFlags', 3)
                }
                if ((doc.form as IForm).needAppearances)
                    (doc.form as IForm).needAppearances = false

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
                            const signedPage = doc.getPage(1),
                                fields = signedPage.getFields()
                            let signatureFieldCandidates = fields.filter((i:IField) => i.fieldName === 'signer.sign')
                            if (!signatureFieldCandidates || signatureFieldCandidates.length === 0) standard.fail("signature field not found")
                            else if (signatureFieldCandidates.length === 1) {
                                standard.pass("signature found")
                            }
                            else standard.fail("something went wrong")
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


