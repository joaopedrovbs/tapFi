const async = require('async')
const ipcRenderer = require('electron').ipcRenderer
// const Quagga = require('quagga').default

// 
// Default configs
// 
const CAMERA_LABEL = /Logitech Webcam C930e/g
const RE_CATCH_WINDOW          = 2000
const MIN_CONSECUTIVE_COUNT    = 3

// 
// Public API
// 
var app = {}

app.consecutiveCount = 0
app.lastBarcode = null
app.lastValidBarcode = null
app.lastBarcodeTimestamp = 0
app.lastValidBarcodeTimestamp = 0
app.didReadBarcode = (barcode) => {
  console.log(barcode)
  let now = Date.now()
  let diff = now - app.lastBarcodeTimestamp
  app.lastBarcodeTimestamp = now

  // Filter wrong barcodes readings
  let insideCatchWindow = now - app.lastValidBarcodeTimestamp < RE_CATCH_WINDOW
  if (app.lastBarcode != barcode && app.lastValidBarcode == barcode && insideCatchWindow) {
    app.lastBarcode = barcode
    app.consecutiveCount = MIN_CONSECUTIVE_COUNT + 1
  }

  // Increase Counter
  if (app.lastBarcode == barcode)
    app.consecutiveCount++
  else
    app.consecutiveCount = 0

  let reachedCounter = app.consecutiveCount == MIN_CONSECUTIVE_COUNT

  // console.log(barcode, app.consecutiveCount)
  if (reachedCounter) {
    app.lastValidBarcode = barcode
    app.lastValidBarcodeTimestamp = now
    ipcRenderer.send('barcode', barcode)
  }

  app.lastBarcode = barcode
}

// 
// Private API
// 
app.getVideoDevices = (next) => {
  navigator.mediaDevices.enumerateDevices().then(function(devices) {
    devices = devices.filter(device => device.kind == 'videoinput')
    next(devices)
  })
}

app.connectToDevice = (device, next) => {
  const mediaConstraints = {
    audio: false,
    video: {
      optional: [{sourceId: device.deviceId}]
    }
  }

  navigator.mediaDevices.getUserMedia(mediaConstraints).then((stream) => {
    console.log(stream)
    next(null, stream)
  }).catch(next);
}

app.configureQuagga = (device) => {
   app.Scanner = Quagga
    .decoder({readers: ['ean_reader']})
    .locator({patchSize: 'medium'})
    .fromSource({
        target: 'body',
        constraints: {
            width: 600,
            height: 600,
            facingMode: "environment",
            deviceId: device.deviceId,
        }
    });
}

app.startQuagga = (device) => {
  app.Scanner.addEventListener('detected', (result) => {
    // console.warn('Detected', result.codeResult)
    app.didReadBarcode(result.codeResult.code)
  }).start();
}

app.bindEvents = (next) => {
  Quagga.onDetected( function () {
    console.log('callback', ...arguments)
  })
  Quagga.start()
  next()
}

// 
// Bootstrap
// 
$(document).ready(function start() {
  let device
  let devices

  const setupSteps = [
    // List all cameras
    (next) => {
      console.log('> List all cameras')
      app.getVideoDevices(discoveredDevices => {
        devices = discoveredDevices
        next()
      })
    },

    // Select best camera
    (next) => {
      // Log cameras
      console.log('> Select best camera', devices.map(device => device.label).join(', '))

      device = devices.find(device => CAMERA_LABEL.test(device.label))

      device = device || devices[0]

      // Stop if no device is found
      next(device ? null : 'No device found')
    },

    // Configure Quagga
    (next) => {
      console.log('> Init Quagga')
      app.configureQuagga(device)
      next()
    },

    // Start Quagga
    (next) => {
      console.log('> Bind Quagga events')
      app.startQuagga()
      next()
    },
  ]

  async.series(setupSteps, (err) => {
    console.log('Finished setup', err, device)
  })

})
// Discover camera to be used
// navigator.mediaDevices.getUserMedia(mediaConstraints).then(function(stream) {
//   console.log(stream)
// }).catch(function(err) {
//   /* handle the error */
// });


// Quagga.init({
//   inputStream : {
//     name : "Live",
//     type : "LiveStream",
//     target: document.querySelector('#yourElement')    // Or '#yourElement' (optional)
//   },
//   decoder : {
//     readers : ["code_128_reader"]
//   }
// }, function(err) {
//     if (err) {
//         console.log(err);
//         return
//     }
//     console.log("Initialization finished. Ready to start");
//     Quagga.start();
// });

// navigator.webkitGetUserMedia({video: true},
//   function(stream) {
//     document.getElementById('camera').src = URL.createObjectURL(stream);
//   },
//   function() {
//     alert('could not connect stream');
//   }
// );