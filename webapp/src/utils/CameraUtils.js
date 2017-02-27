export default {
  getVideoDevices(next) {
    navigator.mediaDevices.enumerateDevices().then(function(devices) {
      devices = devices.filter(device => device.kind === 'videoinput')
      console.log(devices)
      next(devices)
    })
  },

  getBestDevice(devices, deviceName) {
    return devices.find(device => deviceName.test(device.label)) || devices[0]
  },

  connectToDevice(device, next) {
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
}