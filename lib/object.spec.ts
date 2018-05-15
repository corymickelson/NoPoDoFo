import {Document, __mod} from './document'
import * as test from 'tape'
import {join} from 'path';
import {IObj} from "./object";

test('document objects instance of nopodofo.Obj', t => {
    const filePath = join(__dirname, '../test-documents/test.pdf'),
          doc      = new Document(filePath)
    doc.on('ready', e => {
        if (e instanceof Error) t.fail()
        let body = doc.body
        if (body[1].type === 'Dictionary') {
            let nObj = body[1].getDictionary(),
                ks   = nObj.getKeys()
            t.assert((nObj as any)[ks[0]] instanceof __mod.Obj)
            ks.forEach((key:string) => {
                let x = nObj.getKey(key)
                t.assert(x instanceof __mod.Obj)
            })
            for (let key in nObj) {
                let x = (nObj as any)[key]
                t.assert((nObj as any)[key] instanceof __mod.Obj, "object values are instance of Object")
            }
        }
        let streams = body.filter((i: any) => i.hasStream())
        t.assert(streams.length > 0, 'objects streams available')
        t.assert(streams[0].stream instanceof Buffer, 'stream as Buffer')

        let found = false;
        for (let i = 0; i < body.length; i++) {
            if (body[i].type === 'Dictionary') {
                let dictObj = body[i].getDictionary()
                dictObj.getKeys().forEach((key:string) => {
                    if(dictObj.getKey(key).type === 'Reference') {
                        let ref = dictObj.getKey(key).getReference()
                        if(ref.isIndirect()) {
                            console.log('reference')
                            t.assert(ref instanceof __mod.Ref)
                        }
                    }
                })
                let refs = dictObj.getKeys().filter((k:string) => {
                    return dictObj.getKey(k).type === 'Reference'
                }).map(k => {
                    return dictObj.getKey(k)
                })
                if (refs.length > 0) {
                    let ref = (refs[0] as IObj).getReference()
                    if (!ref.isIndirect()) continue
                    let refObj = doc.getObject(ref)
                    t.assert(refObj instanceof __mod.Obj, "Object ref types can dereference to object")
                    found = true
                    break
                }
            }
        }
        t.assert(found === true, "If streams are not found there is a problem")

        let arrs = body.filter((i: any) => i.type === 'Array'),
            arr  = arrs[0].getArray()
        t.throws(() => arr.pop(), "This operation should fail since the underlying PdfArray is immutable")
        let arrObj = arr.at(0)
        t.assert(arrObj instanceof __mod.Obj, "Should return an Obj via array bracket getter notation")

        t.end()
    })
})



