const {execSync} = require('child_process')

execSync('cd .. && git submodule update --init --recursive')
execSync('node-pre-gyp install --fallback-to-build')