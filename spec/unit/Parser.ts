import {AsyncTest, Expect, TestFixture} from 'alsatian'
import {nopodofo as npdf} from '../../'
import {join} from "path";

@TestFixture('Contents Parser')
export class ParserSpec {
    @AsyncTest('Sync test')
    public async getContentsSync() {
        return new Promise((resolve, reject) => {
            const filePath = join(__dirname, '../test-documents/test.pdf'),
                doc = new npdf.Document()

            doc.load(filePath, (e: Error) => {
                if (e) Expect.fail(e.message)
                let tokenizer = new npdf.ContentsTokenizer(doc, 0)

                const pageContents = tokenizer.readSync()
                Expect(pageContents.next().value.startsWith('Form')).toBeTruthy()
                Expect(pageContents.next().done).toBe(false)
                return resolve()
            })
        })

    }

    @AsyncTest('Get Contents Async')
    public async asyncTest() {
        return new Promise(resolve => {

            const filePath = join(__dirname, '../test-documents/test.pdf'),
                doc = new npdf.Document()

            doc.load(filePath, (e: Error) => {
                if (e) Expect.fail(e.message)
                let tokenizer = new npdf.ContentsTokenizer(doc, 0)

                tokenizer.read((err, data) => {
                    if (err) Expect.fail('Contents Tokenizer read into buffer failed')
                    else {
                        Expect(data.length).toBe(5709)
                        return resolve()
                    }
                })
            })

        })
    }
}
