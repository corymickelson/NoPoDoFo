import { unlink } from 'fs'
import { join } from 'path'
import * as test from 'tape'
import { Document, IPage } from './document'

const filePath = join(__dirname, '../test.pdf')
const outFile = './test.out.pdf'
test('Can load a pdf', t => {
    let doc: Document
    try {
        doc = new Document(filePath)
    }
    catch (e) {
        t.fail()
    }
    t.end()
})
test('document get page count', t => {
    const doc = new Document(filePath)
    const pc = doc.getPageCount()
    t.ok(pc)
    t.assert(pc > 0)
    t.end()
})
test('document delete page', t => {
    const doc = new Document(filePath),
        pc = doc.getPageCount()

    doc.deletePage(pc - 1)
    doc.write(outFile)

    const docLessOne = new Document(outFile),
        docLessOnePageCount = docLessOne.getPageCount()
    t.assert(pc === docLessOnePageCount + 1)
    cleanUp(t.end)
})
test('document merger', t => {
    const doc = new Document(filePath),
        originalPC = doc.getPageCount()

    doc.mergeDocument(filePath)
    doc.write(outFile)

    const doc2 = new Document(outFile),
        doc2PC = doc2.getPageCount()

    t.assert(originalPC * 2 === doc2PC, "merged document")
    cleanUp(t.end)
})
test('get page', t => {
    const doc = new Document(filePath),
        page = doc.getPage(0) // pages are zero indexed
    t.ok(page)
    t.end()
})
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
function cleanUp(cb:Function) {
    unlink(outFile, e => {
        if (e) throw e
       cb() 
    })
}