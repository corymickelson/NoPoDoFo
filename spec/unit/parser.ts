import {join} from 'path';
import { nopodofo as npdf} from '../../';
import * as test from 'tape'
if(!global.gc) {
    global.gc = () => {}
}
test('get contents as string, contents tokenizer', t => {

    const filePath = join(__dirname, '../test-documents/test.pdf'),
        doc = new npdf.Document()

    doc.load(filePath, (e: Error) => {
        if (e) t.fail()
            let tokenizer = new npdf.ContentsTokenizer(doc, 0)

        const pageContents = tokenizer.readSync()
        t.assert(pageContents.next().value.startsWith('Form'))
        t.assert(pageContents.next().done === false)
        t.end()
    })
})
test('get contents async', t => {

    const filePath = join(__dirname, '../test-documents/test.pdf'),
        doc = new npdf.Document()

    doc.load(filePath, (e: Error) => {
        if (e) t.fail()
            let tokenizer = new npdf.ContentsTokenizer(doc, 0)

        const pageContents = tokenizer.read((err, data) => {
            if(err) t.fail('Contents Tokenizer read into buffer failed')
            else {
                t.assert(Buffer.isBuffer(data), "Returns a buffer")
                t.assert(data.length === 183776, "buffer is not empty")
                t.end()
            }
        })
    })
})
