import { IForm } from './form'
import { NPDFName as name } from './names'
import { Document, __mod } from './document'
import { join } from 'path'
import * as tap from 'tape'
import { Font } from '.';

const filePath = join(__dirname, '../test-documents/test.pdf')

tap('NPDF Form Accessorts and Methods', standard => {
    let doc = new Document(filePath)
    doc.on('ready', e => {
        if (e instanceof Error) {
            standard.fail(e.message)
        }
        let acroformKeys = doc.form.dictionary.getKeys()
        standard.assert(
            [name.DA, name.DR, name.FIELDS, name.SIG_FLAGS, name.XFA]
                .every(i => acroformKeys.includes(i)),
            'AcroForm Dictionary contains all expected keys')

        standard.assert((doc.form.DA as string).includes('Helv'), 'test.pdf DA shoudl be set to Helv')
        standard.assert((doc.form.DR as any) instanceof __mod.Dictionary)
        standard.assert((doc.form.Fonts as Font[]).length > 0, 'Fonts contains at least one font')
        standard.assert(doc.form.needAppearances === false)
        standard.end()
    })
})