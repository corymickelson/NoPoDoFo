import {unlink, unlinkSync} from 'fs'
import {join} from 'path'
import * as test from 'tape'
import {Document} from './document'
import {Rect} from "./rect";
import {NPdfAnnotation, IAnnotation, Annotation} from "./annotation";
import {Obj} from "./object";
import {Field} from './field';

const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    doc = new Document(filePath),
    page = doc.getPage(0)

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

test('page instance properties', t => {
    t.assert(page.number === 1, 'page number valid')
    t.assert(page.width > 0, 'page width valid')
    t.assert(page.height > 0, 'page height valid')
    t.end()
})
test('get trimbox', t => {
    const trimBox = page.trimBox
    t.assert(trimBox.height > 0, 'trimbox height is not null')
    t.assert(trimBox.width > 0, 'trimbox width not null')
    t.assert(trimBox.left === 0, 'trimbox left')
    t.assert(trimBox.bottom === 0, 'trimbox bottom')
    t.end()
})
test('get number of fields', t => {
    let n = page.getNumFields()
    t.assert(typeof n === 'number')
    // t.assert(page.getNumFields() === 22, 'page get number of fields counts all fields')
    t.end()
})

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

test('get field', t => {
    const field = page.getField(0)

    t.assert(field instanceof Field, 'is an instance of Field')
    t.end()
})

test('get fields', t => {
    const fields = page.getFields()
    t.assert(Array.isArray(fields), 'returns an array')
    t.assert(fields.length === page.getNumFields(), 'is not an empty array')
    t.assert(fields[0] instanceof Field, 'contains instance(s) of Field')
    t.end()
})
test('page number of annotations', t => {
    t.assert(page.getNumAnnots() === 22, 'get\'s all annotations on the page')
    t.end()
})

test('page get annotation', t => {
    const annot = page.getAnnotation(0)
    t.ok(annot)
    t.assert(annot.getType() === 'Widget')
    t.end()
})

test('page create annotation', t => {
    const rect = new Rect([0, 0, 0, 0])

    t.assert(rect.height === 0)
    t.assert(rect.width === 0)
    page.createAnnotation(NPdfAnnotation.Widget, rect, (v: IAnnotation) => {
        t.ok(v)
        t.assert(v instanceof Annotation, 'is an instance of Annotation')
        t.assert(v.getType() === 'Widget')
        t.end()
    })
})

test('page contents', t => {
    const contents = page.getContents(false)
    t.assert(contents instanceof Obj, 'is an instance of Obj')
    t.assert(contents.length > 0, 'content is not null or empty')
    t.end()
})

test('page resources', t => {
    const resources = page.getResources()
    t.assert(resources instanceof Obj, 'is instance ob Obj')
    t.assert(resources.length > 0, 'is not null or empty')
    t.end()
})

function cleanUp(cb: Function) {
    unlink(outFile, e => {
        if (e) throw e
        cb()
    })
}
