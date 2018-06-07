import { existsSync, unlinkSync, writeFile } from 'fs'
import { join } from 'path'
import * as test from 'tape'
import { IObj, IPage, npdf } from "../../dist";
if (!global.gc) {
    global.gc = () => { }
}
const filePath = join(__dirname, '../test-documents/test.pdf'),
    outFile = './test.out.pdf',
    doc = new npdf.Document()

let page: IPage;

doc.load(filePath, e => {
    page = doc.getPage(0)
    runAll()
})

function pageRotation() {

    test('set page rotation', t => {
        const originalRotation = page.rotation,
            newRotation = originalRotation + 90
        page.rotation = newRotation
        doc.write(outFile, e => {
            const testDoc = new npdf.Document()
            testDoc.load(outFile, e => {
                if (e instanceof Error) t.fail()
                const testPage = testDoc.getPage(0)
                t.assert(testPage.rotation === newRotation, 'Page rotation updated')
                unlinkSync(outFile)
                t.end()
            })
        })
    })
}

function pageProperties() {
    test('page instance properties', t => {
        t.assert(page.number === 1, 'page number valid')
        t.assert(page.width > 0, 'page width valid')
        t.assert(page.height > 0, 'page height valid')
        t.end()
    })
}

function pageTrimBox() {
    test('get trimbox', t => {
        const trimBox = page.trimBox
        t.assert(trimBox.height > 0, 'trimbox height is not null')
        t.assert(trimBox.width > 0, 'trimbox width not null')
        t.assert(trimBox.left === 0, 'trimbox left')
        t.assert(trimBox.bottom === 0, 'trimbox bottom')
        t.end()
    })
}

function pageGetAnnotsCount() {
    test('page number of annotations', t => {
        t.assert(page.annotationsCount() === 22, 'get\'s all annotations on the page')
        t.end()
    })
}

function pageGetAnnot() {
    test('page get annotation', t => {
        const annot = page.getAnnotation(0)
        t.ok(annot)
        t.assert(annot.getType() === 'Widget')
        t.end()
    })
}

function pageContents() {
    test('page contents', t => {
        const contents = page.contents
        t.assert(contents instanceof (npdf.Obj as any), 'is an instance of Obj')
        t.assert(contents.length > 0, 'content is not null or empty')
        t.end()
    })
}

function pageResources() {
    test('page resources', t => {
        const resources = page.resources
        t.assert(resources instanceof (npdf.Obj as any), 'is instance ob Obj')
        t.assert(resources.length > 0, 'is not null or empty')
        t.end()
    })
}

function pageAddImg() {
    test('add image', t => {
        const painter = new npdf.Painter(doc),
            img = new npdf.Image(doc, join(__dirname, '../test-documents/test.jpg'))

        painter.setPage(page)
        painter.drawImage(img, 0, page.height - img.height)
        painter.finishPage()
        function extractImg(obj: IObj, jpg: Boolean) {
            let ext = jpg ? '.jpg' : '.ppm'
            writeFile(`/tmp/test.img.extract.${ext}`, obj.stream, err => {
                if (err instanceof Error)
                    t.fail()
                t.assert(existsSync(`/tmp/test.img.extract.${ext}`) === true)
                if (existsSync('./img.out.pdf')) unlinkSync('./img.out.pdf')
                t.end()
            })
        }
        doc.write((e, d) => {
            let sub = new npdf.Document()
            sub.load(d, { fromBuffer: true }, e => {

                if (e instanceof Error) t.fail()
                sub.body.forEach(o => {
                    if (o.type === 'Dictionary') {
                        let objDict = o.getDictionary(),
                            objType = objDict.hasKey('Type') ? objDict.getKey('Type') : null,
                            objSubType = objDict.hasKey('SubType') ? objDict.getKey('SubType') : null

                        if ((objType && objType.type === 'Name') ||
                            (objSubType && objSubType.type === 'Name')) {

                            if ((objType && objType.getName() === 'XObject') || (objSubType && objSubType.getName() === 'Image')) {
                                let imgObj = objDict.hasKey('Filter') ? objDict.getKey('Filter') : null
                                if (imgObj && imgObj.type === 'Array') {
                                    const imgObjArr = imgObj.getArray()
                                    if (imgObjArr.length === 1) {
                                        if (imgObjArr.at(0).type === 'Name') {
                                            if (imgObjArr.at(0).getName() === 'DCTDecode') {
                                                extractImg(o, true)
                                                return
                                            }
                                        }
                                    }
                                }
                                else if (imgObj && imgObj.type === 'Name' && imgObj.getName() === 'DCTDecode') {
                                    extractImg(o, true)
                                    return
                                }
                            }
                        }
                    }
                })
            })
        })

    })
}

function runTest(test: Function) {
    setImmediate(() => {
        global.gc()
        test()
    })
}

export function runAll() {
    [
        pageRotation,
        pageProperties,
        pageTrimBox,
        pageGetAnnotsCount,
        pageGetAnnot,
        pageContents,
        pageResources //,
        // pageAddImg
    ].map(i => runTest(i))
}

