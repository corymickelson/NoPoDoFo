import {AsyncTest, Expect, TestFixture, Timeout} from 'alsatian'
import {join} from "path";
import {nopodofo, NPDFName} from '../../'
import {readFileSync, writeFileSync} from "fs";
import {v4} from 'uuid'
import Rect = nopodofo.Rect;
import StreamDocument = nopodofo.StreamDocument;
import Document = nopodofo.Document;
import Base = nopodofo.Base;
import Painter = nopodofo.Painter;
import Image = nopodofo.Image;
import Page = nopodofo.Page;
import Dictionary = nopodofo.Dictionary;

@TestFixture('Image')
export class ImageSpec {
    @AsyncTest('SetImageMemDocument')
    @Timeout(500000)
    public async setImageTestMemDocument() {
        let doc: Base
        await new Promise<Document>(resolve => {
            let document = new Document();
            (document as Document).load(join(__dirname, '../test-documents/test.pdf'), (err, data) => {
                if (err) Expect.fail(err.message)
                else {
                    const painter = new Painter(document)
                    const image = new Image(document, join(__dirname, '../test-documents/test.jpg'))
                    const page = document.getPage(0)
                    painter.setPage(page)
                    painter.drawImage(image, 0, page.height - image.height, {height: 0.5, width: 0.5})
                    painter.finishPage();
                    (document as Document).write((err1, data1) => {
                        if (err1) Expect.fail(err1.message)
                        else {
                            this.testDocument(data1)
                                .then(() => resolve())
                                .catch(e => Expect.fail(e))
                        }
                    })
                }
            })
        })
    }

    @AsyncTest('SetImageStreamDocument')
    public async setImageTestStreamDocument() {
        const doc = new StreamDocument()
        const page = doc.createPage(new Rect(0, 0, 612, 792))
        const painter = new Painter(doc)
        const imgSrc = readFileSync(join(__dirname, '../test-documents/test.jpg'))
        const image = new Image(doc, imgSrc)
        painter.setPage(page)
        painter.drawImage(image, 0, page.height - image.height)
        painter.finishPage();
        const data = doc.close()
        await this.testDocument(data)
    }

    public async testDocument(data: string | Buffer) {
        const doc = new Document();
        (doc as Document).load(data, (e, d) => {
            if (e) Expect.fail(e.message)
            else {
                this.getPageImg(doc.getPage(0))
                    .then(f => {
                        f!.forEach(i => {
                            if (!i) Expect.fail('Failed to read image')
                            let img = readFileSync(i as string)
                            Expect(img).toBeDefined()
                            Expect(img.length).toEqual(readFileSync(join(__dirname, '../test-documents/test.jpg')).length)
                        })
                    })
                    .catch(e => {
                        Expect.fail(e)
                    })
            }
        })
    }

    static extractImg(obj: nopodofo.Object, jpg: Boolean): Promise<string> {
        return new Promise((resolve, reject) => {
            let ext = jpg ? '.jpg' : '.ppm'
            let filename = `/tmp/${v4() + ext}`
            writeFileSync(filename, obj.stream)
            return resolve(filename)
        })
    }

    private async getPageImg(page: Page): Promise<string[] | null> {
        const resourcesKeys = page.resources.getDictionary().getKey<Dictionary>('XObject').getKeys()
        let ks = resourcesKeys.filter(i => i.startsWith('XOb'))
        if (ks!.length > 0) {
            const files: string[] = []
            await new Promise(resolve => {
                ks.forEach(async k => {
                    const xob = page.resources.getDictionary().getKey<Dictionary>('XObject').getKey<Dictionary>(k)
                    if (xob.getKey<String>(NPDFName.SUBTYPE) === 'Image') {
                        if (xob.hasKey('Filter') && xob.getKey<String>('Filter') === 'DCTDecode') {
                            let f = await ImageSpec.extractImg(xob.obj, true)
                            files.push(f)
                        } else {
                            let f = await ImageSpec.extractImg(page.resources.getDictionary().getKey<Dictionary>('XObject').getKey<nopodofo.Object>(k, false), true)
                            files.push(f)
                        }
                    }
                })
                return resolve()
            })
            return files
        } else return Promise.resolve(null)
    }
}
