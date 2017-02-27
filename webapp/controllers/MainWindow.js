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

  console.log(TAG, chalk.cyan('Launching mainWindow'))
}

// Quit when all windows are closed.
electron.app.on('window-all-closed', function () {
  electron.app.quit()
});


exports.notify = function notify(title, message, stick) {
  if (!mainWindow) {
    return;
  }

  mainWindow.webContents.send('notify', title, message, stick);
}

exports.sendBarcode = function (barcode) {
  if (!mainWindow) {
    return;
  }

  mainWindow.webContents.send('barcode', barcode);
}