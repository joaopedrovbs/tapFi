const CONSTS = require('./CONSTS')
const ConManager = require('./index')

/*
 * Called once in a while, to remove old devices and maitain the list updated
 */
module.exports = function maintainDevices() {
  let now = Date.now()

  for (let k in ConManager.devices) {
    let device = ConManager.devices[k]

    // Connected devices wont update lastSeen, so we must avoid it by updating lastSeen
    let isDeviceConnected = device.tapFi.state == 'connected'
    if(isDeviceConnected) {
      devices.lastSeen = now
    }


    if (now - device.lastSeen > CONSTS.DISCONNECT_TIMEOUT) {
      // Disconnect device after timeout
      ConManager.remove(device)
    }
  }
}