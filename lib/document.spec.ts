import { unlink } from 'fs'
import { join } from 'path'
import * as test from 'tape'
import { Document, DocumentEncryptOption } from './document'

const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    uPwd = 'secret',
    pwdDoc = join(__dirname, '../pwd.pdf')

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

test('document requires password', t => {
    let doc: Document
    try {
        doc = new Document(pwdDoc)
    }
    catch (e) {
        t.ok(e instanceof Error, 'should throw an error')
        t.assert((e.message as string).includes("Password"), 'password required error')
        t.end()
    }
})

test.skip('is not a pdf document', t => {})
test('throws JS error on file not found', t => {
    let doc:Document
    try {
        doc = new Document('/bad/path')
    } catch (e) {
        t.ok(e instanceof Error, 'should throw error')
        t.end();
    }
})

test.skip('objects', t => {})

//todo read the trailer object for the encrypt key for test success criterial
test.skip('encryption: user password', t => {
    const doc = new Document(filePath),
        encryptionOption:DocumentEncryptOption = {
            userPassword: 'secret',
            keyLength: 40
        },
        secureDoc = join(__dirname, '../secure.pdf')
    doc.setEncrypt(encryptionOption)
    doc.write(secureDoc)

    let sDoc:Document
    try {
        sDoc = new Document(secureDoc)
    } catch (e) {
        t.ok(e instanceof Error, 'should throw a password required error')
        t.assert((e.message as string).includes('Password'), 'is a password required error')
        t.doesNotThrow(() => {doc.password = 'secret'}, 'setting password does not throw')
        t.end()
    }
    t.end('Should have thrown error for requiring password')
})
test.skip('encryption: owner password', t => {})
test.skip('encryption: protection => FillAndSign', t => {})
test.skip('encryption: invalid payload throws PdfError', t => {})
test.skip('write modes', t => {})

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

test('Document set encrypt', t => {
    t.end()
})

function cleanUp(cb: Function) {
    unlink(outFile, e => {
        if (e) throw e
        cb()
    })
}