import {Expect, AsyncTest, TestFixture, TestCase, AsyncSetup, AsyncTeardown, Timeout} from 'alsatian'
import {nopodofo, NPDFFontEncoding, NPDFName as name} from '../../'
import {join} from "path"

@TestFixture('Acro Form')
export class FormSpec {

    private readonly filePath = join(__dirname, '../test-documents/test.pdf')

    @AsyncTest('text field custom AP')
    public async memDocTextFieldAP() {
        return new Promise((resolve, reject) => {
            const doc = new nopodofo.Document()
            doc.load(join(__dirname, '../test-documents/test.pdf'), err => {
                if (err) Expect.fail(err.message)
                const p1 = doc.getPage(0)
                const field = p1.getField<nopodofo.TextField>(0)
                const value = 'TESTING'
                field.text = value
                const painter = new nopodofo.Painter(doc)
                const xRect = new nopodofo.Rect(0, 0, field.widgetAnnotation.rect.width, field.widgetAnnotation.rect.height)
                const xobj = doc.createXObject(xRect)
                painter.setPage(xobj)
                painter.setClipRect(xRect)
                painter.save()
                const black = new nopodofo.Color(1.0, 1.0, 1.0)
                painter.setColor(black)
                painter.restore()
                const firaCode = doc.createFont({
                    fontName: 'Fira Code',
                    fileName: join(__dirname, '../test-documents/FiraCode_Regular.ttf'),
                    bold: false,
                    embed: true
                })
                firaCode.size = 12
                painter.font = firaCode
                painter.beginText({x: 0, y: 5})
                painter.setStrokeWidth(20)
                painter.addText(value)
                painter.endText()
                painter.finishPage()
                field.readOnly = true
                const apDict = new nopodofo.Dictionary()
                apDict.addKey(name.N, xobj.reference)
                field.obj.getDictionary().addKey('Test', true)
                Expect(field.obj.getDictionary().getKeys().indexOf('Test')).toBeGreaterThan(-1)
                field.AP = apDict
                const daStr = `0 0 0 rg /${firaCode.identifier} tf`
                field.DA = daStr
                const tmp = join(__dirname, '../tmp/form_ap_test.pdf')
                doc.write(tmp, err => {
                    if (err) {
                        return reject(err)
                    }
                    const td = new nopodofo.Document()
                    td.load(tmp, e => {
                        if (e) return reject(e)
                        const tf = td.getPage(0).getField<nopodofo.TextField>(0)
                        Expect(tf.DA).toEqual(daStr)
                        return resolve()
                    })
                })
            })
        })
    }

    @AsyncTest('Accessors & Methods')
    public async instanceProperties() {
        return new Promise(resolve => {

            let doc = new nopodofo.Document()
            doc.load(this.filePath, e => {
                if (e instanceof Error) {
                    Expect.fail(e.message)
                }
                let acroformKeys = doc.form.dictionary.getKeys()
                Expect(
                    [name.DA, name.DR, name.FIELDS, name.SIG_FLAGS, name.XFA]
                        .every(i => acroformKeys.includes(i))).toBeTruthy()
                Expect((doc.form.DA as string).includes('Helv')).toBeTruthy()
                Expect((doc.form.DR as any) instanceof (nopodofo.Dictionary as any)).toBeTruthy()
                Expect(doc.form.needAppearances).toBe(false)
                let dr = doc.form.DR as nopodofo.Dictionary
                let font = doc.createFont({fontName: 'Helvetica', encoding: NPDFFontEncoding.WinAnsi})
                let fontObj = dr.hasKey(name.FONT) ? dr.getKey<nopodofo.Object>(name.FONT, false) : null
                let fontDict: nopodofo.Dictionary
                if (!fontObj) Expect.fail('Font Object undefined')
                else {
                    fontDict = fontObj.getDictionary()
                    fontDict.addKey(font.identifier, font.object)
                    Expect(fontDict.getKeys().includes(font.identifier)).toBeTruthy()
                    let da = `0 0 0 rg /${font.identifier} ${font.size} Tf`
                    doc.form.DA = da
                    Expect(doc.form.DA).toBe(da)
                }
                return resolve()
            })
        })
    }
}