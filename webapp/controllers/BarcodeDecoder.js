'use strict'
const TAG = newTAG('BarcodeDecoder')

const ipcMain = electron.ipcMain
const BrowserWindow = electron.BrowserWindow

let mainWindow = null

exports.getWindow = function (){
  return mainWindow;
}

exports.launch = function (){
  console.log(TAG, chalk.cyan('Launching BarcodeDecoder'))

  // Skip re-opening window on re-launch
  if(mainWindow)
    return

  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    // icon: app.config.icon,
    show: false,
  })

  // and load the index.html of the app.
  let targetUrl = `file://${__dirname}/../barcode/index.html`
  

  // Open the DevTools.
  if(app.helpers.isDev) {
    // Set target to React
    // targetUrl = 'http://localhost:3000'
    mainWindow.webContents.openDevTools({detached: true})
  }

  // Starts listening and forwarding barcodes to the MainWindow
  exports.listenToBarcodes()

  // Load from specified url
  mainWindow.loadURL(targetUrl)

  // Starts listening to barcode events

  // Emitted when the window is closed.
  // mainWindow.on('closed', function () {
  //   mainWindow = null
  //   console.log(TAG, chalk.red('Closed mainWindow'))
  // })

  // console.log(TAG, chalk.cyan('Launching mainWindow'))
}

exports.listenToBarcodes = function () {
  ipcMain.on('barcode', (event, barcode) => {
    console.log(TAG, 'new barcode', barcode)
    // Pass to MainWindow
    app.controllers.MainWindow.sendBarcode(barcode)
  })
}