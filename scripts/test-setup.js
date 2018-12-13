const {mkdirSync,accessSync} = require('fs')
const {join} = require('path')
const tmp = join(__dirname, '../spec/tmp')
try {
 accessSync(tmp)
} catch(e) {
  mkdirSync(tmp)
}
