import {unlink} from 'fs'
import {join} from 'path'
import * as test from 'tape'
import {Document} from './document'
import {Rect} from "./rect";
import {NPdfAnnotation} from "./annotation";
import {Obj} from "./object";
import { Field } from './field';

const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf'

test('set page rotation', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        originalRotation = page.rotation,
        newRotation = originalRotation + 90
    page.rotation = newRotation
    doc.write(outFile)

    const testDoc = new Document(outFile),
        testPage = testDoc.getPage(0)
    t.assert(testPage.rotation === newRotation, 'Page rotation updated')
    cleanUp(t.end)
})

test('page instance properties', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0)
    t.assert(page.number === 1, 'page number valid')
    t.assert(page.width > 0, 'page width valid')
    t.assert(page.height > 0, 'page height valid')
    t.end()
})
test('get trimbox', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        trimBox = page.trimBox
    t.assert(trimBox.height > 0, 'trimbox height is not null')
    t.assert(trimBox.width > 0, 'trimbox width not null')
    t.assert(trimBox.left === 0, 'trimbox left')
    t.assert(trimBox.bottom === 0, 'trimbox bottom')
    t.end()
})
test('get number of fields', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0)
    t.assert(page.getNumFields() === 22, 'page get number of fields counts all fields')
    t.end()
})

test('get fields info', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        fields = page.getFieldsInfo()

    t.assert(fields.length === 22, 'page get field info get\'s info for each field')
    t.end()
})

test('get field', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        field = page.getField(0)

    t.assert(field instanceof Field, 'is an instance of Field')
    t.end()
})

test('get fields', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        fields = page.getFields()
    t.assert(Array.isArray(fields), 'returns an array')
    t.assert(fields.length === page.getNumFields(), 'is not an empty array')
    t.assert(fields[0] instanceof Field, 'contains instance(s) of Field')
    t.end()
})
test('page number of annotations', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0)
    t.assert(page.getNumAnnots() === 22, 'get\'s all annotations on the page' )
    t.end()
})

test('page get annotation', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        annot = page.getAnnotation(0)
    t.ok(annot)
    t.assert(annot.getType() === 'Widget')
    t.end()
})

test('page create annotation', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        rect = new Rect([0, 0, 0, 0])

    t.assert(rect.height === 0)
    t.assert(rect.width === 0)
    const annot = page.createAnnotation(NPdfAnnotation.Widget, rect)
    t.ok(annot)
    t.assert(annot.getType() === 'Widget')
    t.end()
})

test('page contents', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        contents = page.getContents(false)
    t.assert(contents instanceof Obj, 'is an instance of Obj')
    t.assert(contents.length > 0, 'content is not null or empty')
    t.end()
})

test('page resources', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        resources = page.getResources()
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
