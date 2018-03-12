import * as tap from 'tape'
import {join} from "path"
import {Encrypt, EncryptOption} from "./encrypt"
import {Document} from "./document"
import {access, unlinkSync} from "fs";
import {F_OK} from "constants";

const filePath = join(__dirname, '../test-documents/test.pdf'),
    secureDoc = join(__dirname, '../test-documents/secure.pdf')

tap('encrypt api', sub => {
    sub.test('can persist encrypted document', standard => {
        const doc = new Document(filePath),
            encryptionOption: EncryptOption = {
                ownerPassword: 'secret',
                userPassword: 'secret',
                keyLength: 40,
                protection: ['Edit', 'FillAndSign'],
                algorithm: 'aesv3'
            },
            secureDoc = join(__dirname, '../test-documents/secure.tmp.pdf')
        doc.on('ready', () => {
            doc.createEncrypt(encryptionOption)
            doc.write(e => {
                if (e instanceof Error) standard.fail()
                else {
                    access(secureDoc, F_OK, err => {
                        if (err) standard.fail('Wrote bad document')
                        else {
                            standard.pass('Can create encrypted document')
                            unlinkSync(secureDoc)
                            standard.end()
                        }
                    })
                }
            }, secureDoc)
        })
    })
    //     const encryptDoc = new Document(join(__dirname, '../test-documents/secure.pdf'), false, 'secret')
    // encryptDoc.on('ready', (pdf: Document) => {
    //
    // })
    sub.test('Can read encrypted document, pwd provided', standard => {
        const encryptDoc = new Document(secureDoc, false, 'secret')
        encryptDoc.on('ready', e => {
            if (e instanceof Error) {
                standard.fail(`error thrown: ${e.message}`)
                return
            }
            const encryptObj = new Encrypt(encryptDoc)
            standard.ok(encryptObj, 'encrypt object is NOT null')
            standard.true((encryptObj as Encrypt).isAllowed('Edit'), 'Encrypt protections enforced, allowed with password')
            standard.false((encryptObj as Encrypt).isAllowed('Copy'), 'Unset protections not allowed')
            standard.end()
        })
            .on('error', () => standard.fail('Failed to throw error warning'))
    })
    sub.test('Can read encrypted document, pwd NOT provided', standard => {
        const encryptDoc = new Document(secureDoc)
        encryptDoc.on('ready', e  => {
            if (e instanceof Error) {
                standard.fail(`error thrown: ${e.message}`)
            }
        })
            .on('error', e => {
                standard.pass('error thrown, password required')
                encryptDoc.password = 'secret'
                const encrypt: Encrypt = encryptDoc.encrypt as Encrypt
                standard.ok(encrypt)
                standard.end()
            })
    })

})
