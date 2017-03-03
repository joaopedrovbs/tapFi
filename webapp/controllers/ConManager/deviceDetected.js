const CONSTS = require('./CONSTS')
const ConManager = require('./index')

/*
    Device properties:
      id: <string> 
      name: <string>
      manufacturer: <string>
      tapFi: <TapFiDevice>

    Private properties:
      lastSeen: <number>

 */
module.exports = function deviceDetected(_props) {
  // Check if device is in list
  let device = ConManager.getDevice(_props.id) || {
    id: _props.id,
    name: null,
    info: null,
    status: 'Pairing...',
    tapFi: null,
  }

  // Update properties
  for (let k in _props) {
    if (!_props[k])
      continue

    device[k] = _props[k]
  }

  // Instantiate tapFi if needed
  if (!device.tapFi && device.peripheral) {
    device.tapFi = new app.controllers.TapFiDevice(device.peripheral)
  }

  // Update timestamp
  device.lastSeen = Date.now()

  // Add device
  ConManager.add(device)
}