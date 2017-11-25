import { unlinkSync } from 'fs'
import { join } from 'path'
import * as test from 'tape'
import { Document } from './document'
import { EncryptInitOption, Encrypt } from './encrypt'
import { TestCase } from 'tape';

const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = './test.out.pdf',
    uPwd = 'secret',
    pwdDoc = join(__dirname, '../test-documents/pwd.pdf')

test('can load pdf', t => {
    let doc = new Document(filePath)
    doc.on('ready', () => {
        t.assert(doc.loaded === true, 'doc loaded')
        t.end()
    })
        .on('error', e => {
            notCompiledErrorHandler(e, t)
        })
})
test('password protected, \'Password required\' error bubbles up', t => {
    let doc = new Document(pwdDoc)
    doc.on('ready', () => {
        t.fail('Should have thrown password required error')
    })
        .on('error', e => {
            t.ok(e instanceof Error, 'should bubble up error')
            t.assert(e.message === "Password required to modify this document")
            t.end()
        })
})
test('throws JS error on file not found', t => {
    t.throws(() => {
        let doc = new Document('/bad/path')
    })
    t.end()
})
test('encryption: user password', t => {
    const doc = new Document(filePath),
        encryptionOption: EncryptInitOption = {
            ownerPassword: 'secret',
            userPassword: 'secret',
            keyLength: 40,
            protection: ['Edit', 'FillAndSign'],
            algorithm: 'aesv3'
        },
        secureDoc = join(__dirname, '../test-documents/secure.pdf')
    doc.on('ready', () => {
        doc.encrypt = new Encrypt(null, encryptionOption)
        doc.write(e => {
            if (e) t.fail(e.message)
            let sDoc: Document = new Document(secureDoc)
            sDoc.on('ready', () => {
                t.fail("Password required error should have been thrown.")
            })
                .on('error', e => {
                    if (e instanceof Error) {
                        t.end()
                    }
                })
        }, secureDoc)
    })
    .on('error', e => notCompiledErrorHandler(e, t))
})
test('document get page count', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        const pc = doc.getPageCount()
        t.ok(pc)
        t.assert(pc > 0)
        t.end()

    })
    .on('error', e => notCompiledErrorHandler(e, t))
})

test('document delete page', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        let pc = doc.getPageCount()

        doc.deletePage(pc - 1)
        doc.write(e => {
            if (e) t.fail(e.message)
            const docLessOne = new Document(outFile)
            docLessOne.on('ready', () => {
                let docLessOnePageCount = docLessOne.getPageCount()
                t.assert(pc === docLessOnePageCount + 1)
                unlinkSync(outFile)
                t.end()
            })
            .on('error', e => notCompiledErrorHandler(e, t))
        }, outFile)
    })
        .on('error', e => notCompiledErrorHandler(e, t))
})

test('document merger', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        let originalPC = doc.getPageCount()
        doc.mergeDocument(filePath)
        doc.write(e => {
            if (e) t.fail(e.message)
            const doc2 = new Document(outFile)
            doc2.on('ready', () => {
                let doc2PC = doc2.getPageCount()
                t.assert(originalPC * 2 === doc2PC, "merged document")
                unlinkSync(outFile)
                t.end()
            })
                .on('error', e => notCompiledErrorHandler(e, t))
        }, outFile)
    })
        .on('error', e => notCompiledErrorHandler(e, t))
})

test('get page', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        let page = doc.getPage(0) // pages are zero indexed
        t.ok(page)
        t.end()
    })
        .on('error', e => notCompiledErrorHandler(e, t))
})

test('write buffer', t => {
    const doc = new Document(filePath)
    doc.on('ready', (pdf: Document) => {
        pdf.write((e, d) => {
            if (e) t.fail(e.message)
            else {
                t.ok(d)
                t.assert(d instanceof Buffer)
                t.assert((d as Buffer).length > 0)
                t.end()
            }
        })
    })
        .on('error', e => notCompiledErrorHandler(e, t))
})
export function notCompiledErrorHandler(e: Error, t: any) {
    if (e.message.includes('error: 49')) {
        t.pass('YOUR CURRENT PODOFO BUILD WAS NOT COMPILED WITH ENCRYPT. To use pdf encrypt recompile with the correct options')
    } else {
        t.fail(e)
    }
}