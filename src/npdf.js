exports.main = function main() {
const npdf = require('bindings')('npdf')
const doc = new npdf.Document()
doc.load('/home/red/fw9.pdf')
let page = doc.getPage(0)
return {doc, page}
}
