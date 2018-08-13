import {join} from 'path'
import {writeFileSync} from 'fs'
import * as tap from 'tape'
import {Test} from 'tape'
import {IDocument, npdf, NPDFDestinationFit, NPDFName, NPDFPageMode} from '../../lib';
import {IObj, Ref} from "../../lib/object";
import {IOutline} from "../../lib/outlines";

if (!global.gc) {
    global.gc = () => {
    }
}
const filePath = join(__dirname, '../test-documents/test.pdf'),
    pwdDoc = join(__dirname, '../test-documents/pwd.pdf')

export const end = (...tests: Test[]) => tests.forEach(t => t.end())

tap('IStreamDocument', (t: Test) => {
    let strPath = join(__dirname, './stream.tmp.pdf')
    const doc = new npdf.StreamDocument(strPath)
    t.comment('StreamDocument default values')
    t.assert(doc.version === 1.7, 'default to pdf version 1.7')
    t.assert(doc.info.producer === 'PoDoFo - http://podofo.sf.net', 'Produced by PoDoFo')
    t.assert(doc.getPageCount() === 0, 'StreamDocument instantiated with 0')
    t.assert(doc.pageMode === NPDFPageMode.UseNone, 'defaults to \'UseNone\'')
    t.end()
})
tap('IDocument', (t: Test) => {
    const doc: IDocument = new npdf.Document()
    doc.load(filePath, async e => {
        if (e) t.fail(e.message)
        t.comment("Document instance accessors [GET]")
        t.assert(doc.getPage(0) instanceof (npdf.Page as any), "Get Page returns a Page")
        doc.splicePages(0, 1)
        t.comment('Pages spliced')
        t.assert(doc.getPageCount() === 3, 'Page zero removed from document')
        t.assert(doc.form instanceof (npdf.Form as any), "Get Form returns an AcroForm")
        t.assert(doc.catalog instanceof (npdf.Obj as any), "Catalog as Obj")
        t.assert(doc.trailer instanceof (npdf.Obj as any), "Catalog as Obj")
        t.assert(Array.isArray(doc.body)
            && doc.body.filter(i => i.type === 'Reference').length === 0,
            'document body returns an array of objects')
        let outline: IOutline = doc.getOutlines(false) as any
        t.assert(outline  === null, 'returns null when outline does not exist')
        outline = doc.getOutlines(true) as any
        t.ok(outline, 'create a new outline object with create = true')
        t.doesNotThrow(() => {
            doc.attachFile(join(__dirname, '../test-documents/scratch.txt'))
        })
        let names = doc.getNames(false)
        if (!names) {
            t.fail();
            return
        }
        t.assert(names && names.type === 'Dictionary')
        t.assert(names.getDictionary().getKeys().includes(NPDFName.EMBEDDED_FILES))
        let embeddedFiles = names.getDictionary().getKey(NPDFName.EMBEDDED_FILES).getDictionary().getKey(NPDFName.KIDS)
        let fileSpecArray: IObj = embeddedFiles.getArray().at(0) as any
        t.assert((fileSpecArray.getDictionary().getKey(NPDFName.NAMES).getArray().at(0)as IObj).getString().includes('scratch2Etxt'), 'embedded file path correct')

        t.comment("Document instance accessors [SET]")
        t.comment('Can create an encrypt instance and set to Document')
        let encrypt = npdf.Encrypt.createEncrypt({
            ownerPassword: 'secret',
            userPassword: 'secret',
            keyLength: 40,
            protection: ['Edit', 'FillAndSign'],
            algorithm: 'rc4v1'
        })

        t.doesNotThrow(() => {
            doc.encrypt = encrypt
        }, 'Can set the Document Encrypt instance from CreateEncrypt value')
        t.comment("Document instance methods")
        let docFont = doc.getFont('PoDoFoFt31')
        t.assert(docFont instanceof (npdf.Font as any))
        t.assert(docFont.getMetrics().fontSize > 0)
        t.assert(doc.getPageCount() === 3)
        t.comment('create font')
        let font = doc.createFont({italic: true, embed: false, fontName: 'Courier'})

        t.assert(font.getMetrics().fontSize === 12, 'Default font size is 12')

        doc.write((err, data) => {
            if (err) t.fail(err.message)
            else {
                t.assert(Buffer.isBuffer(data), 'Persists document to new Buffer')
                t.comment('Loading document from a node Buffer')
                let childDoc = new npdf.Document()
                childDoc.load(data, e => {
                    if (e) {
                        t.comment('load should fail, password required')
                        t.pass()
                        writeFileSync(join(__dirname, '../test-documents/attachment.pdf'), data)
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

tap('File Spec attachment', (t: Test) => {
    const fDoc = new npdf.Document()
    fDoc.load(join(__dirname, '../test-documents/attachment.pdf'), {password: 'secret'}, e => {
        if (e) {
            fDoc.password = 'secret'
        }
        let names = fDoc.getNames(false)
        if (!names) {
            t.fail();
            return
        }
        t.assert(names && names.type === 'Dictionary')
        t.assert(names.getDictionary().getKeys().includes(NPDFName.EMBEDDED_FILES))
        let embeddedFiles = names.getDictionary().getKey(NPDFName.EMBEDDED_FILES).getDictionary().getKey(NPDFName.KIDS)
        let fileSpecArray: IObj = embeddedFiles.getArray().at(0) as any
        let fileSpecObj = fileSpecArray.getDictionary().getKey(NPDFName.NAMES).getArray().at(1) as any
        global.gc()
        if (Array.isArray(fileSpecObj)) {
            fileSpecObj = fDoc.getObject(fileSpecObj as Ref)
        }
        t.assert((fileSpecObj as IObj).getDictionary().getKey(NPDFName.TYPE).getName() === NPDFName.FILESPEC, 'FileSpec object')
        let npdfFileSpec = new npdf.FileSpec(fileSpecObj)
        t.ok(npdfFileSpec, 'copy constructor')
        t.end()
    })

})
tap('IDocument password protected', (t: Test) => {
    let doc = new npdf.Document()
    doc.load(pwdDoc, {password: 'secret'}, e => {
        if (e) t.fail(e.message)
        else {
            t.pass('Loaded password protected document (provided a valid password on load)')
            t.comment('testing insert existing pages')
            let inner = new npdf.Document()
            inner.load(filePath, err => {
                if (err) t.fail(err.message)
                else {
                    inner.insertExistingPage(doc, 0, -1)
                    t.assert(inner.getPageCount() === 5, 'added additional page')
                    t.end()
                }
            })
        }
    })
})

/**
 * @todo: Append only works with Document + Document, fix to support StreamDocument as well
 */
tap('IDocument append doc', (t: Test) => {
    // let doc = new npdf.StreamDocument('/tmp/append.test.pdf')
    // let page = doc.createPage(new npdf.Rect(0, 0, 612, 792))
    // let painter = new npdf.Painter(doc)
    // let font = doc.createFont({fontName: 'Courier'})
    // painter.setPage(page)
    // painter.font = font
    // painter.drawTextAligned({x:365, y: 690, width: 40}, 'Testing', NPDFAlignment.Center)
    // painter.finishPage()
    let doc = new npdf.Document()
    doc.load(filePath, e => {
        if (e) t.fail(e.message)
        let mergeDoc = new npdf.Document()
        mergeDoc.load(filePath, e => {
            if (e) t.fail(e.message)
            else {
                doc.append(mergeDoc)
                t.assert(doc.getPageCount() === mergeDoc.getPageCount() * 2)
                t.end()
            }
        })
    })

})

tap('IDocument has/get signatures', t => {
    let doc = new npdf.Document()
    doc.load(join(__dirname, '../test-documents/signed.pdf'), e => {
        if (e) {
            t.fail(e.message)
            return
        }
        t.assert(doc.hasSignatures(), 'Found signature(s) fields')
        t.ok(doc.getSignatures(), 'can get signatures')
        t.assert(doc.getSignatures().length === 1, 'has one signature')
        t.end()
    })
})

tap('IDocument Destinations, Outline', t => {
    let doc = new npdf.Document()
    doc.load(filePath, e => {
        if (e) t.fail()
        else {
            let outline: IOutline = doc.getOutlines(true, 'Test') as any
            let dest = new npdf.Destination(doc.getPage(0), NPDFDestinationFit.Fit)
            let first = outline.createChild('First', dest);
            t.ok(first.title)
            let sec = new npdf.Destination(doc.getPage(1), NPDFDestinationFit.Fit)
            let firstSecond = first.createNext('Second', sec)
            doc.write((err, data) => {
                if (err) t.fail('Failed to write document with named destination')
                else {
                    let inn = new npdf.Document()
                    inn.load(data, e => {
                        if (e) t.fail('Failed to load new document with named destination')
                        else {
                            let outlines: IOutline = inn.getOutlines() as any
                            t.assert((outlines.first as IOutline).title === 'Test', 'Outlines persist with title data')
                            t.assert((outlines.last as IOutline).title === 'Second', 'Outlines persist with title data')
                            t.end()
                        }
                    })
                }
            })
        }
    })
})