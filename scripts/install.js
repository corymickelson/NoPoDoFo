const {execSync: run} = require('child_process')
const {join} = require('path')
const cmd = `cmake-js compile --CDPROJECT_DIR=${join(__dirname, '../../../')} --prefer-clang --s=c++14`
console.log('CMD: ', cmd)
run(cmd, {"stdio": "inherit"})