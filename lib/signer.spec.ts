import {writeFileSync} from 'fs'
import {Document} from './document'
import {SignatureField} from './field'
import {NPDFAnnotation, NPDFAnnotationFlag} from './annotation'
import {Rect} from './rect'
import {join} from 'path'
import {Form} from "./form";
import {Signer, signature} from "./signer";


const doc = new Document(join(__dirname, '../test-documents/test.pdf'), true)
doc.on('ready', async e => {
    if (e instanceof Error) throw e
    try {
        // check for form
        let form = new Form(doc),
            formDict = form.getObject().asDictionary()
        if (!formDict.hasKey('SigFlags') ||
            formDict.getKey('SigFlags').type !== 'Number' ||
            formDict.getKey('SigFlags').asNumber() !== 3) {
            if (formDict.hasKey('SigFlags'))
                formDict.removeKey('SigFlags')
            formDict.addKey('SigFlags', 3)
        }
        if (form.needAppearances)
            form.needAppearances = false

        // create signature field
        const rect = new Rect([0, 0, 10, 10]),
            page = doc.getPage(doc.pageCount - 1),
            annot = page.createAnnotation(NPDFAnnotation.Widget, rect)
        annot.flag = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
        const field = new SignatureField(annot, form, doc)
        field.setReason('test')
        field.setLocation('here')
        field.setCreator('me')
        field.setDate()

        let signer = new Signer(doc, field)
        let signatureStr = signature(join(__dirname, '../test-documents/certificate.pem'), join(__dirname, '../test-documents/key.pem'))
        let signed = signer.sign(signatureStr)
        console.log(signed)
        if (signed) {
            writeFileSync('/tmp/test.pdf', signed)
            console.log('ok')
        }
    }
    catch
        (e) {
        console.error(e)
    }
})

