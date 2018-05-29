import * as tap from 'tape'
import {join} from "path"
import {IEncrypt,npdf} from "../../dist"

const filePath = join(__dirname, '../test-documents/test.pdf'),
    secureDoc = join(__dirname, '../test-documents/secure.pdf')

tap('IEncrypt', t => {
    let doc = new npdf.Document()
    doc.load(filePath, e => {
        if (e) t.fail(e.message)
        else {
            let enc:IEncrypt = npdf.Encrypt.createEncrypt({
                ownerPassword: 'secret',
                userPassword: 'secret',
                keyLength: 40,
                protection: ['Edit', 'FillAndSign'],
                algorithm: 'aesv3'
            })
            t.assert(enc.isAllowed('Edit') === true, 'Edit allowed')
            t.assert(enc.isAllowed('FillAndSign') === true, 'fill and sign allowed')
            t.assert(enc.keyLength === 40)
            t.end()
        }
    })
})

