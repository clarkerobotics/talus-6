const i2c = require('i2c-bus')

console.log('i2c starting - Motor Tests')

const scenario = [{
  cmds: [{
    id: '0x10',
    dir: 0,
    cmd: 't1250',
    ext: null,
    cbId: 0
  }, {
    id: '0x20',
    dir: 1,
    cmd: 't250',
    ext: null,
    cbId: 0
  }],
  delay: 2500
}, {
  cmds: [{
    id: '0x10',
    dir: 1,
    cmd: 't450',
    ext: null,
    cbId: 0
  }, {
    id: '0x20',
    dir: 0,
    cmd: 't1250',
    ext: null,
    cbId: 0
  }],
  delay: 4500
}, {
  cmds: [{
    id: '0x10',
    dir: 0,
    cmd: 't800',
    ext: null,
    cbId: 0
  }, {
    id: '0x20',
    dir: 1,
    cmd: 't150',
    ext: null,
    cbId: 123
  }],
  delay: 750
}]

const runItem = (item) => {
  for (var d = 0; d < item.cmds.length; d++) {
    const s = item.cmds[d]
    const msg = `000<${s.id},${s.dir},${s.cmd},${s.ext || ''},cb+${s.cbId}>000`

    bus.i2cWrite(parseInt(s.id, 16), msg.length, new Buffer(msg), () => {
      console.log('Action Finished', s.id, s.dir, s.cmd)
    })
  }
}

const runScenario = () => {
  // MSG FORMAT: <ID,DIRECTION,MENU-CMD,EXTENDED-DATA,CB-ID>
  // NOTE: Added 000 padding for data assurance
  let item = scenario.shift()

  const done = () => {
    console.log('Scenario Finished')
  }

  if (!item) {
    done()
    return
  }

  runItem(item)
  if (item.delay) setTimeout(runScenario, item.delay)
  else done()
}

// Initialize connection cpn!
const bus = i2c.open(1, function (er) {
  if (er) return;
  runScenario()
});
