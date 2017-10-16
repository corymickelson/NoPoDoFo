import {existsSync, unlink, unlinkSync} from 'fs'
import {join} from 'path'
import * as test from 'tape'
import {Document} from './document'
import {Rect} from "./rect";
import {NPdfAnnotation, IAnnotation, Annotation} from "./annotation";
import {Obj} from "./object";
import {Field} from './field';
import {Painter} from "./painter";
import {Image} from "./image";


const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    doc = new Document(filePath),
    page = doc.getPage(0)

function pageRotation() {
    test('set page rotation', t => {
        const originalRotation = page.rotation,
            newRotation = originalRotation + 90
        page.rotation = newRotation
        doc.write(outFile)

        const testDoc = new Document(outFile),
            testPage = testDoc.getPage(0)
        t.assert(testPage.rotation === newRotation, 'Page rotation updated')
        unlinkSync(outFile)
        t.end()
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

function pageFields() {
    test('get number of fields', t => {
        let n = page.getNumFields()
        t.assert(typeof n === 'number')
        // t.assert(page.getNumFields() === 22, 'page get number of fields counts all fields')
        t.end()
    })
}

function pageFieldInfoArray() {
    test('get fields info', t => {
        try {
            const fields = page.getFieldsInfo()

            // t.assert(fields.length === 22, 'page get field info get\'s info for each field')
            t.assert(fields instanceof Array)
            t.ok(fields)
            t.assert(fields.length > 0)
            t.end()
        }
        catch (e) {
            console.log(e)
            t.fail()
        }

    })
}

function pageGetField() {
    test('get field', t => {
        const field = page.getField(0)

        t.assert(field instanceof Field, 'is an instance of Field')
        t.end()
    })
}

function pageGetFields() {
    test('get fields', t => {
        const fields = page.getFields()
        t.assert(Array.isArray(fields), 'returns an array')
        t.assert(fields.length === page.getNumFields(), 'is not an empty array')
        t.assert(fields[0] instanceof Field, 'contains instance(s) of Field')
        t.end()
    })
}

function pageGetAnnotsCount() {
    test('page number of annotations', t => {
        t.assert(page.getNumAnnots() === 22, 'get\'s all annotations on the page')
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
        const contents = page.getContents(false)
        t.assert(contents instanceof Obj, 'is an instance of Obj')
        t.assert(contents.length > 0, 'content is not null or empty')
        t.end()
    })
}

function pageResources() {
    test('page resources', t => {
        const resources = page.getResources()
        t.assert(resources instanceof Obj, 'is instance ob Obj')
        t.assert(resources.length > 0, 'is not null or empty')
        t.end()
    })
}

function pageAddImg() {
    test('add image', t => {
        const painter = new Painter(),
            img = new Image(doc, '/home/red/test.jpg')

        painter.page = page
        painter.drawImage(img, 0, page.height - img.getHeight())
        painter.finishPage()
        if (existsSync('./img.out.pdf'))
            unlinkSync('./img.out.pdf')
        doc.write('./img.out.pdf')

        // todo: finish extracting img, requires stream wrapper completion
        let objs = doc.getObjects()
        for (let i = 0; i < objs.length; i++) {
            let o = objs[i]
            if (o.type === 'Dictionary') {
                let objDict = o.asDictionary(),
                    objType = objDict.hasKey('Type') ? objDict.getKey('Type') : null,
                    objSubType = objDict.hasKey('SubType') ? objDict.getKey('SubType') : null

                if ((objType && objType.type === 'Name') ||
                    (objSubType && objSubType.type === 'Name')) {

                    if ((objType && (objType as Obj).asName() === 'XObject' ) || (objSubType && (objSubType as Obj).asName() === 'Image')) {
                        let imgObj = o.asDictionary().hasKey('Filter') ? o.asDictionary().getKey('Filter') : null

                        if (imgObj && imgObj.type === 'Array') {
                            if (imgObj.asArray().length === 1) {
                                if (imgObj.asArray().at(0).type === 'Name') {
                                    if (imgObj.asArray().at(0).asName() === 'DCTDecode') {
                                        extractImg(o, true)
                                        return
                                    }
                                }
                            }
                        }
                        else if (imgObj && imgObj.type === 'Name' && imgObj.asName() === 'DCTDecode') {
                            extractImg(o, true)
                            return
                        }
                        else {
                            extractImg(o, false)
                            return
                        }
                    }
                }
            }
        }

        function extractImg(obj: Obj, jpg: Boolean) {
            let ext = jpg ? '.jpg' : '.ppm'
            t.end()
        }

        t.fail('should have been able to find the DCTDecode object')
    })
}

function runTest(test: Function) {
    setImmediate(() => {
        global.gc()
        test()
    })
}

[
    pageRotation,
    pageProperties,
    pageTrimBox,
    pageFields,
    pageFieldInfoArray,
    pageGetField,
    pageGetFields,
    pageGetAnnotsCount,
    pageGetAnnot,
    pageContents,
    pageResources,
    pageAddImg
].map(i => runTest(i))
