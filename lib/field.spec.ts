import {join} from 'path'
import * as test from 'tape'
import {Document} from './document'
import {CheckBox, ComboBox, Field, ListBox, TextField} from './field'

//const filePath = join(__dirname, '../test-documents/test.pdf'),
const filePath = join(__dirname, '../test-documents/iss.16.checkbox-field-state-options.pdf'),
    outFile = './test.out.pdf'

test('create field instance from existing pdf field', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        const page = doc.getPage(0),
            field = page.getField(0)
        t.assert(field !== null)
        t.assert(field instanceof Field)
        console.log(field.getType())
        t.assert(field.getType() === 'TextField')
        t.end()
    })
})
test('Can instantiate a TextField given a field of type TextField', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        const page = doc.getPage(0),
            field = page.getField(0)
        const text = new TextField(field)
        t.ok(text)
        t.assert(text.text === '')
        t.end()
    })
})
test('Can instantiate a Checkbox given a field of type CheckBox', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        const page = doc.getPage(0)
        const fields = page.getFieldsInfo()
        let index = -1
        for (let i = 0; i < fields.length; i++) {
            let type = fields[i].type
            if (type === 'CheckBox') {
                index = i
                break
            }
        }
        if (index === -1) {
            console.warn(`Fields variant Checkbox was not found in document ${filePath}`)
            t.end()
        } else {
            const field = page.getField(index)
            t.assert(field.getType() === 'CheckBox')
            const checkbox = new CheckBox(field)
            t.ok(checkbox)
            t.assert(checkbox.checked === false)
            t.end()
        }

    })
})

test('Can instantiate a ListBox', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        const page = doc.getPage(0)
        const fields = page.getFieldsInfo()
        let index = -1
        for (let i = 0; i < fields.length; i++) {
            let type = fields[i].type
            if (type === 'ListBox') {
                index = i
                break
            }
        }
        if (index === -1) {
            console.warn(`Fields variant ListBox was not found in document ${filePath}`)
            t.end()
        } else {
            const field = page.getField(index)
            t.assert(field.getType() === 'ListField')
            const list = new ListBox(field)
            t.ok(list)
            t.assert(list.selected === -1)
            t.end()
        }

    })
})
test('Can instantiate a ComboBox', t => {
    const doc = new Document(filePath)
    doc.on('ready', () => {
        const page = doc.getPage(0)
        const fields = page.getFieldsInfo()
        let index = -1
        for (let i = 0; i < fields.length; i++) {
            let type = fields[i].type
            if (type === 'ComboBox') {
                index = i
                break
            }
        }
        if (index === -1) {
            console.warn(`Fields variant ComboBox was not found in document ${filePath}`)
            t.end()
        } else {
            const field = page.getField(index)
            t.assert(field.getType() === 'ComboBox')
            const combo = new ComboBox(field)
            t.ok(combo)
            t.assert(combo.selected === -1)
            t.end()
        }

    })
})
