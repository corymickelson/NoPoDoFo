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
        if (e instanceof Error) t.fail()
            let tokenizer = new npdf.ContentsTokenizer(doc, 0)

        const pageContents = tokenizer.reader()
        t.assert(pageContents.next().value.startsWith('Form'))
        t.assert(pageContents.next().done === false)
        t.end()
    })
})

