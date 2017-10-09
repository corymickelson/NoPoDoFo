import { IObj, Obj } from './object'
import { Document } from './document'
import * as test from 'tape'
import { join } from 'path';
import { Arr } from './arr';
import { existsSync, unlinkSync } from 'fs'

const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    outTxtFile = './test.out.txt'


test('obj write sync', t => {
    const doc = new Document(filePath),
        obj = doc.getObjects()[0]
    obj.writeSync(outTxtFile)
    t.ok(existsSync(outTxtFile))
    unlinkSync(outTxtFile)
    t.end()
})
test('obj write async', t => {
    const doc = new Document(filePath),
        obj = doc.getObjects()[0]
    obj.write(outTxtFile, (e, v) => {
        t.assert(e === null)
        t.assert(v === outTxtFile)
        t.ok(existsSync(outTxtFile))
        unlinkSync(outTxtFile)
        t.end()
    })
})
