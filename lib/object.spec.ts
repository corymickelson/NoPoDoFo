import {Obj} from './object'
import {Document} from './document'
import * as test from 'tape'
import {join} from 'path';
import {existsSync, unlinkSync} from 'fs'
import {Dictionary} from './dictionary';
import {Arr} from "./arr";
import {Ref} from './reference';
// import { ok, equal, fail, ifError, throws } from 'assert'

// function objectInstance() {
// ok(objs[0] instanceof Obj, 'Objects array, instance of nopodofo.Obj')
test('document objects instance of nopodofo.Obj', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()

        t.assert(objs[0] instanceof Obj, 'Objects array, instance of nopodofo.Obj')
        t.end()
    })

})
// }

// function objToObject() {
test('npdf.Obj to Object', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()

        let nObj = objs[1].asDictionary(),
            jsObj = nObj.toObject()
        for (let key in jsObj) {
            t.assert(jsObj[key] instanceof Obj)
        }
        t.end()
    })

})
// }
// function objectType() {
// ok(failures.length === 0, 'all object types defined')
test('object type defined', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()

        let failures = objs.some((i:any) => !i.type)
        t.assert(!failures, 'all object types defined')
        t.end()
    })

})
// }

// function objectStream() {
test('object streams available', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()

        let streams = objs.filter((i:any) => i.hasStream())
        t.assert(streams.length > 0, 'objects streams available')
        t.assert(streams[0].stream instanceof Buffer, 'stream as Buffer')
        t.end()
    })

})
// }

// function objectRefDeref() {
test.only('object ref, deref Ref to object', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()

        let found = false;
        for (let i = 0; i < objs.length; i++) {
            if (objs[i].type === 'Dictionary') {
                let dictObj = objs[i].asDictionary().toObject()
                let refs = Object.values(dictObj).filter(v => v.type === 'Reference')
                if (refs.length > 0) {
                    let ref = refs[0].asReference()
                    if (!ref.isIndirect()) continue
                    let refObj = ref.deref()
                    t.assert(refObj instanceof Obj)
                    t.end()
                    found = true
                    break
                }
            }
        }
        if (!found) t.fail()
    })

})
// }

// function objectAsDictionary() {
test('object to dictionary', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()
        let dicts = objs.filter((i:any)=> i.type === 'Dictionary'),
            dict = dicts[0].asDictionary()
        t.assert(dict instanceof Dictionary)
        t.end()
    })
})
// }

// function objectAsArray() {
test('object to NoPoDoFo Arr', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()
        let arrs = objs.filter((i:any) => i.type === 'Array'),
            arr = arrs[0].asArray()
        t.assert(arr instanceof Arr)
        t.end()
    })

})
// }

// function objectAsName() { }

// function dictionaryKeys() {
test('dictionary keys', t => {
    const filePath = join(__dirname, '../test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', pdf => {
        const objs = pdf.getObjects()
        let dict = objs.filter((i:any) => i.type === 'Dictionary')[0].asDictionary(),
            keys = dict.getKeys()
        t.assert(keys instanceof Array, 'keys instance of array')
        t.assert(keys.length > 0, 'keys not null or empty')
        t.assert(typeof keys[0] === 'string', 'keys contains string values')
        t.end()
    })

})

// }

function runTest(test: Function) {
    setImmediate(() => {
        global.gc()
        test()
    })
}

// export function runAll() {
//     [
//         objectInstance,
//         objectType,
//         objectStream,
//         objectRefDeref,
//         objectAsDictionary,
//         objectAsArray,
//         objectAsName,
//         dictionaryKeys
//     ].map(i => runTest(i))
// }

// runAll()

