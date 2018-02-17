var i2c = require('i2c-bus'),
  bus;
  console.log('i2c starting')


var ADDRESS = 0x06,
  ACTION = 0x01;
//
// var TSL2561_ADDR = 0x39,
//   TSL2561_CMD = 0x80,
//   TSL2561_REG_ID = 0x0a;

var onOff = 0;

//you might conclude the address is 160 when writing and 161 when reading
bus = i2c.open(1, function (er) {

  // setInterval(() => {
  //   // bus.scan((err, devs) => {
  //   //   console.log('Address\'s:', devs)
  //   // })
  //   bus.readWord(ADDRESS, onOff, function (err, stuff) {
  //     console.log('onOff', onOff);
  //     onOff = (onOff === 0) ? 1 : 0;
  //   })
  //   bus.sendByte(ADDRESS, onOff, function (err, stuff) {
  //     console.log('stuff', stuff);
  //     onOff = (onOff === 0) ? 1 : 0;
  //   })
  //   bus.writeWord(ADDRESS, ACTION, 0x01, function (err) {
  //     console.log('writeWord', err);
  //   })
  // }, 2000)
  // setInterval(() => {
  //   bus.readWord(0x10, onOff, function (err, stuff) {
  //     console.log('onOff', onOff);
  //     onOff = (onOff === 0) ? 1 : 0;
  //   })
  // }, 1500)

  // // Clears current buffer data
  // const start = '-'.padEnd(60)
  // bus.i2cWrite(0x10, 60, new Buffer(start), () => {
  //   console.log('Master Write Start')
  // })

  let h = 0
  setInterval(() => {
    // onOff = (onOff === 0) ? 1 : 0;
    // console.log('onOff', onOff);
    // bus.readWord(0x10, onOff, () => {})
    // bus.readWord(0x20, onOff, () => {})
    // bus.readWord(0x30, onOff, () => {})
    // bus.readWord(0x40, onOff, () => {})
    // bus.readWord(0x50, onOff, () => {})
    // bus.readWord(0x60, onOff, () => {})

    // bus.readWord(0x10, onOff, () => {})
    // bus.writeQuick(0x10, 1, () => {
    //   console.log('HERE writeQuick')
    // })

    // const msg = `000<0x10,t000,230,500,cb+${h},${+new Date()}>000`
    // // const msg = `cb+${h}`
    // bus.i2cWrite(0x10, msg.length, new Buffer(msg), () => {
    //   console.log('Master Write Finished', msg)
    // })

    for (var i = 1; i < 7; i++) {
      const msg = `000<0x${i}0,t000,230,${i}00,cb+${h},${+new Date()}>000`
      // const msg = `cb+${h}`
      bus.i2cWrite(parseInt(`0x${i}0`, 16), msg.length, new Buffer(msg), () => {
        // console.log('Master Write Finished', msg)
      })
    }
    console.log('Master Write Finished x6')
    h++
  }, 2000)

  // (function readTempHigh() {
  //   i2c1.readWord(DS1621_ADDR, DS1621_CMD_ACCESS_TH, function (err, stuff) {
  //   console.log('stuff', stuff);
  //     if (err) throw err;
  //     // readTempHigh();
  //   });
  // }());
});
