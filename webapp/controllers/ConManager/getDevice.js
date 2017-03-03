const CONSTS = require('./CONSTS')
const ConManager = require('./index')

/*
 * Get a device from the list by ID
 */
module.exports = function getDevice(id) {
  return ConManager.devices.find(device => id === device.id)
}