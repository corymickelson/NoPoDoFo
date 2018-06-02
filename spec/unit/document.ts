import {join} from 'path'
import * as tap from 'tape'
import {Test} from 'tape'
import {NPDFVersion, IDocument, npdf, NPDFPageMode} from '../../dist';

const filePath = join(__dirname, '../test-documents/test.pdf'),
    pwdDoc = join(__dirname, '../test-documents/pwd.pdf')

export const end = (...tests: Test[]) => tests.forEach(t => t.end())

tap('IStreamDocument', (t:Test) => {
    const doc = new npdf.StreamDocument('/tmp/npdf.streamdoc.pdf')
    t.comment('StreamDocument default values')
    t.assert(doc.version === 1.7, 'default to pdf version 1.7')
    t.assert(doc.info.producer === 'PoDoFo - http://podofo.sf.net', 'Produced by PoDoFo')
    t.assert(doc.getPageCount() === 0, 'StreamDocument instantiated with 0')
    t.assert(doc.pageMode === NPDFPageMode.UseNone, 'defaults to \'UseNone\'')
    t.end()
})
tap('IDocument', (t:Test) => {
    const doc:IDocument = new npdf.Document()
    doc.load(filePath, e => {
        if (e) t.fail(e.message)
        t.comment("Document instance accessors [GET]")
        t.assert(doc.getPage(0) instanceof (npdf.Page as any), "Get Page returns a Page")
        doc.splicePages(0, 1)
        t.comment('Pages spliced')
        global.gc()
        t.comment('Pages after GC')
        t.assert(doc.getPageCount() === 3, 'Page zero removed from document')
        t.assert(doc.form instanceof (npdf.Form as any), "Get Form returns an AcroForm")
        t.assert(doc.catalog instanceof (npdf.Obj as any), "Catalog as Obj")
        t.assert(doc.trailer instanceof (npdf.Obj as any), "Catalog as Obj")
        t.assert(Array.isArray(doc.body)
            && doc.body.filter(i => i.type === 'Reference').length === 0,
            'document body returns an array of objects')

        global.gc()
        t.comment("Document instance accessors [SET]")
        t.comment('Can create an encrypt instance and set to Document')
        let encrypt = npdf.Encrypt.createEncrypt({
            ownerPassword: 'secret',
            userPassword: 'secret',
            keyLength: 40,
            protection: ['Edit', 'FillAndSign'],
            algorithm: 'aesv3'
        })

        global.gc()
        t.doesNotThrow(() => {
            doc.encrypt = encrypt
        }, 'Can set the Document Encrypt instance from CreateEncrypt value')
        t.comment("Document instance methods")
        let docFont = doc.getFont('PoDoFoFt31')
        t.assert(docFont instanceof (npdf.Font as any))
        global.gc()
        t.assert(docFont.getMetrics().fontSize > 0)
        // doc.splicePages(0, 1)
        global.gc()
        // t.assert(doc.getPageCount() === 3)
        let font = doc.createFont({italic: true, embed: false, fontName: 'serif'})

        global.gc()
        t.assert(font.getMetrics().fontSize === 12, 'Default font size is 12')
        doc.write((err, data) => {
            if (err) t.fail(err.message)
            else {
                t.assert(Buffer.isBuffer(data), 'Persists document to new Buffer')
                t.comment('Loading document from a node Buffer')
                let childDoc = new npdf.Document()
                childDoc.load(data, {fromBuffer: true}, e => {
                    if (e) {
                        t.comment('load should fail, password required')
                        t.pass()
                        t.end()
                    }
                    else {
                        t.fail('encrypt did not persist.')
                    }
                })
            }
        })
    })
})
tap('IDocument password protected', (t:Test) => {
    let doc = new npdf.Document()
    doc.load(pwdDoc, {password: 'secret'}, e => {
        if(e) t.fail(e.message)
        else {
            t.pass('Loaded password protected document (provided a valid password on load)')
            t.end()
        }
    })
})
