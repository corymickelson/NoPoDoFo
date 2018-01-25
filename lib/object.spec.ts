import {Obj} from './object'
import {Document} from './document'
import * as test from 'tape'
import {join} from 'path';
import {Dictionary} from './dictionary';

test('document objects instance of nopodofo.Obj', t => {
    const filePath = join(__dirname, '../test-documents/test.pdf'),
        doc = new Document(filePath)
    doc.on('ready', e => {
        if (e instanceof Error) t.fail()
        const objs = doc.getObjects()
        if (objs[1].type === 'Dictionary') {
            let nObj:Object = objs[1].asObject(),
                ks = Object.keys(nObj)
            // t.assert((nObj as any)[ks[0]] instanceof Obj) 
            for (let key in nObj) {
                let x = (nObj as any)[key]
                t.assert((nObj as any)[key] instanceof Obj, "object values are instance of Object")
            }
        }
        let streams = objs.filter((i: any) => i.hasStream())
        t.assert(streams.length > 0, 'objects streams available')
        t.assert(streams[0].stream instanceof Buffer, 'stream as Buffer')
        t.assert(objs[0] instanceof Obj, 'Objects array, instance of nopodofo.Obj')

        let found = false;
        for (let i = 0; i < objs.length; i++) {
            if (objs[i].type === 'Dictionary') {
                let dictObj:Object = objs[i].asObject()
                let refs = Object.values(dictObj).filter(v => v.type === 'Reference')
                if (refs.length > 0) {
                    let ref = refs[0].asReference()
                    if (!ref.isIndirect()) continue
                    let refObj = ref.deref()
                    t.assert(refObj instanceof Obj, "Object ref types can dereference to object")
                    found = true
                    break
                }
            }
        }
        t.assert(found === true, "If streams are not found there is a problem")

        let dicts = objs.filter((i: any) => i.type === 'Dictionary'),
            dict = dicts[0].asObject()
        // t.assert(dict instanceof Dictionary, "Can get object as dictionary where type === Dictionary")

        let arrs = objs.filter((i: any) => i.type === 'Array'),
            arr = arrs[0].asArray()
        t.throws(() => arr.pop(), "This operation should fail since the underlying PdfArray is immutable")
        let arrObj = arr[0]
        t.assert(arrObj instanceof Obj, "Should return an Obj via array bracket getter notation")

        let keys = Object.keys(dict)
        t.assert(keys instanceof Array, 'keys instance of array')
        t.assert(keys.length > 0, 'keys not null or empty')
        t.assert(typeof keys[0] === 'string', 'keys contains string values')

        t.end()
    })
})



