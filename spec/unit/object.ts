import {IDocument, npdf} from '../../dist'
import * as test from 'tape'
import {join} from 'path';

test('document objects instance of nopodofo.Obj', t => {
    const filePath = join(__dirname, '../test-documents/test.pdf'),
          doc      = new npdf.Document()
    doc.load(filePath, e => {
        if (e instanceof Error) t.fail()

        t.end()
    })
})



