'use strict'
const TAG = newTAG('ConManager')

const async = require('async')

const DISCONNECT_TIMEOUT = 15000
const MANAGE_INTERVAL    = 1000

const TASK_PAY           = 'PAY'
const TASK_GET_INFO      = 'GET_INFO'

var ConManager = module.exports = {
  devices: [],
}

/*
 * Must be called on startup to initialize and start services
 */
ConManager.init = () => {
  console.log(TAG, 'init')

  // Start maintaining
  setInterval(ConManager.maintainDevices, MANAGE_INTERVAL)

  // Start Queue
  ConManager.queue = async.queue(ConManager.handleQueue, 1)
}

/*
 * Handle the queue 
 * Queue is a must in order to limit conccurrent connections on BLE
 */
ConManager.handleQueue = (task, finish) => {
  let { device } = task

  let tTAG = newTAG('task:'+task.task)
  console.log(tTAG, chalk.cyan('QUEUE'), chalk.dim('run'), chalk.yellow(task.task))

  if (task.task == TASK_GET_INFO) {
    // Gatter device info
    
    device.tapFi.getInfo((err, info) => {
      if (err) {
        console.log(tTAG, chalk.red('Queue error'), err)
        return finish(err)
      }
      
      device.info = info
      ConManager.add(device, true)
      finish()
    }) 
  } else if (task.task == TASK_PAY) {
    // Execute payment
    device.tapFi.makePayment(3.14, 'micmic@best-ilp.herokuapp.com', (err) => {
      if (err) {
        console.log(tTAG, chalk.red('Queue error'), err)
        return finish(err)
      }

      console.log(tTAG, chalk.green('Payment succeeded!'))
      finish()
    }, (status) => {
      console.log(tTAG, chalk.green('Statys update:'), chalk.blue(status))
    })
  }
}

/*
    Device properties:
      id: <string> 
      name: <string>
      manufacturer: <string>
      tapFi: <TapFiDevice>

    Private properties:
      lastSeen: <number>

 */
ConManager.deviceDetected = (_props) => {
  // Check if device is in list
  let device = ConManager.getDevice(_props.id) || {
    id: _props.id,
    name: null,
    acc: null,
    avatar: null,
    manufacturer: null,
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

/*
 * Get a device from the list by ID
 */
ConManager.getDevice = (id) => {
  return ConManager.devices.find(device => id === device.id)
}

/*
 * Adds a device to the list, or update it if already exists.
 * If device is new, adds a task in the queue 
 */
ConManager.add = (device, _logUpdate) => {
  let index = ConManager.devices.indexOf(ConManager.getDevice(device.id))
  
  
  if (index < 0) {
    console.log(TAG, chalk.green.dim('added  '), ConManager.nameTag(device))
    ConManager.devices.push(device)

    // Add task to gatter info from device
    let task = TASK_GET_INFO
    ConManager.queue.push({task, device})

    // DEBUG
    // task = TASK_PAY
    // ConManager.queue.push({task, device})

    return 
  }

  if (_logUpdate)
    console.log(TAG, chalk.cyan.dim('updated'), ConManager.nameTag(device))

  ConManager.devices[index] = device
}

/*
 * Removes a device from the devices list
 */
ConManager.remove = (device) => {
  let index = ConManager.devices.indexOf(device) 
  
  if (index < 0)
    return

  console.log(TAG, chalk.red.dim('removed'), ConManager.nameTag(device))
  ConManager.devices.splice(index, 1)
}

/*
 * Called once in a while, to remove old devices and maitain the list updated
 */
ConManager.maintainDevices = () => {
  let now = Date.now()

  for (let k in ConManager.devices) {
    let device = ConManager.devices[k]

    // Connected devices wont update lastSeen, so we must avoid it by updating lastSeen
    let isDeviceConnected = device.tapFi.state == 'connected'
    if(isDeviceConnected) {
      devices.lastSeen = now
    }


    if (now - device.lastSeen > DISCONNECT_TIMEOUT) {
      // Disconnect device after timeout
      ConManager.remove(device)
    }
  }
}

/*
 * Utility helper to log device info
 */
ConManager.nameTag = (device) => {
  let id = device.id.substring(0, 8)
  let name = device.name

  // if (device.info && device.info.avatar) {
  // }

  return chalk.dim(`<${id} `) + ` ${chalk.blue(name)}` + chalk.yellow(` ${device.info ? device.info.acc : '?'}`) + chalk.dim(`>`)
}