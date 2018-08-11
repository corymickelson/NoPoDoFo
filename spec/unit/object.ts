import {npdf} from '../../lib'
import * as test from 'tape'

if (!global.gc) {
    global.gc = () => {
    }
}
test('document objects instance of nopodofo.Obj', t => {
    // const doc = new npdf.StreamDocument()
    t.ok(new npdf.Date().toString().match(/^D:/))
    t.end()
})



