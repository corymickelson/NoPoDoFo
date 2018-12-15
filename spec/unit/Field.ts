import {AsyncTest, Expect, Test, TestFixture} from 'alsatian'
import {nopodofo as npdf, NPDFFieldType, NPDFAnnotation} from '../../'
import {join} from 'path'

@TestFixture('Form Field')
export class FieldSpec {
    private readonly filePath = join(__dirname, '../test-documents/iss.16.checkbox-field-state-options.pdf')
    private readonly outfile = join(__dirname, '../tmp/npdf.values.test.pdf')
    private readonly streamTestFile = join(__dirname, '../tmp/fields.create.pdf')

    @Test('Create Simple Form')
    public createSimpleForm() {
        const doc = new npdf.StreamDocument(this.streamTestFile)
        let courier = doc.createFont({fontName: 'Courier'})
        const painter = new npdf.Painter(doc)
        const defaultPageMediaBox = new npdf.Rect(0, 0, 612, 792)
        const page = doc.createPage(defaultPageMediaBox)
        const nameLabel = 'First name:'
        const nameFieldAnnot = page.createAnnotation(
            NPDFAnnotation.Widget,
            new npdf.Rect(100 + courier.stringWidth(nameLabel), 500, 100, 40))

        // Add field label to page
        painter.setPage(page)
        const black = new npdf.Color(1.0)
        painter.setColor(black)
        courier.size = 11
        painter.font = courier
        painter.drawText({x: 100, y: 500}, nameLabel)
        painter.finishPage()

        const nameField = page.createField(NPDFFieldType.TextField, nameFieldAnnot, doc.form)
        nameField.fieldName = 'FirstName'
        const red = new npdf.Color(1.0, 0.0, 0.0)
        nameField.setBackgroundColor(red)
        doc.close()
    }

    @AsyncTest('Fields')
    public async simpleFieldsTest() {
        return new Promise((resolve, reject) => {

            const doc = new npdf.Document()
            doc.load(this.filePath, (e: Error) => {
                if (e) Expect.fail(e.message)
                else {
                    const page = doc.getPage(0)
                    const fields = page.getFields()
                    fields.forEach(field => {
                        Expect(field).toBeDefined()
                        Expect(field.type).toBeDefined()
                        Expect(field.fieldName).toBeDefined()
                        Expect(typeof field.exported === 'boolean').toBeTruthy()
                        Expect(typeof field.readOnly === 'boolean').toBeTruthy()
                        Expect(typeof field.required === 'boolean').toBeTruthy()
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
                    doc.write(this.outfile, (err, data) => {
                        if (err) Expect.fail(err.message)
                        else {
                            let fieldsDoc = new npdf.Document()
                            fieldsDoc.load(this.outfile, () => {
                                let tPage = fieldsDoc.getPage(0)
                                tPage.getFields().forEach(field => {
                                    switch (field.type) {
                                        case NPDFFieldType.TextField:
                                            let text = field as npdf.TextField
                                            Expect(text.text).toEqual('TEST')
                                            break
                                        case NPDFFieldType.Checkbox:
                                            let check = field as npdf.Checkbox
                                            Expect(check.checked).toBeTruthy()
                                            break
                                        case NPDFFieldType.ComboBox:
                                            let lf = field as npdf.ComboBox
                                            Expect(lf.selected).toEqual(0)
                                            break
                                        default:
                                            break
                                    }
                                })
                                return resolve()
                            })
                        }
                    })
                }
            })

        })
    }
}
