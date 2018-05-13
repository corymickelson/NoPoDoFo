import {Obj} from './object'
import {Document, __mod} from './document'
import * as test from 'tape'
import {join} from 'path';
import { IForm } from './form';

test('tmp', t => {
    const filePath = join(__dirname, '../test-documents/signed.pdf'),
          doc      = new Document(filePath)
    doc.on('ready', e => {
        (doc.form as IForm).SigFlags
        t.end()
    })
})
test.skip('document objects instance of nopodofo.Obj', t => {
    const filePath = join(__dirname, '../test-documents/test.pdf'),
          doc      = new Document(filePath)
    doc.on('ready', e => {
        if (e instanceof Error) t.fail()
        // let body = doc.body
        // if (body[1].type === 'Dictionary') {
            // let nObj = body[1].getDictionary(),
                // ks   = nObj.getKeys()
            // t.assert((nObj as any)[ks[0]] instanceof Obj) 
            // ks.forEach(key => {
                // let x = nObj.getKey(key)
                // t.assert(x instanceof __mod.Obj)
            // })
            // for (let key in nObj) {
            //     let x = (nObj as any)[key]
            //     t.assert((nObj as any)[key] instanceof Obj, "object values are instance of Object")
            // }
        // }
        // let streams = body.filter((i: any) => i.hasStream())
        // t.assert(streams.length > 0, 'objects streams available')
        // t.assert(streams[0].stream instanceof Buffer, 'stream as Buffer')
        // t.assert(objs[0] instanceof Obj, 'Objects array, instance of nopodofo.Obj')

        // let found = false;
        // for (let i = 0; i < body.length; i++) {
        //     if (body[i].type === 'Dictionary') {
        //         let dictObj = body[i].getDictionary()
        //         dictObj.getKeys().forEach(key => {
        //             if(dictObj.getKey(key).type === 'Reference') {
        //                 let ref = dictObj.getKey(key).getReference()
        //                 if(ref.isIndirect()) {
        //                     console.log('reference')
        //                     t.assert(ref instanceof __mod.Ref)
        //                 }
        //             }
        //         })
                // let refs = Object.values(dictObj).filter(v => v.type === 'Reference')
                // if (refs.length > 0) {
                //     let ref = refs[0].asReference()
                //     if (!ref.isIndirect()) continue
                //     let refObj = ref.deRef(doc)
                //     t.assert(refObj instanceof Obj, "Object ref types can dereference to object")
                //     found = true
                //     break
                // }
            // }
        // }
        // t.assert(found === true, "If streams are not found there is a problem")

        // let dicts = objs.filter((i: any) => i.type === 'Dictionary'),
            // dict = dicts[0].asObject()
        // t.assert(dict instanceof Dictionary, "Can get object as dictionary where type === Dictionary")

        // let arrs = body.filter((i: any) => i.type === 'Array'),
        //     arr  = arrs[0].getArray()
        // t.throws(() => arr.pop(), "This operation should fail since the underlying PdfArray is immutable")
        // let arrObj = arr.at(0)
        // t.assert(arrObj instanceof Obj, "Should return an Obj via array bracket getter notation")

        // let keys = Object.keys(dict)
        // t.assert(keys instanceof Array, 'keys instance of array')
        // t.assert(keys.length > 0, 'keys not null or empty')
        // t.assert(typeof keys[0] === 'string', 'keys contains string values')

        t.end()
    })
})



