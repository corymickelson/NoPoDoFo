import {join} from 'path'
import {Suite, Options} from 'benchmark'
import {} from 'microtime'
import {npdf} from '../../dist'
import {readFileSync} from "fs";

const opts: Options = {
    defer: true,
    delay: 0.05,
    minSamples: 10,
    onError: console.error,
    onStart: (target: any) => console.log('Starting benchmark: ', target.target.name)
}
const testDoc = join(__dirname, '../../test-documents/test.pdf')
const testDocData = readFileSync(testDoc)
const reader = new Suite('reader')
/**
 * Load the document and iterate each object in the document body. This will force any delayed
 * Objects to be loaded.
 */
reader.add('Document load (from file) and iterate body', Object.assign({
    fn: (defer: PromiseConstructor) => {
        const doc = new npdf.Document()
        doc.load(testDoc, err => {
            if (err) defer.reject(err)
            else {
                let work: { [key: string]: number } = {}
                doc.body.forEach(o => {
                    if (!work.hasOwnProperty(o.type)) work[o.type] = 0
                    work[o.type]++
                })
                global.gc()
                defer.resolve()
            }
        })
    }
}, opts))
    .add('Document load (from buffer) and iterate body', Object.assign({
        fn: (defer: PromiseConstructor) => {
            const doc = new npdf.Document()
            doc.load(testDocData, {fromBuffer: true}, err => {
                if (err) defer.reject(err)
                else {
                    let work: { [key: string]: number } = {}
                    doc.body.forEach(o => {
                        if (!work.hasOwnProperty(o.type)) work[o.type] = 0
                        work[o.type]++
                    })
                    global.gc()
                    defer.resolve()
                }
            })
        }
    }, opts))
    .on('complete', () => {
        reader.forEach((t: any) => console.log(t.toString()))
    })
    .run({async: false})