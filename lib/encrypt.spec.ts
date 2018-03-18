import * as tap from 'tape'
import {join} from "path"
import {EncryptOption, IEncrypt} from "./encrypt"
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

    sub.test('Can read encrypted document, pwd provided', standard => {
        const encryptDoc = new Document(secureDoc, false, 'secret')
        encryptDoc.on('ready', e => {
            if (e instanceof Error) {
                standard.fail(`error thrown: ${e.message}`)
                return
            }
            const encryptSubject = encryptDoc.encrypt as IEncrypt
            standard.ok(encryptSubject, 'encrypt object is NOT null')
            standard.true(encryptSubject.protections.hasOwnProperty('Edit') &&
                encryptSubject.protections.Edit === true,
                'Encrypt protections enforced, allowed with password')
            standard.false(
                encryptSubject.protections.Copy,
                'Non explicitly set protections default to false')
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
                if(e.message.match(/password required/gi).length > 0) {
                    encryptDoc.password = 'secret'
                    const encrypt = encryptDoc.encrypt as IEncrypt
                    standard.ok(encrypt)
                    standard.end()
                } else {
                    standard.fail('Password required error was NOT thrown')
                }

            })
    })

})
