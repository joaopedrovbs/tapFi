/*
 * Helper method to disconnect and callback if flag "disconnect" is set to true
 */
module.exports = function finishMaybeDisconnecting(shouldDisconnect, err, callback, ...params) {
  if (shouldDisconnect) {
    // Disconnect first
    console.log(this.TAG, 'disconnecting')
    this.device.disconnect((next) => {
      console.log(this.TAG, 'disconnected')
      if (err)
        return callback(err)

      return callback(null, ...params)
    })
  } else {
    // Skip disconnecting
    if (err)
        return callback(err)

    return callback(null, ...params)
  }
}