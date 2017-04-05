module.exports = function readLongCharacteristic(charac, next) {

  /*
   * Flow: 
   *  1. Subscribe to Notification on Characteristic
   *  2. Each time a Notification arrives, concat buffer
   *  3. Once an empty buffer (length == 0) is got, finish and callback
   */
  
  let cb = null
  let err = null
  let chunks = []
  let longValue = null

  charac.subscribe((err) => {
    if (err) {
      err = err
      charac.unsubscribe()
      return next(err)
    }
    
    next()
  })

  // Listen to new data and pushes to array. On "empty", concat buffer and callback
  charac.on('data', function (data){
    console.log(data)
    if (data.length == 0){
      charac.unsubscribe()

      // Concat buffer
      longValue = Buffer.concat(chunks)

      // Callback if already set
      cb && cb(null, longValue)
    }

    chunks.push(data)
  })

  // Returns a function that allows registering for callback later to fetch the characteristic
  return function (_cb) {
    // Check if error ocurred
    if (err) {
      return _cb(err)
    }

    // Check if longValue is already available
    if (longValue) {
      return _cb(null, longValue)
    }

    // Save callback to be called later
    cb = _cb;
  }
}