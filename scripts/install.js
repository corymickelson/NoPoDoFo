const {execSync: run} = require('child_process')
const {platform } = require('os')
const {join} = require('path')
const {readdirSync} = require('fs')

/**
 * Look for node_modules directory
 * @param dir
 * @param depth
 * @returns {*}
 */
function findModules(dir = './', depth = 0) {
    if (depth >= 4) {
        throw Error('Depth error')
    }
    let files = readdirSync(dir)
    if (files.indexOf('node_modules') !== -1) {
        return join(__dirname, dir + '../')
    } else {
        return findModules(dir + '../', depth + 1)
    }
}

if(platform() === 'win32') {
    if(!process.env.VCPKG_PATH) {
        console.warn('Windows installation requires environment variable VCPKG_PATH to be set to the root of your' +
            'vcpkg installation')
        process.exit(1)
    }
}
let cmd = `${join(__dirname, '../node_modules/.bin/cmake-js')} build`
if (process.argv.length >= 3) {
    if (process.argv[2] === '-D') cmd += ' -D'
}

cmd += ` --CDPROJECT_DIR=${findModules()} -s=c++17`
console.log('CMD: ', cmd)
run(cmd, {"stdio": "inherit"})
