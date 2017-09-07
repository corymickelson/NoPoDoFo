const mod = require('bindings')('npdf');
const npdf = new mod;
npdf.load("/home/skyslope/tes.out.pdf")
console.log(npdf.getVersion())
