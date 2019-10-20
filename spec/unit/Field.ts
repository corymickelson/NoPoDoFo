import {AsyncTest, Expect, Test, TestCase, TestFixture, Timeout} from 'alsatian'
import {nopodofo, NPDFAnnotation, NPDFFieldType, NPDFName, NPDFPaintOp} from '../../'
import {join} from 'path'
import Dictionary = nopodofo.Dictionary;

@TestFixture('Form Field')
export class FieldSpec {
    private readonly filePath = join(__dirname, '../test-documents/iss.16.checkbox-field-state-options.pdf')
    private readonly outfile = join(__dirname, '../tmp/nopodofo.values.test.pdf')
    private readonly streamTestFile = join(__dirname, '../tmp/fields.create.pdf')

    @Test('Create Simple Form')
    public createSimpleForm() {
        const doc = new nopodofo.StreamDocument(this.streamTestFile)
        let courier = doc.createFont({fontName: 'Courier'})
        const painter = new nopodofo.Painter(doc)
        const defaultPageMediaBox = new nopodofo.Rect(0, 0, 612, 792)
        const page = doc.createPage(defaultPageMediaBox)
        const nameLabel = 'First name:'
        const nameFieldAnnot = page.createAnnotation(
            NPDFAnnotation.Widget,
            new nopodofo.Rect(100 + courier.stringWidth(nameLabel), 500, 100, 40))

        nameFieldAnnot.color = new nopodofo.Color(0, 0, 1)
        nameFieldAnnot.setBorderStyle({horizontal: 1, vertical: 1, width: 1})
        // Add field label to page
        painter.setPage(page)
        const black = new nopodofo.Color(1.0)
        painter.setColor(black)
        courier.size = 11
        painter.font = courier
        painter.drawText({x: 100, y: 500}, nameLabel)
        painter.finishPage()

        const nameField = nopodofo.Page.createField(NPDFFieldType.TextField, nameFieldAnnot, doc.form);
        nameField.fieldName = 'FirstName'
        const red = new nopodofo.Color(1.0, 0.0, 0.0)
        nameField.setBackgroundColor(red)
        doc.close()
    }

    @AsyncTest('Fields')
    public async simpleFieldsTest() {
        return new Promise((resolve) => {

            const doc = new nopodofo.Document()
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
                        switch (field.type) {
                            case NPDFFieldType.TextField:
                                (field as nopodofo.TextField).text = 'TEST'
                                break
                            case NPDFFieldType.Checkbox:
                                (field as nopodofo.Checkbox).checked = true
                                break
                            case NPDFFieldType.ComboBox:
                                (field as nopodofo.ComboBox).selected = 0 // set to first value
                                break
                            default:
                                break
                        }
                    })
                    doc.write(this.outfile, (err) => {
                        if (err) Expect.fail(err.message)
                        else {
                            let fieldsDoc = new nopodofo.Document()
                            fieldsDoc.load(this.outfile, () => {
                                let tPage = fieldsDoc.getPage(0)
                                tPage.getFields().forEach(field => {
                                    switch (field.type) {
                                        case NPDFFieldType.TextField:
                                            let text = field as nopodofo.TextField
                                            Expect(text.text).toEqual('TEST')
                                            break
                                        case NPDFFieldType.Checkbox:
                                            let check = field as nopodofo.Checkbox
                                            Expect(check.checked).toBeTruthy()
                                            break
                                        case NPDFFieldType.ComboBox:
                                            let lf = field as nopodofo.ComboBox
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

    @AsyncTest('Field refresh AP')
    @Timeout(100000)
    @TestCase(0, 'Helvetica', null, new nopodofo.Color(1.0, 0.0, 0.0))
    @TestCase(1, 'FiraCode', join(__dirname, '../test-documents/FiraCode_Regular.ttf'), new nopodofo.Color(0.0, 1.0, 0.0))
    @TestCase(2, 'Courier', null, new nopodofo.Color(0.0, 0.0, 1.0))
    public async refreshAppearances(idx: number, name:string, file:string, color:nopodofo.Color) {
        return new Promise((resolve, reject) => {
            const doc = new nopodofo.Document()
            doc.load(join(__dirname, '../test-documents/test.pdf'), err => {
                if (err) Expect.fail(err.message)
                const p1 = doc.getPage(0)
                const field = p1.getField<nopodofo.TextField>(idx)
                if(field.type !== NPDFFieldType.TextField) {
                    return resolve()
                }
                field.text = 'TESTING'
                field.readOnly = true
                const font = file ? doc.createFont({
                    fontName: name,
                    fileName: file,
                    bold: false,
                    embed: true
                }) : doc.createFont({
                    fontName: name,
                    bold: false,
                    embed: true
                })
                font.size = 12
                const daStr = `${color.getColorStreamString()} /${font.identifier} ${font.size} ${NPDFPaintOp.FontAndSizeOp}`
                // set the default appearance
                if (doc.form.needAppearances) {
                    if (doc.form.DA === undefined || doc.form.DA === '') {
                        doc.form.DA = daStr
                    }
                    field.DA = doc.form.DA
                } else {
                    doc.form.DA = daStr
                }

                // refresh the field with the new DA value
                // this method will take the DA stream and apply it to this fields AP stream
                // this pattern is a good practice as not all pdf readers conform to the rendering specification
                // for default appearances
                field.refreshAppearanceStream()
                const tmp = join(__dirname, `../tmp/refresh-appearances_${idx}.pdf`)
                doc.write(tmp, err => {
                    if (err) {
                        return reject(err)
                    }
                    const td = new nopodofo.Document()
                    td.load(tmp, e => {
                        if (e) return reject(e)
                        const tf = td.getPage(0).getField<nopodofo.TextField>(idx)
                        const n = doc.getObject((tf.AP as Dictionary).getKey(NPDFName.N))
                        if (!n.hasStream()) {
                            Expect.fail("Stream not found")
                        }
                        const apStream = new nopodofo.Stream(n)
                        const streamContent = apStream.copy(true)
                        Expect(streamContent.toString().replace(/[ \n]/ig, '').match(/\/TxBMCqBT[0-1]{3}rg\/Ft[0-9]{1,2}2Tf2\.02\.0Td\(TESTING\)TjETQEMC/)).toBeTruthy()
                        return resolve()
                    })
                })
            })
        })
    }
}
