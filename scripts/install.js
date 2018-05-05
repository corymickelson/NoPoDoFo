const {execSync: run} = require('child_process')
const {join} = require('path')
const {readdirSync} = require('fs')

function findModules(dir = '../', depth = 0) {
  if (depth >= 4) {
   throw Error('Depth error') 
  }
  let files = readdirSync(dir)
  if (files.indexOf('node_modules')) {
    return join(__dirname, dir)
  } else {
    return findModules(dir + '../', depth++)
  }
}
let cmd = 'cmake-js'
if (process.argv.length >= 3) {
  if (process.argv[2] === '-D') cmd += ' -D'
}

cmd += ` --CDPROJECT_DIR=${findModules()} --prefer-clang --s=c++14`
console.log('CMD: ', cmd)
run(cmd, {"stdio": "inherit"})