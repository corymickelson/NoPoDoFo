const exec = require('child_process').exec

exec('podofosign', (err, stdout, stderr) => {
    if(stdout == null) {
        console.warn("PoDoFo required before running install command")
        process.exit(1)
    }
})