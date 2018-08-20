import { join } from 'path'
import * as tap from 'tape'
import { nopodofo as npdf, NPDFFieldType, NPDFAnnotation } from '../../'
import { unlinkSync } from "fs";
if (!global.gc) {
    global.gc = () => { }
}
const filePath = join(__dirname, '../test-documents/iss.16.checkbox-field-state-options.pdf')
const outfile = join(__dirname, '../tmp/npdf.values.test.pdf')

const streamTestFile = join(__dirname, '../tmp/fields.create.pdf')
const simpleFormTestFile = join(__dirname, '../tmp/simple-form.pdf')
const simpleFormOutFile = join(__dirname, '../tmp/simple-form.out.pdf')
/**
 * @todo: Add assertions
 */
tap('StreamDocument create simple form', t => {
    // instantiate required objects: doc, painter, font, xobj, page, annot, and field
    const doc = new npdf.StreamDocument(streamTestFile)
    let courier = doc.createFont({ fontName: 'Courier' })
    const painter = new npdf.Painter(doc)
    const defaultPageMediaBox = new npdf.Rect(0, 0, 612, 792)
    const page = doc.createPage(defaultPageMediaBox)
    // const appearanceStream = doc.createXObject(new npdf.Rect(0, 0, 612, 792))
    const nameLabel = 'First name:'
    const nameFieldAnnot = page.createAnnotation(
            NPDFAnnotation.Widget,
            new npdf.Rect(100 + courier.stringWidth(nameLabel), 500, 100, 40))

    // Add field label to page
    painter.setPage(page)
    painter.setColor([0.0, 0.0, 0.0])
    courier.size = 11
    painter.font = courier
    painter.drawText({ x: 100, y: 500 }, nameLabel)
    painter.finishPage()

    const nameField = page.createField( NPDFFieldType.TextField, nameFieldAnnot, doc.form)
    nameField.fieldName = 'FirstName'

    doc.close()
    t.end()
})
tap.skip('fill and flatten fields', t => {
    // Create a more effecient pattern (load -> write -> load -> write ...)
    const doc = new npdf.Document()
    doc.load(streamTestFile, e => {
        if(e) t.fail(e.message)
        let page = doc.getPage(0)
        let fields = page.getFields()
        doc.form.needAppearances = true
        t.assert(fields.length > 0) // fields should have one item
        t.assert(fields[0].type === NPDFFieldType.TextField);
        (fields[0] as npdf.TextField).text = 'TEST'
        // NOTE: doc.form appearances has NOT been set to true, this is using the annotations appearances stream
        doc.write(simpleFormTestFile, e => {
            if(e) t.fail(e.message)
            else {
                const interim = new npdf.Document()
                interim.load(simpleFormTestFile, e => {
                    if(e) t.fail(e.message)
                    else {
                        let page = interim.getPage(0)
                        page.flattenFields()
                        interim.write(simpleFormOutFile, e => {
                            if(e) t.fail(e.message)
                            let output = new npdf.Document()
                            output.load(simpleFormOutFile, e => {
                                if(e) t.fail(e.message)
                                else {
                                    let page = output.getPage(0)
                                    t.assert(page.getFields().length === 0)
                                    // todo: how to test the flattened field is written to the page?
                                    t.end()
                                }
                            })
                        })
                    }
                })
            }
        })        
    })
})
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
                        (field as npdf.TextField).text = 'TEST'
                        break
                    case NPDFFieldType.Checkbox:
                        (field as npdf.Checkbox).checked = true
                        break
                    case NPDFFieldType.ComboBox:
                        (field as npdf.ComboBox).selected = 0 // set to first value
                        break
                    default:
                        break
                }
            })
            t.test('fields persist', t => {
                doc.write(outfile, (err, data) => {
                    if (err) t.fail(err.message)
                    else {
                        let fieldsDoc = new npdf.Document()
                        fieldsDoc.load(outfile, () => {
                            let tPage = fieldsDoc.getPage(0)
                            tPage.getFields().forEach(field => {
                                switch (field.type) {
                                    case NPDFFieldType.TextField:
                                        let text = field as npdf.TextField
                                        t.assert(text.text === 'TEST')
                                        break
                                    case NPDFFieldType.Checkbox:
                                        let check = field as npdf.Checkbox
                                        t.true(check.checked)
                                        break
                                    case NPDFFieldType.ComboBox:
                                        let lf = field as npdf.ComboBox
                                        t.assert(lf.selected === 0)
                                        break
                                    default:
                                        break
                                }
                            })
                            t.end()
                        })
                    }
                })
            })
            t.end()
        }
    })

})

tap.onFinish(() => {
    unlinkSync(outfile)
})