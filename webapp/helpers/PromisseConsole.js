const util = require('util')

function Postlog(console) {
  console.log = function log() {
    
  }
}

Postlog.lines = function lines() {
  // body...
}


const stdout = process.stdout
let longstr = ''
for (let i = 0; i < 100; i ++) {
  longstr += 'a'
}
console.log('hey!')
console.log(longstr)
console.log('you!')
console.log('there!')
setTimeout(() => {
  // Save
  stdout.write('\033[s')
  // Move up
  stdout.write('\033[3E')
  // Clear line
  stdout.write('\033[2K\033[1G')
  stdout.write('lol\n')
  // Restore
  stdout.write('\033[u')


  stdout.write('other thing\n')
  stdout.write('other thing\n')
}, 1000)
