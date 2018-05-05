const {execSync: run} = require('child_process')
const {join} = require('path')
const {readdirSync} = require('fs')

function findModules(dir = './', depth = 0) {
  console.log('dir: ', dir)
  console.log('depth: ', depth)
  if (depth >= 4) {
   throw Error('Depth error') 
  }
  let files = readdirSync(dir)
  console.log(JSON.stringify(files))
  if (files.indexOf('node_modules') !== -1) {
    return join(__dirname, dir + '../')
  } else {
    return findModules(dir + '../', depth + 1)
  }
}
let cmd = 'cmake-js'
if (process.argv.length >= 3) {
  if (process.argv[2] === '-D') cmd += ' -D'
}

cmd += ` --CDPROJECT_DIR=${findModules()} --prefer-clang --s=c++14`
console.log('CMD: ', cmd)
run(cmd, {"stdio": "inherit"})