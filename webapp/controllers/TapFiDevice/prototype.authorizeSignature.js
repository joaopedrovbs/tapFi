const CONSTS = require('./CONSTS')

/*
 * Requests an authentication key for the specified conditional payment.
 * Will return with the authentication key, or "null" if it didn't authorized
 */
module.exports = function authorizeSignature(value, destination, next) {
  let characAuth, characValue, characSignature, characPublickey, authorization
  let packet, condition, fullPacket
  let onSignature, onPublickey
  let signature, publickey

  // Generate IPR for it
  let [iprHash, iprChop] = this.makeIPR(value, destination)
  
  // If device is already connected, will not disconnect on the end of process
  let shouldDisconnect = false

  let log = console.draft(this.TAG, 'authorize');

  async.waterfall([
    // Validate input data
    (next) => {
      if (value <= 0)
        return next('Payment value not valid: '+value)
      
      next()
    },

    // Connect to BLE
    async.timeout((next) => {
      // Skip connecting if already connected
      if (this.device.state == 'connected')
        return next()
      
      shouldDisconnect = true
      log(this.TAG, 'connecting')
      this.device.connect(next)
    }, CONSTS.DEFAULT_TIMEOUT_MS * 2),
    
    // Make sure device and account exists, and read characteristics
    this.getInfo.bind(this),

    // Get characteristics
    (info, next) => {
      log(this.TAG, 'getting characteristics...')
      characValue     = this.getCharacteristic(CONSTS.SERVICE_PAY_UUID, CONSTS.SERVICE_PAY_VALUE_UUID)
      characSignature = this.getCharacteristic(CONSTS.SERVICE_PAY_UUID, CONSTS.SERVICE_PAY_SIGNATURE_UUID)

      // Check they exist
      if (!characValue || !characSignature )
        return next('Some characteristics were not found.')
      
      next(null)
    },

    // DEBUG: Signature subscribe
    (next) => {
      console.log(this.TAG, 'subscribing to signature')
      onSignature = this.readLongCharacteristic(characSignature, next)
    },

    // Write the ammount and Wait Authorization. (Wrapped in a Timeout call)
    async.timeout((next) => {
      log(this.TAG, 'Waiting authentication')

      // Prepare buffer
      // let valueBuffer = new Buffer(4)
      // valueBuffer.writeFloatLE(value)

      // Write value
      characValue.write(iprChop, false)
      next()

    }, CONSTS.PAYMENT_TIMEOUT_MS, 'Could not Authorize. Timeout occurred'),

    (next) => {
      onSignature((err, token) => {
        if(err) {
          return next(err)
        }

        console.log('onSignature', err, token.toString())
        signature = token.slice(0, 63)
        publickey = token.slice(63)
        next(null, token)
      })
    },
    
    // Verify signature
    (authorization, next) => {
      // Check signature
      
      // Its ok. Proceed...
      next()
    },
  ], (err) => {
    log(this.TAG, 'finished', chalk.red(err), signature && signature.toString('HEX'))

    // Unsubscribe to changes
    characAuth && characAuth.unsubscribe()

    this.finishMaybeDisconnecting(shouldDisconnect, err, next, iprHash, signature, publickey)
  })
}