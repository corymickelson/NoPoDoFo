import {join} from 'path'
import {writeFileSync} from 'fs'
import * as tap from 'tape'
import {Test} from 'tape'
import {nopodofo as npdf, NPDFDestinationFit, NPDFName, NPDFPageMode, Ref} from '../../'

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
    const doc = new npdf.Document()
    doc.load(filePath, async e => {
        if (e) t.fail(e.message)
        t.comment("Document instance accessors [GET]")
        t.assert(doc.getPage(0) instanceof (npdf.Page as any), "Get Page returns a Page")
        doc.splicePages(0, 1)
        t.comment('Pages spliced')
        t.assert(doc.getPageCount() === 3, 'Page zero removed from document')
        t.assert(doc.form instanceof (npdf.Form as any), "Get Form returns an AcroForm")
        t.assert(doc.catalog instanceof (npdf.Object), "Catalog as Obj")
        t.assert(doc.trailer instanceof (npdf.Object), "Catalog as Obj")
        t.assert(Array.isArray(doc.body)
            && doc.body.filter(i => i.type === 'Reference').length === 0,
            'document body returns an array of objects')
        let outline = doc.getOutlines(false)
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
        let embeddedFiles = names.getDictionary().getKey<npdf.Dictionary>(NPDFName.EMBEDDED_FILES).getKey<npdf.Array>(NPDFName.KIDS)
        let fileSpecArray = embeddedFiles.at(0) as npdf.Object
        t.assert((fileSpecArray.getDictionary().getKey<npdf.Array>(NPDFName.NAMES).at(0) as npdf.Object).getString().includes('scratch2Etxt'), 'embedded file path correct')

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
            doc.encrypt = encrypt as npdf.Encrypt
        }, 'Can set the Document Encrypt instance from CreateEncrypt value')
        t.comment("Document instance methods")
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
            fDoc.setPassword('secret')
        }
        let names = fDoc.getNames(false)
        if (!names) {
            t.fail();
            return
        }
        t.assert(names && names.type === 'Dictionary')
        t.assert(names.getDictionary().getKeys().includes(NPDFName.EMBEDDED_FILES))
        let embeddedFiles = names.getDictionary().getKey<npdf.Dictionary>(NPDFName.EMBEDDED_FILES).getKey<npdf.Array>(NPDFName.KIDS)
        let fileSpecArray = embeddedFiles.at(0) as npdf.Object
        let fileSpecObj = fileSpecArray.getDictionary().getKey<npdf.Array>(NPDFName.NAMES).at(1) as any
        global.gc()
        if (Array.isArray(fileSpecObj)) {
            fileSpecObj = fDoc.getObject(fileSpecObj as Ref)
        }
        t.assert((fileSpecObj as npdf.Object).getDictionary().getKey<String>(NPDFName.TYPE) === NPDFName.FILESPEC, 'FileSpec object')
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
            let outline: npdf.Outline = doc.getOutlines(true, 'Test') as any
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
                            let outlines: npdf.Outline = inn.getOutlines() as any
                            t.assert((outlines.first as npdf.Outline).title === 'Test', 'Outlines persist with title data')
                            t.assert((outlines.last as npdf.Outline).title === 'Second', 'Outlines persist with title data')
                            t.end()
                        }
                    })
                }
            })
        }
    })
})

tap('StreamDocument write to buffer', t => {
    const doc = new npdf.StreamDocument()
    const page = doc.createPage(new npdf.Rect(0,0,612, 792))
    const font = doc.createFont({fontName: 'Carlito', embed: true})
    font.size = 12
    const painter = new npdf.Painter(doc)
    painter.setPage(page)
    painter.font = font
    const black = new npdf.Color(0, 0, 0, 1)
    painter.setColor(black)
    t.comment('utf8 test. To verify print test output to disk and open file to verify test is correct')
    painter.drawText({x: 150, y: 400}, 'Unicode Umlauts: ÄÖÜß')
    painter.finishPage()
    const output = doc.close()
    t.assert(Buffer.isBuffer(output))
    t.end()
})