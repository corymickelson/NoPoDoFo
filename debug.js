const { execSync } = require('child_process')
const { readdirSync } = require('fs')

execSync('cmake-js build -D')

readdirSync('.')
  .filter(i => i.match(/v[1-9]$/))
  .map(i => i.substr(i.length - 1))
  .forEach(i => {
    console.log(`Copying addon to lib/binding/${i}/`)
    execSync(`cp build/Debug/nopodofo.node lib/binding/${i}/`)
  })

