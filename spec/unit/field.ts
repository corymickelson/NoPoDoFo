import {join} from 'path'
import * as tap from 'tape'
import {npdf, ICheckBox, IComboBox, IField, ITextField, NPDFFieldType} from '../../dist'
import {unlinkSync} from "fs";
if(!global.gc) {
    global.gc = () => {}
}
const filePath = join(__dirname, '../test-documents/iss.16.checkbox-field-state-options.pdf')
tap('IField', t => {
    const doc = new npdf.Document()
    doc.load(filePath, (e: Error) => {
        if (e) t.fail(e.message)
        else {
            const page = doc.getPage(0)
            const fields = page.getFields()
            fields.forEach(field => {
                t.ok(field)
                t.ok(field.type)
                t.ok(field.fieldName)
                t.ok(typeof field.exported === 'boolean')
                t.ok(typeof field.readOnly === 'boolean')
                t.ok(typeof field.required === 'boolean')
                switch (field.type) {
                    case NPDFFieldType.TextField:
                        (field  as ITextField).text = 'TEST'
                        break
                    case NPDFFieldType.CheckBox:
                        (field as ICheckBox).checked = true
                        break
                    case NPDFFieldType.ComboBox:
                        (field as IComboBox).selected = 0 // set to first value
                        break
                    default:
                        break
                }
            })
            t.test('fields persist', t => {

                let outfile = '/tmp/npdf.values.test.pdf'
                doc.write(outfile, (err, data) => {
                    if (err) t.fail(err.message)
                    else {
                        let fieldsDoc = new npdf.Document()
                        fieldsDoc.load(outfile, () => {
                            let tPage = fieldsDoc.getPage(0)
                            tPage.getFields().forEach(field => {
                                switch (field.type) {
                                    case NPDFFieldType.TextField:
                                        let text = field as ITextField
                                        t.assert(text.text === 'TEST')
                                        break
                                    case NPDFFieldType.CheckBox:
                                        let check = field as ICheckBox
                                        t.true(check.checked)
                                        break
                                    case NPDFFieldType.ComboBox:
                                        let lf =field as IComboBox
                                        t.assert(lf.selected === 0)
                                        break
                                    default:
                                        break
                                }
                            })
                            t.end()
                            unlinkSync(outfile)
                        })
                    }
                })
            })
            t.end()
        }
    })
})

