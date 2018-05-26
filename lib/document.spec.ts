import {access, readFile, unlinkSync} from 'fs'
import {join} from 'path'
import * as tap from 'tape'
import {IDocument, NPDFFontEncoding, __mod as mod, NPDFPageMode} from './document'
import {F_OK} from "constants";
import {v4} from 'uuid'
import {Test} from "tape";
import {IObj} from "./object";
import {EncryptOption} from "./encrypt";
import {NPDFVersion} from '.';

const filePath = join(__dirname, '../test-documents/test.pdf'),
    pwdDoc = join(__dirname, '../test-documents/pwd.pdf')

export const end = (...tests: Test[]) => tests.forEach(t => t.end())

tap('IStreamDocument', t => {
    const doc = new mod.StreamDocument('/tmp/npdf.streamdoc.pdf')
    t.comment('StreamDocument default values')
    t.assert(doc.version === NPDFVersion.Pdf17, 'default to pdf version 1.7')
    t.assert(doc.info.producer === 'PoDoFo - http://podofo.sf.net', 'Produced by PoDoFo')
    t.assert(doc.getPageCount() === 0, 'StreamDocument instantiated with 0')
    t.assert(doc.pageMode === NPDFPageMode.UseNone, 'defaults to \'UseNone\'')
    t.end()
})
tap('IDocument', t => {
    const doc = new mod.Document()
    doc.load(filePath, e => {
        if (e) t.fail(e.message)
        t.comment("Document instance accessors [GET]")
        t.assert(doc.getPage(0) instanceof (mod.Page as any), "Get Page returns a Page")
        t.assert(doc.form instanceof (mod.Form as any), "Get Form returns an AcroForm")
        t.assert(doc.catalog instanceof (mod.Obj as any), "Catalog as Obj")
        t.assert(doc.trailer instanceof (mod.Obj as any), "Catalog as Obj")
        t.comment("Document instance accessors [SET]")
        t.comment('Can create an encrypt instance and set to Document')
        let encrypt = mod.Encrypt.createEncrypt({
            ownerPassword: 'secret',
            userPassword: 'secret',
            keyLength: 40,
            protection: ['Edit', 'FillAndSign'],
            algorithm: 'aesv3'
        })
        t.doesNotThrow(() => {
            doc.encrypt = encrypt
        }, 'Can set the Document Encrypt instance from CreateEncrypt value')
        t.comment("Document instance methods")
        t.assert(doc.getFont('PoDoFoFt31') instanceof (mod.Font as any))
        doc.splicePages(0, 1)
        t.assert(doc.getPageCount() === 3)
        doc.write((err, data) => {
            if(err) t.fail(err.message)
            else {
                t.assert(Buffer.isBuffer(data), 'Persists document to new Buffer')
                t.comment('Loading document from a node Buffer')
                let childDoc = new mod.Document()
                childDoc.load(data, {fromBuffer: true}, e => {
                    if(e) t.fail(e.message)
                    else {
                        t.comment('Child document reflects changes from parent')
                        t.assert(childDoc.getPageCount() === 3, 'Spliced page persists')
                    }
                })
                t.end()
            }
        })
    })
})