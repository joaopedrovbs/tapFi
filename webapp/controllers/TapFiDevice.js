const NOT_ENOUGH_MONEY               = 'NeM'

const PAYMENT_TIMEOUT_MS             = 9000
const DEFAULT_TIMEOUT_MS             = 5000

const SERVICE_INFO_UUID              = '1823'
const SERVICE_INFO_DOMAIN_UUID       = 'aaa1'
const SERVICE_INFO_USERNAME_UUID     = 'fff1'
          
const SERVICE_PAY_UUID               = '1823'
const SERVICE_PAY_VALUE_UUID         = 'bbb1'
const SERVICE_PAY_AUTHORIZE_UUID     = 'ccc1'

const CHARACTERISTICS = [
  SERVICE_INFO_USERNAME_UUID,
  SERVICE_INFO_DOMAIN_UUID,

  SERVICE_PAY_VALUE_UUID,
  SERVICE_PAY_AUTHORIZE_UUID,
]

var TapFiDevice = module.exports = function (device) {
  this.TAG = newTAG('TapFi: '+device.advertisement.localName)
  this.device = device;
}

/*
 * Loads all caracteristics in the device, and validates if the device has all necessary features
 */
TapFiDevice.prototype.discoverCharacteristics = function (next) {
  async.waterfall([
    // Find Services
    async.timeout((next) => {
      console.log(this.TAG, 'find services')

      this.device.discoverServices([
        SERVICE_INFO_UUID
      ], next)
    }, DEFAULT_TIMEOUT_MS),

    // Validate Services, find Characteristics
    async.timeout((services, next) => {
      console.log(this.TAG, 'validate services. find characteristics')

      let service = services.find(service => service.uuid === SERVICE_INFO_UUID)

      if (!service)
        return next('Invalid tapFi device. service not found: ' + service.SERVICE_INFO_UUID)
      
      service.discoverCharacteristics(CHARACTERISTICS, next)
    }, DEFAULT_TIMEOUT_MS),

    // Validate Characteristics, read values
    async.timeout((characteristics, next) => {
      console.log(this.TAG, 'validate characteristics. read values')

      if (characteristics.length !== CHARACTERISTICS.length)
        return next('Invalid tapFi device. Characteristics dont match: ' + characteristics.length + ' != ' + CHARACTERISTICS.length)

      // Read values
      async.map(characteristics, (charac, next) => charac.read(next), next)
    }, DEFAULT_TIMEOUT_MS),
  ], next)
}

/*
 * Requests a transfer from the tapFi device, for the desired value and destination
 * Will call next on success or error (Failed to authorize/Transfer)
 *
 * Note: "completion" might get called before next with a status string
 *       (Should be used to show contextual status on the UI side)
 */
TapFiDevice.prototype.makePayment = function (value, destination, next, completion) {
  async.waterfall([
    // Set completion status
    (next) => { completion && completion('Authorizing...'); next() },

    // Get device's Authorization
    (next) => {
      this.authorize(value, destination, next)
    },

    // With password, make the transfer
    (authorization, next) => { 
      completion && completion('Transfering...')

      // Pull payment from desired account
      app.helpers.IlpKitApi.makePayment(this.info.acc, authorization, destination, value, next)
    },

  ], next)
}

/*
 * Requests an authentication key for the specified conditional payment.
 * Will return with the authentication key, or "null" if it didn't authorized
 */
TapFiDevice.prototype.authorize = function (value, destination, next) {
  let characAuth, characValue, authorization

  // If device is already connected, will not disconnect on the end of process
  let shouldDisconnect = ( this.device.state == 'disconnected' )

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
      
      console.log(this.TAG, 'connecting')
      this.device.connect(next)
    }, DEFAULT_TIMEOUT_MS * 2),
    
    // Make sure device and account exists, and read characteristics
    this.getInfo.bind(this),

    // Get characteristics
    (info, next) => {
      // console.log(this.TAG, 'getting characteristics...')
      characAuth  = this.getCharacteristic(SERVICE_PAY_UUID, SERVICE_PAY_AUTHORIZE_UUID)
      characValue = this.getCharacteristic(SERVICE_PAY_UUID, SERVICE_PAY_VALUE_UUID)

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
    }, DEFAULT_TIMEOUT_MS * 2),

    // Write the ammount and Wait Authorization. (Wrapped in a Timeout call)
    async.timeout((next) => {
      console.log(this.TAG, 'Waiting authentication')

      // Listen for changes in auth characteristic
      characAuth.once('data', (authKey) => {
        authorization = authKey.toString()
        // console.log(this.TAG, chalk.cyan('new data in AUTH:'), chalk.green(authorization))
        next(null, authorization)
      })

      // Prepare buffer
      let valueBuffer = new Buffer(4)
      valueBuffer.writeFloatLE(value)

      // Write value
      characValue.write(valueBuffer, false)

    }, PAYMENT_TIMEOUT_MS, 'Could not Authorize. Timeout occurred'),

    // Verify authorization
    (authorization, next) => {
      // Check for NotEnoughtMoney
      if (authorization == NOT_ENOUGH_MONEY) {
        return next('Refused payment')
      }

      // Its ok. Proceed...
      next()
    },
  ], (err) => {
    console.log(this.TAG, 'finished', err, authorization && authorization.toString())

    // Unsubscribe to changes
    characAuth && characAuth.unsubscribe()

    this._finishMaybeDisconnecting(shouldDisconnect, err, next, authorization)
  })
}

/*
 * Get's a single characteristic from the device with the service and characteristic UUID's 
 * Note: Must already be discovered
 *       this function is sync.
 *       No requests are made to BLE.
 */
TapFiDevice.prototype.getCharacteristic = function (serviceUUID, characteristicUUID) {
  // Find service
  let service = this.device.services.find(s => s.uuid == serviceUUID)
  if (!service)
    return null
  
  // Find characteristc
  let characteristic = service.characteristics.find(c => c.uuid == characteristicUUID)
  if (!characteristic)
    return null

  return characteristic
}

/*
 * Reads the information from the TapFi device (Account)
 * and fetches the user data from it's ledger.
 * Returns an object "info" with the folowing properties:
 *  avatar: <string>     | URL for the avatar
 *  domain: <string>     | Domain of it's ledger
 *  username: <string>   | Username of user 
 *  acc: <string>        | Concatenated `username@domain`
 *
 * Note: If information cannot be read from the ledger,
 * it will return `null` to represent corrupt/invalid data
 */
TapFiDevice.prototype.getInfo = function (next) {
  let self = this
  let info = self.info

  // if (info) 
    // return next(null, info)

  console.log(self.TAG, chalk.cyan.cyan('getInfo'))

  // If device is already connected, will not disconnect on the end of process
  let shouldDisconnect = ( self.device.state == 'disconnected' )

  async.waterfall([
    // Connect to BLE
    async.timeout((next) => {
      // Skip connecting if already connected
      if (self.device.state == 'connected')
        return next()

      console.log(self.TAG, 'connecting')
      self.device.connect(next)
    }, DEFAULT_TIMEOUT_MS * 2),

    async.timeout(self.discoverCharacteristics.bind(self), DEFAULT_TIMEOUT_MS),

    // Process data
    (values, next) => {
      // Unpack read values (must be in order of CHARACTERISTICS)
      let [username, domain] = values

      info = {}
      info.domain = domain.toString().replace('.heroku', '.herokuapp.com')
      info.username = username.toString()
      info.acc = info.username + '@' + info.domain

      next()
    },

    // Gets the destination information (url) and confirms it's authenticity
    async.timeout((next) => {
      app.helpers.IlpKitApi.getAccountInfo(info.acc, next)
    }, DEFAULT_TIMEOUT_MS),

    // Update the tapFi info
    (ilpKitInfo, next) => {
      info.avatar = ilpKitInfo.imageUrl
      info.ledgerUri = ilpKitInfo.ledgerUri
      info.paymentUri = ilpKitInfo.paymentUri
      info.name = ilpKitInfo.name
      self.info = info

      next()
    },
  ], (err) => {
    console.log(self.TAG, 'finished', err)
    self._finishMaybeDisconnecting(shouldDisconnect, err, next, self.info)
  })
}

/*
 * Helper method to disconnect and callback if flag "disconnect" is set to true
 */
TapFiDevice.prototype._finishMaybeDisconnecting = function (shouldDisconnect, err, callback, ...params) {
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