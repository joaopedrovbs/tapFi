const CONSTS = require('./CONSTS')

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
module.exports = function getInfo(next) {
  let self = this
  let info = self.info

  // if (info) 
    // return next(null, info)

  let TAG = self.TAG + ' ' + chalk.cyan.cyan('getInfo')
  let log = console.draft(TAG)

  // If device is already connected, will not disconnect on the end of process
  let shouldDisconnect = ( self.device.state == 'disconnected' )

  async.waterfall([
    // Connect to BLE
    async.timeout((next) => {
      // Skip connecting if already connected
      if (self.device.state == 'connected')
        return next()

      log(TAG, 'connecting')
      self.device.connect(next)
    }, CONSTS.DEFAULT_TIMEOUT_MS * 2),

    async.timeout(self.discoverCharacteristics.bind(self), CONSTS.DEFAULT_TIMEOUT_MS),

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
      log(TAG, 'Getting account info')
      app.helpers.IlpKitApi.getAccountInfo(info.acc, next)
    }, CONSTS.DEFAULT_TIMEOUT_MS),

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
    log(TAG, 'finished', err ? chalk.red(err) : chalk.green('OK'))
    
    // console.log(self.TAG, 'finished', err)
    self.finishMaybeDisconnecting(shouldDisconnect, err, next, self.info)
  })
}