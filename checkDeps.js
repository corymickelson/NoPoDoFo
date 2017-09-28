const exec = require('child_process').exec

exec('podofosign', (err, stdout, stderr) => {
    if(stdout === null) {
        console.warn("PoDoFo required before running install command")
        process.exit(1)
    }
})

let vStr = process.version.substr(1).split('.')
try {
let major = parseInt(vStr[0]),
  minor = parseInt(vStr[1])
  if(major < 8 || (major >= 8 && minor < 6)) {
    console.warn("Requires node v8.6+")
    process.exit(1)
  }
} catch(e) {
  console.error("failed to parse node version")
  process.exit(1)
}

  
