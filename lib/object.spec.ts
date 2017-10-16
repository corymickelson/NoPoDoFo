import {IObj, Obj} from './object'
import {Document} from './document'
import * as test from 'tape'
import {join} from 'path';
import {existsSync, unlinkSync} from 'fs'
import {Dictionary} from './dictionary';
import {Arr} from "./arr";

const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    outTxtFile = './test.out.txt'

const doc = new Document(filePath),
    obj = doc.getObjects()[0]

test('obj write sync', t => {
    obj.writeSync(outTxtFile)
    t.ok(existsSync(outTxtFile), 'write to destination successful')
    unlinkSync(outTxtFile)
    t.end()
})
test('obj write async', t => {
    try {
        t.plan(3)
        obj.write(outTxtFile, (e, v) => {
            t.assert(e === null, 'error is null')
            t.assert(v === outTxtFile, 'returns destination file path')
            t.ok(existsSync(outTxtFile), 'write to destination successful')
            unlinkSync(outTxtFile)
            t.end()
        })
    }
    catch (e) {
        console.log(e)
        t.fail()
    }

})
test('obj getOffset', t => {
    t.plan(2)
    const dict = new Dictionary(obj),
        keys = dict.getKeys(),
        sync = obj.getOffsetSync(keys[0])
    obj.getOffset(keys[0], (e, v) => {
        t.assert(e === null, 'error is null')
        t.assert(v === sync, 'sync and async return same value')
        t.end()
    })
})

