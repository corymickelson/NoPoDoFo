import {join} from 'path';
import { npdf, IContentsTokenizer} from '../../dist';
import * as test from 'tape'

test('get contents as string, contents tokenizer', t => {

    const filePath = join(__dirname, '../test-documents/test.pdf'),
        doc = new npdf.Document()

    doc.load(filePath, e => {
        if (e instanceof Error) t.fail()
            let tokenizer: IContentsTokenizer = new npdf.ContentsTokenizer(doc, 0)

        const pageContents = tokenizer.readAll()
        t.assert(pageContents.length === 155) // fix this
        t.end()
    })
})

