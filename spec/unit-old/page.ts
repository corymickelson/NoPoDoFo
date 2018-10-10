import { existsSync, unlinkSync, writeFile } from 'fs'
import { join } from 'path'
import * as test from 'tape'
import { nopodofo as npdf, NPDFAnnotation, NPDFFieldType, NPDFName } from "../../";
if (!global.gc) {
    global.gc = () => { }
}
const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = join(__dirname, '../tmp/test.out.pdf'),
    doc = new npdf.Document()

let page: npdf.Page;

doc.load(filePath, e => {
    page = doc.getPage(0)
    runAll()
})

function createField() {
    test('create field', t => {
        let doc = new npdf.StreamDocument('/tmp/test.pdf')
        let page = doc.createPage(new npdf.Rect(0, 0, 612, 792))
        let annot = page.createAnnotation(NPDFAnnotation.Widget, new npdf.Rect(0, 0, 100, 100))
        let field = page.createField(NPDFFieldType.TextField, annot, doc.form)
        t.ok(field)
        t.assert(page.fieldCount() === 1)
        t.end()
    })
}
function removeAnnotation() {
    test('remove annotation', t => {
        let annCount = page.annotationCount()
        let fieldsCount = page.fieldCount()
        page.deleteAnnotation(0)
        let field = page.getField(0)
        t.assert(annCount - 1 === page.annotationCount())
        t.assert(fieldsCount - 1 === page.fieldCount())
        t.ok(field)
        t.end()
    })
}

function pageRotation() {

    test('set page rotation', t => {
        const originalRotation = page.rotation,
            newRotation = originalRotation + 90
        page.rotation = newRotation
        doc.write(outFile, e => {
            const testDoc = new npdf.Document()
            testDoc.load(outFile, e => {
                if (e instanceof Error) t.fail()
                const testPage = testDoc.getPage(0)
                t.assert(testPage.rotation === newRotation, 'Page rotation updated')
                t.end()
            })
        })
    })
}

function pageProperties() {
    test('page instance properties', t => {
        t.assert(page.number === 1, 'page number valid')
        t.assert(page.width > 0, 'page width valid')
        t.assert(page.height > 0, 'page height valid')
        t.end()
    })
}

function pageTrimBox() {
    test('get trimbox', t => {
        const trimBox = page.trimBox
        t.assert(trimBox.height > 0, 'trimbox height is not null')
        t.assert(trimBox.width > 0, 'trimbox width not null')
        t.assert(trimBox.left === 0, 'trimbox left')
        t.assert(trimBox.bottom === 0, 'trimbox bottom')
        t.end()
    })
}

function pageGetAnnotsCount() {
    test('page number of annotations', t => {
        t.assert(page.annotationCount() === 21, 'get\'s all annotations on the page')
        t.end()
    })
}

function pageGetAnnot() {
    test('page get annotation', t => {
        const annot = page.getAnnotation(0)
        t.ok(annot)
        t.assert(annot.getType() === 'Widget')
        t.end()
    })
}

function pageContents() {
    test('page contents', t => {
        const contents = page.contents
        t.assert(contents instanceof (npdf.Object), 'is an instance of Obj')
        t.assert(contents.length > 0, 'content is not null or empty')
        t.end()
    })
}

function pageResources() {
    test('page resources', t => {
        const resources = page.resources
        t.assert(resources instanceof (npdf.Object), 'is instance ob Obj')
        t.assert(resources.length > 0, 'is not null or empty')
        t.end()
    })
}

function pageGetImgIterator() {
    test('add image', t => {
        const painter = new npdf.Painter(doc),
            img = new npdf.Image(doc, join(__dirname, '../test-documents/test.jpg'))

        painter.setPage(page)
        painter.drawImage(img, 0, page.height - img.height)
        painter.finishPage()
        function extractImg(obj: npdf.Object, jpg: Boolean) {
            let ext = jpg ? '.jpg' : '.ppm'
            writeFile(`/tmp/test.img.extract[1]${ext}`, obj.stream, err => {
                if (err instanceof Error)
                    t.fail()
                t.end()
            })
        }
        doc.write((e, d) => {
            let sub = new npdf.Document()
            sub.load(d, e => {
                if (e instanceof Error) t.fail()
                sub.body.forEach(o => {
                    if (o.type === 'Dictionary') {
                        let objDict = o.getDictionary(),
                            objType = objDict.hasKey('Type') ? objDict.getKeyType('Type') : null,
                            objSubType = objDict.hasKey('SubType') ? objDict.getKeyType('SubType') : null

                        if ((objType && objType === 'Name') ||
                            (objSubType && objSubType === 'Name')) {

                            if ((objType && objDict.getKey<String>('Type') === 'XObject') || (objSubType && objDict.getKey<String>('SubType') === 'Image')) {
                                if (objDict.hasKey('Filter')) {
                                    if (objDict.getKeyType('Filter') === 'Array') {
                                        const imgObjArr = objDict.getKey<npdf.Array>('Filter')
                                        if (imgObjArr.length === 1) {
                                            if ((imgObjArr.at(0) as npdf.Object).type === 'Name') {
                                                if ((imgObjArr.at(0) as npdf.Object).getName() === 'DCTDecode') {
                                                    extractImg(o, true)
                                                    return
                                                }
                                            }
                                        }
                                    } else if (objDict.getKeyType('Filter') === 'Name' && objDict.getKey<String>('Filter') === 'DCTDecode') {
                                        extractImg(o, true)
                                        return
                                    }

                                }
                            }
                        }
                    }
                })
            })
        })

    })
}

function pageImg() {
    test('add image', t => {
        const painter = new npdf.Painter(doc),
            img = new npdf.Image(doc, join(__dirname, '../test-documents/test.jpg'))

        painter.setPage(page)
        painter.drawImage(img, 0, page.height - img.height)
        painter.finishPage()
        function extractImg(obj: npdf.Object, jpg: Boolean) {
            let ext = jpg ? '.jpg' : '.ppm'
            writeFile(`/tmp/test.img.extract[2]${ext}`, obj.stream, err => {
                if (err instanceof Error)
                    t.fail()
                t.end()
            })
        }
        doc.write('/tmp/foo.pdf', (e, d) => {
            let sub = new npdf.Document()
            sub.load(d, e => {
                if (e instanceof Error) t.fail()
                let page = sub.getPage(0)
                let resource = page.resources.getDictionary().getKey<npdf.Dictionary>('XObject')
                let xob = resource.getKey<npdf.Dictionary>('XOb4')
                t.comment('GC')
                global.gc()
                if (xob.getKey<String>(NPDFName.SUBTYPE) === 'Image') {
                    if (xob.hasKey('Filter') && xob.getKey<String>('Filter') === 'DCTDecode') {
                        extractImg(xob.obj, true)
                    } else {
                        extractImg(resource.getKey<npdf.Object>('XOb4', false), true)
                    }
                }
            })
        })
    })
}

function runTest(test: Function) {
    setImmediate(() => {
        global.gc()
        test()
    })
}

export function runAll() {
    [   createField,
        pageRotation,
        pageProperties,
        pageTrimBox,
        pageGetAnnot,
        pageContents,
        pageResources,
        pageGetImgIterator,
        pageImg
    ].map(i => runTest(i))
}
