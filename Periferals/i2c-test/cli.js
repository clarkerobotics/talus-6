const i2c = require('i2c-bus')

/*
  ----- Closed Loop Control -----
  Quick Tests
  a  -  250 step distance
  s  -  50 step distance

  Commands
  t  -  step input: t0000
  e  -  enable toggle - boolean
  r  -  set point - r000 -> r270, use DEG
  d  -  dir: d0 -> 0 = negative | 1 = positive

  Settings
  i  -  set Min - i000 -> i30, use DEG
  o  -  set Max - o000 -> o270, use DEG
  n  -  set Step Number - n000 -> n3994, use full revolution step count
*/

// Examples:
// 0x10 1 t1200 0 0 -> address 10, dir 1, cmd t1200, extra 0, cb 0
// 0x20 0 e 0 1
// 0x30 0 p 0 1
// 0x40 1 s 1 0
const args = process.argv
const params = args.splice(2)
// console.log('process.argv', params)

console.log('Talus CLI Coms Starting')

// Initialize connection cpn!
const bus = i2c.open(1, function (er) {
  if (er) return;
  const msg = `000<${params[0]},${params[1]},${params[2]},${params[3]},cb+${params[4]}>000`

  bus.i2cWrite(parseInt(params[0], 16), msg.length, new Buffer(msg), () => {
    console.log('Action Finished', params[0], params[1], params[2])
  })
});
