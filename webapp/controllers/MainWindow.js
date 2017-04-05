'use strict'
const TAG = newTAG('MainWindow')

const BrowserWindow = electron.BrowserWindow

let mainWindow = null

exports.getWindow = function (){
  return mainWindow;
}

exports.launch = function (){
  // Skip re-opening window on re-launch
  if(mainWindow)
    return

  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    // icon: app.config.icon,
  })

  // and load the index.html of the app.
  let targetUrl = `file://${__dirname}/../build/index.html`
  

  // Open the DevTools.
  if(app.helpers.isDev) {
    // Set target to React
    targetUrl = 'http://localhost:3000'
    mainWindow.webContents.openDevTools({detached: true})
  }

  // Load from specified url
  mainWindow.loadURL(targetUrl)

  // Emitted when the window is closed.
  mainWindow.on('closed', function () {
    mainWindow = null
    console.log(TAG, chalk.red('Closed mainWindow'))
  })

  // Listen for changes on Devices in ConManager, and send to webContents
  app.controllers.ConManager.on('didPay', (success) => exports.send('didPay', success))
  app.controllers.ConManager.on('change', (devices) => exports.send('devices', devices))

  // Listen for incoming pay request events
  electron.ipcMain.on('pay', (event, deviceId, value) => {
    console.log(TAG, chalk.cyan('PAID'), chalk.blue(value), chalk.dim(deviceId))
    
    // Add to queue
    let ConManager = app.controllers.ConManager
    ConManager.queue.push({
      task: ConManager.CONSTS.TASK_PAY,
      device: ConManager.getDevice(deviceId),
      value: value,
    })
  })

  console.log(TAG, chalk.cyan('Launching mainWindow'))
}

// Quit when all windows are closed.
electron.app.on('window-all-closed', function () {
  electron.app.quit()
});


exports.send = function send(channel, ...params) {
  if (!mainWindow) {
    return;
  }

  mainWindow.webContents.send(channel, ...params);
}

exports.sendBarcode = function (barcode) {
  if (!mainWindow) {
    return;
  }

  mainWindow.webContents.send('barcode', barcode);
}