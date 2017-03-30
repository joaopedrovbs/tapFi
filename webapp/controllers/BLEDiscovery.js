'use strict'
const TAG = newTAG('BLEDiscovery')

const noble = require('noble');

const UUIDs = [
  'e0b76bd0fda411e6bc6492361f002671'
]

var BLEDiscovery = module.exports = {
  state: 'unknown',
  scanning: false,
}

BLEDiscovery.init = () => {
  console.log(TAG, 'init')

  noble.on('discover', BLEDiscovery.discoveryCallback);
  noble.on('stateChange', BLEDiscovery.updateAdaptorState);

  BLEDiscovery.updateAdaptorState()
}

BLEDiscovery.discoveryCallback = (peripheral) => {
  console.log(TAG, chalk.yellow(peripheral.advertisement.localName))

  let parsedPeripheral = BLEDiscovery.parsePeripheral(peripheral)
  app.controllers.ConManager.deviceDetected(parsedPeripheral)
}

BLEDiscovery.updateAdaptorState = (nextState) => {
  
  if (noble.state == 'poweredOn' && !BLEDiscovery.scanning) {
    console.log(TAG, chalk.green('start scanning'), nextState)
    noble.startScanning(UUIDs, true);
  } else if (noble.state == 'poweredOff') {
    console.log(TAG, chalk.red('stop scanning'))
    noble.stopScanning();
  }

  BLEDiscovery.scanning = noble.state == 'poweredOn'
}

BLEDiscovery.parsePeripheral = (peripheral) => {
  let adv = peripheral.advertisement
  let device = {
    id: peripheral.id,
    name: adv.localName,
    address: peripheral.address,
    peripheral: peripheral,
  }

  return device
}