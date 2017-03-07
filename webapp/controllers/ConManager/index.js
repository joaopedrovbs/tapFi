const EventEmmiter = require('events')

const CONSTS = require('./CONSTS')

/*
 * Manages connected peripherals, by removing them after timeout expires.
 * Also, has a "queue" system for issuing commands to devices, such as:
 *  + PAY
 *  + GET_INFO
 *
 * This is a singleton instance, that inherits from EventEmmiter. In order
 * to listen for changes in the current device list, and receive a formatted
 * list, use:
 *  ConManager.on('change', (data) => {...})
 */
var ConManager = module.exports = new EventEmmiter()

/*
 * Must be called on startup to initialize and start services
 */
ConManager.init = function init() {
  console.log(CONSTS.TAG, 'init')

  // Init device list
  ConManager.devices = []

  // Start maintaining
  setInterval(ConManager.maintainDevices, CONSTS.MANAGE_INTERVAL)

  // Start Queue
  ConManager.queue = async.queue(ConManager.handleQueue, 1)

  // Publish initial state
  ConManager.publishChanges()
}

/*
 * Publish change event in ConManager
 */
ConManager.publishChanges = () => {
  let devices = _.map(ConManager.devices, _.partialRight(_.pick, ['id', 'name', 'info', 'status']))
  // console.log(devices)
  ConManager.emit('change', devices)
}

/*
 * Get a device from the list by ID
 */
ConManager.getDevice = (id) => {
  return ConManager.devices.find(device => id === device.id)
}

/*
 * Removes a device from the devices list
 */
ConManager.remove = (device) => {
  let index = ConManager.devices.indexOf(device) 
  
  if (index < 0)
    return

  device.log(CONSTS.TAG, chalk.red.dim('removed'), chalk.dim(ConManager.nameTag(device)))
  ConManager.devices.splice(index, 1)

  // Publish changes
  ConManager.publishChanges()
}

/*
 * Adds a device to the list, or update it if already exists.
 * If device is new, adds a task in the queue 
 */
ConManager.add = (device, _forceUpdate) => {
  let index = ConManager.devices.indexOf(ConManager.getDevice(device.id))
  
  if (index < 0) {
    device.log(CONSTS.TAG, chalk.green.dim('added  '), ConManager.nameTag(device))
    ConManager.devices.push(device)

    // Add task to gatter info from device
    let task = CONSTS.TASK_GET_INFO
    ConManager.queue.push({task, device})

    // DEBUG
    // task = CONSTS.TASK_PAY
    // ConManager.queue.push({task, device})

    // Publish changes
    ConManager.publishChanges()

    return 
  }

  ConManager.devices[index] = device

  device.log(CONSTS.TAG, chalk.green('connect'), ConManager.nameTag(device))
  if (_forceUpdate) {

    // Publish changes
    ConManager.publishChanges()
  }
}