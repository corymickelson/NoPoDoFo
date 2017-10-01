import {unlink} from 'fs'
import {join} from 'path'
import * as test from 'tape'
import {Document} from './document'

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
    t.assert(testPage.rotation === newRotation)
    cleanUp(t.end)
})

test('page instance properties', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0)
    t.assert(page.number === 1)
    t.assert(page.width > 0)
    t.assert(page.height > 0)
    t.end()
})
test('get trimbox', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        trimBox = page.trimBox
    t.assert(trimBox.height > 0)
    t.assert(trimBox.width > 0)
    t.assert(trimBox.left === 0)
    t.assert(trimBox.bottom === 0)
    t.end()
})
test('get number of fields', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0)
    t.assert(page.getNumFields() === 22)
    t.end()
})

test('get fields', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0),
        fields = page.getFields()

    t.assert(fields.length === 22)
    t.end()
})

test('page number of annotations', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0)
    t.assert(page.getNumAnnots() === 22)
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

function cleanUp(cb: Function) {
    unlink(outFile, e => {
        if (e) throw e
        cb()
    })
}
