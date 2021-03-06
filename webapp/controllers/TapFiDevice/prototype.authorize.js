const CONSTS = require('./CONSTS')
/*
 * Requests an authentication key for the specified conditional payment.
 * Will return with the authentication key, or "null" if it didn't authorized
 */
module.exports = function authorize(value, destination, next) {
  let characAuth, characValue, authorization
  let packet, condition, fullPacket

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
      characAuth      = this.getCharacteristic(CONSTS.SERVICE_PAY_UUID, CONSTS.SERVICE_PAY_AUTHORIZE_UUID)
      characValue     = this.getCharacteristic(CONSTS.SERVICE_PAY_UUID, CONSTS.SERVICE_PAY_VALUE_UUID)

      // Check they exist
      if (!characAuth || !characValue)
        return next('Some characteristics were not found.')
      
      next(null)
    },

    // Subscribe to notification on Auth
    async.timeout((next) => {
      console.log(this.TAG, 'subscribing...')

      // Subscribe for changes
      characAuth.subscribe(next)
    }, CONSTS.DEFAULT_TIMEOUT_MS * 2, 'Could not subscribe to `auth`'),

    // Write the ammount and Wait Authorization. (Wrapped in a Timeout call)
    async.timeout((next) => {
      log(this.TAG, 'Waiting authentication')

      // Listen for changes in auth characteristic
      characAuth.once('data', (authKey) => {
        authorization = authKey.toString()
        // log(this.TAG, chalk.cyan('new data in AUTH:'), chalk.green(authorization))
        next(null, authorization)
      })

      // Prepare buffer
      let valueBuffer = new Buffer(4)
      valueBuffer.writeFloatLE(value)

      // Write value
      characValue.write(valueBuffer, false)

    }, CONSTS.PAYMENT_TIMEOUT_MS, 'Could not Authorize. Timeout occurred'),

    // Verify authorization
    (authorization, next) => {
      // Check for NotEnoughtMoney
      if (authorization == CONSTS.NOT_ENOUGH_MONEY) {
        return next('Refused payment')
      }

      // Its ok. Proceed...
      next()
    },
  ], (err) => {
    log(this.TAG, 'finished', chalk.red(err), authorization && authorization.toString())

    // Unsubscribe to changes
    characAuth && characAuth.unsubscribe()

    this.finishMaybeDisconnecting(shouldDisconnect, err, next, authorization)
  })
}