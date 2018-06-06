const {execSync} = require('child_process')

execSync('git submodule update --init --recursive')
execSync('node-pre-gyp install --fallback-to-build')