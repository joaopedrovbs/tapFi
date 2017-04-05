const CONSTS = require('./CONSTS')

/*
 * Helper method to disconnect and callback if flag "disconnect" is set to true
 */
module.exports = function finishMaybeDisconnecting(shouldDisconnect, err, callback, ...params) {
  if (shouldDisconnect) {
    // Disconnect first
    try {
      this.device.disconnect((err) => {
        return callback(err, ...params)
      })
    } catch (e) {
      callback(err, ...params)
    }
  } else {
    // Skip disconnecting
    if (err)
        return callback(err)

    return callback(null, ...params)
  }
}