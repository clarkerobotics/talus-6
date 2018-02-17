// var noble = require('noble');

var serviceUUIDs = ['F2:6B:F3:18:D1:1F']
var async = require('async');
var noble = require('noble');

var peripheralIdOrAddress = serviceUUIDs[0].toLowerCase();
// var peripheralIdOrAddress = 'f26bf318d11f'

noble.on('stateChange', function(state) {
  if (state === 'poweredOn') {
    console.log('state poweredOn');
    noble.startScanning();
  } else {
    noble.stopScanning();
  }
});

noble.on('discover', function(peripheral) {
  if (peripheral.id === peripheralIdOrAddress || peripheral.address === peripheralIdOrAddress) {
    noble.stopScanning();

    console.log('peripheral with ID ' + peripheral.id + ' found');
    var advertisement = peripheral.advertisement;

    var localName = advertisement.localName;
    var txPowerLevel = advertisement.txPowerLevel;
    var manufacturerData = advertisement.manufacturerData;
    var serviceData = advertisement.serviceData;
    var serviceUuids = advertisement.serviceUuids;

    if (localName) {
      console.log('  Local Name        = ' + localName);
    }

    if (txPowerLevel) {
      console.log('  TX Power Level    = ' + txPowerLevel);
    }

    if (manufacturerData) {
      console.log('  Manufacturer Data = ' + manufacturerData.toString('hex'));
    }

    if (serviceData) {
      console.log('  Service Data      = ' + JSON.stringify(serviceData, null, 2));
    }

    if (serviceUuids) {
      console.log('  Service UUIDs     = ' + serviceUuids);
    }

    console.log();

    explore(peripheral);
  }
});

function explore(peripheral) {
  console.log('services and characteristics:');

  peripheral.on('disconnect', function(e) {
    console.log(peripheralIdOrAddress, 'disconnected!');
    process.exit(0);
  });

  peripheral.connect(function(error) {
    console.log('peripheral.connect error', error, peripheral.state)

    //'aeae'
    //'dada'
    peripheral.discoverSomeServicesAndCharacteristics(['dada'], ['aeae'], function(err, services, characteristics) {
      console.log('error', err)
      var character = characteristics[0];
      console.log('service', services[0].uuid, 'character', character.uuid)
      // character.read(function(e, data) {
      //   // convert buffer to string
      //   var d = data.toString()
      //   console.log('character.read error, data', e, d)
      // });
      //
      character.subscribe(function(e) {
        console.log('character.subscribe error', e)

        character.once('notify', function(state) {
          console.log('state', state)
        })

        character.on('data', function(data, isNotification) {
          // convert buffer to string
          const d = data.toString()
          console.log('character.on data', isNotification, d)
        });
      });

      // setInterval(() => {
      //   character.read(function(e, data) {
      //     // convert buffer to string
      //     var d = data.toString()
      //     console.log('character.read error, data', e, d)
      //   });
      // }, 5000)

    });

    // peripheral.discoverServices([], function(error, services) {
    //   var serviceIndex = 0;
    //
    //   async.whilst(
    //     function () {
    //       return (serviceIndex < services.length);
    //     },
    //     function(callback) {
    //       var service = services[serviceIndex];
    //       var serviceInfo = service.uuid;
    //
    //       if (service.name) {
    //         serviceInfo += ' (' + service.name + ')';
    //       }
    //       console.log('serviceInfo', serviceInfo);
    //
    //       service.discoverCharacteristics(['181c'], function(error, characteristics) {
    //         var characteristicIndex = 0;
    //         console.log('discoverCharacteristics', error, characteristics)
    //
    //         async.whilst(
    //           function () {
    //             return (characteristicIndex < characteristics.length);
    //           },
    //           function(callback) {
    //             var characteristic = characteristics[characteristicIndex];
    //             var characteristicInfo = '  ' + characteristic.uuid;
    //
    //             if (characteristic.name) {
    //               characteristicInfo += ' (' + characteristic.name + ')';
    //             }
    //
    //             async.series([
    //               function(callback) {
    //                 characteristic.discoverDescriptors(function(error, descriptors) {
    //                   async.detect(
    //                     descriptors,
    //                     function(descriptor, callback) {
    //                       if (descriptor.uuid === '2901') {
    //                         return callback(descriptor);
    //                       } else {
    //                         return callback();
    //                       }
    //                     },
    //                     function(userDescriptionDescriptor){
    //                       if (userDescriptionDescriptor) {
    //                         userDescriptionDescriptor.readValue(function(error, data) {
    //                           if (data) {
    //                             characteristicInfo += ' (' + data.toString() + ')';
    //                           }
    //                           callback();
    //                         });
    //                       } else {
    //                         callback();
    //                       }
    //                     }
    //                   );
    //                 });
    //               },
    //               function(callback) {
    //                     characteristicInfo += '\n    properties  ' + characteristic.properties.join(', ');
    //
    //                 characteristic.on('data', function(data, isNotification) {
    //                   console.log('data', data, isNotification)
    //                   characteristic.read(function(error, data) {
    //                     if (data) {
    //                       var string = data.toString('ascii');
    //
    //                       characteristicInfo += '\n    value       ' + data.toString('hex') + ' | \'' + string + '\'';
    //                     }
    //                     callback();
    //                   });
    //                 });
    //
    //                 if (characteristic.properties.indexOf('read') !== -1) {
    //                   characteristic.read(function(error, data) {
    //                     if (data) {
    //                       var string = data.toString('ascii');
    //
    //                       characteristicInfo += '\n    value       ' + data.toString('hex') + ' | \'' + string + '\'';
    //                     }
    //                     callback();
    //                   });
    //                 } else {
    //                   callback();
    //                 }
    //               },
    //               function() {
    //                 console.log(characteristicInfo);
    //                 characteristicIndex++;
    //                 callback();
    //               }
    //             ]);
    //           },
    //           function(error) {
    //             serviceIndex++;
    //             callback();
    //           }
    //         );
    //       });
    //     },
    //     function (err) {
    //       // peripheral.disconnect();
    //     }
    //   );
    // });
  });
}

// function explore(peripheral) {
//   console.log('services and characteristics:');
//
//   peripheral.on('disconnect', function() {
//     process.exit(0);
//   });
//
//   peripheral.connect(function(error) {
//     peripheral.discoverServices([], function(error, services) {
//       var serviceIndex = 0;
//
//       async.whilst(
//         function () {
//           return (serviceIndex < services.length);
//         },
//         function(callback) {
//           var service = services[serviceIndex];
//           var serviceInfo = service.uuid;
//
//           if (service.name) {
//             serviceInfo += ' (' + service.name + ')';
//           }
//           console.log(serviceInfo);
//
//           service.discoverCharacteristics([], function(error, characteristics) {
//             var characteristicIndex = 0;
//
//             async.whilst(
//               function () {
//                 return (characteristicIndex < characteristics.length);
//               },
//               function(callback) {
//                 var characteristic = characteristics[characteristicIndex];
//                 var characteristicInfo = '  ' + characteristic.uuid;
//
//                 if (characteristic.name) {
//                   characteristicInfo += ' (' + characteristic.name + ')';
//                 }
//
//                 async.series([
//                   function(callback) {
//                     characteristic.discoverDescriptors(function(error, descriptors) {
//                       async.detect(
//                         descriptors,
//                         function(descriptor, callback) {
//                           if (descriptor.uuid === '2901') {
//                             return callback(descriptor);
//                           } else {
//                             return callback();
//                           }
//                         },
//                         function(userDescriptionDescriptor){
//                           if (userDescriptionDescriptor) {
//                             userDescriptionDescriptor.readValue(function(error, data) {
//                               if (data) {
//                                 characteristicInfo += ' (' + data.toString() + ')';
//                               }
//                               callback();
//                             });
//                           } else {
//                             callback();
//                           }
//                         }
//                       );
//                     });
//                   },
//                   function(callback) {
//                         characteristicInfo += '\n    properties  ' + characteristic.properties.join(', ');
//
//                     if (characteristic.properties.indexOf('read') !== -1) {
//                       characteristic.read(function(error, data) {
//                         if (data) {
//                           var string = data.toString('ascii');
//
//                           characteristicInfo += '\n    value       ' + data.toString('hex') + ' | \'' + string + '\'';
//                         }
//                         callback();
//                       });
//                     } else {
//                       callback();
//                     }
//                   },
//                   function() {
//                     console.log(characteristicInfo);
//                     characteristicIndex++;
//                     callback();
//                   }
//                 ]);
//               },
//               function(error) {
//                 serviceIndex++;
//                 callback();
//               }
//             );
//           });
//         },
//         function (err) {
//           peripheral.disconnect();
//         }
//       );
//     });
//   });
// }

/*
  Continously scans for peripherals and prints out message when they enter/exit

    In range criteria:      RSSI < threshold
    Out of range criteria:  lastSeen > grace period

  based on code provided by: Mattias Ask (http://www.dittlof.com)
*/
// var noble = require('../index');


// noble.on('stateChange', function(state) {
//   if (state === 'poweredOn') {
//     noble.startScanning();
//   } else {
//     noble.stopScanning();
//   }
// });
//
// noble.on('discover', function(peripheral) {
//   console.log('peripheral discovered (' + peripheral.id +
//               ' with address <' + peripheral.address +  ', ' + peripheral.addressType + '>,' +
//               ' connectable ' + peripheral.connectable + ',' +
//               ' RSSI ' + peripheral.rssi + ':');
//   console.log('\thello my local name is:');
//   console.log('\t\t' + peripheral.advertisement.localName);
//   console.log('\tcan I interest you in any of the following advertised services:');
//   console.log('\t\t' + JSON.stringify(peripheral.advertisement.serviceUuids));
//
//   var serviceData = peripheral.advertisement.serviceData;
//   if (serviceData && serviceData.length) {
//     console.log('\there is my service data:');
//     for (var i in serviceData) {
//       console.log('\t\t' + JSON.stringify(serviceData[i].uuid) + ': ' + JSON.stringify(serviceData[i].data.toString('hex')));
//     }
//   }
//   if (peripheral.advertisement.manufacturerData) {
//     console.log('\there is my manufacturer data:');
//     console.log('\t\t' + JSON.stringify(peripheral.advertisement.manufacturerData.toString('hex')));
//   }
//   if (peripheral.advertisement.txPowerLevel !== undefined) {
//     console.log('\tmy TX power level is:');
//     console.log('\t\t' + peripheral.advertisement.txPowerLevel);
//   }
//
//   console.log();
// });


// [CHG] Device F2:6B:F3:18:D1:1F Connected: yes
// [NEW] Primary Service
// 	/org/bluez/hci0/dev_F2_6B_F3_18_D1_1F/service000a
// 	00001801-0000-1000-8000-00805f9b34fb
// 	Generic Attribute Profile
// [NEW] Primary Service
// 	/org/bluez/hci0/dev_F2_6B_F3_18_D1_1F/service000b
// 	6e400001-b5a3-f393-e0a9-e50e24dcca9e
// 	Vendor specific
// [NEW] Characteristic
// 	/org/bluez/hci0/dev_F2_6B_F3_18_D1_1F/service000b/char000c
// 	6e400003-b5a3-f393-e0a9-e50e24dcca9e
// 	Vendor specific
// [NEW] Descriptor
// 	/org/bluez/hci0/dev_F2_6B_F3_18_D1_1F/service000b/char000c/desc000e
// 	00002902-0000-1000-8000-00805f9b34fb
// 	Client Characteristic Configuration
// [NEW] Characteristic
// 	/org/bluez/hci0/dev_F2_6B_F3_18_D1_1F/service000b/char000f
// 	6e400002-b5a3-f393-e0a9-e50e24dcca9e
// 	Vendor specific
