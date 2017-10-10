import { IObj, Obj } from './object'
import { Document } from './document'
import * as test from 'tape'
import { join } from 'path';
import { existsSync, unlinkSync } from 'fs'
import { Dictionary } from './dictionary';
import {Arr} from "./arr";

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
test('obj getOffset', t => {
    t.plan(2)
    const doc = new Document(filePath),
        obj = doc.getObjects()[0],
        dict = new Dictionary(obj),
        keys = dict.getKeys(),
        sync = obj.getOffsetSync(keys[0])
    obj.getOffset(keys[0], (e, v) => {
        t.assert(e === null)
        t.assert(v === sync)
        t.end()
    })
})

test('obj get as T', t => {
    const doc = new Document(filePath),
        obj = doc.getObjects().filter(i => i.type === 'Array')[0],
        sync = obj.as(obj.type)
    t.ok(sync instanceof Arr)
    t.end()
})