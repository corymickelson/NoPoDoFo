import {join} from 'path'
import * as tap from 'tape'
import {__mod} from './document'
import {ICheckBox, IComboBox, IField, ITextField} from './field'
import {unlinkSync} from "fs";

const filePath = join(__dirname, '../test-documents/iss.16.checkbox-field-state-options.pdf')
tap('IField', t => {
    const doc = new __mod.Document()
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
                switch (field.typeAsString) {
                    case 'TextField':
                        (field  as ITextField).text = 'TEST'
                        break
                    case 'CheckBox':
                        (field as ICheckBox).checked = true
                        break
                    case 'ComboBox':
                        (field as IComboBox).selected = 0 // set to first value
                        break
                    case 'ListBox':
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
                        let fieldsDoc = new __mod.Document()
                        fieldsDoc.load(outfile, () => {
                            let tPage = fieldsDoc.getPage(0)
                            tPage.getFields().forEach(field => {
                                switch (field.typeAsString) {
                                    case 'TextField':
                                        let text = field as ITextField
                                        t.assert(text.text === 'TEST')
                                        break
                                    case 'CheckBox':
                                        let check = field as ICheckBox
                                        t.true(check.checked)
                                        break
                                    case 'ComboBox':
                                        let lf =field as IComboBox
                                        t.assert(lf.selected === 0)
                                        break
                                    case 'ListBox':
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
// tap.skip('Fields', sub => {
//     const doc = new Document(filePath)
//     doc.on('ready', e => {
//         if (e instanceof Error) sub.fail()
//
//         const page = doc.getPage(0),
//             fields = page.getFields()
//
//         sub.test('variants', standard => {
//
//             standard.test('create field instance from existing pdf field', t => {
//                 let field = page.getField(0)
//                 t.assert(field !== null)
//                 t.assert(field instanceof Field)
//                 console.log(field.type)
//                 t.assert(field.type === 'TextField')
//                 t.end()
//             })
//             standard.test('Can instantiate a TextField given a field of type TextField', t => {
//                 let field = page.getField(0)
//                 const text = new TextField(field)
//                 t.ok(text)
//                 t.end()
//             })
//             standard.test('Can instantiate a Checkbox given a field of type CheckBox', t => {
//                 let index = -1
//                 for (let i = 0; i < fields.length; i++) {
//                     let type = fields[i].type
//                     if (type === 'CheckBox') {
//                         index = i
//                         break
//                     }
//                 }
//                 if (index === -1) {
//                     console.warn(`Fields variant Checkbox was not found in document ${filePath}`)
//                     t.end()
//                 } else {
//                     const field = page.getField(index)
//                     t.assert(field.type === 'CheckBox')
//                     const checkbox = new CheckBox(field)
//                     t.ok(checkbox)
//                     t.assert(checkbox.checked === false)
//                     t.end()
//                 }
//
//             })
//
//             standard.test('Can instantiate a ListBox', t => {
//                 let index = -1
//                 for (let i = 0; i < fields.length; i++) {
//                     let type = fields[i].type
//                     if (type === 'ListBox') {
//                         index = i
//                         break
//                     }
//                 }
//                 if (index === -1) {
//                     console.warn(`Fields variant ListBox was not found in document ${filePath}`)
//                     t.end()
//                 } else {
//                     const field = page.getField(index)
//                     t.assert(field.type === 'ListField')
//                     const list = new ListBox(field)
//                     t.ok(list)
//                     t.assert(list.selected === -1)
//                     t.end()
//                 }
//
//             })
//             standard.test('ComboBox variant', t => {
//                 let index = -1
//                 for (let i = 0; i < fields.length; i++) {
//                     let type = fields[i].type
//                     if (type === 'ComboBox') {
//                         index = i
//                         break
//                     }
//                 }
//                 if (index === -1) {
//                     console.warn(`Fields variant ComboBox was not found in document ${filePath}`)
//                     t.end()
//                 } else {
//                     const field = page.getField(index)
//                     t.assert(field.type === 'ComboBox')
//                     const combo = new ComboBox(field)
//                     t.ok(combo)
//                     t.assert(combo.selected === -1)
//                     t.end()
//                 }
//             })
//         })
//     })
// })


