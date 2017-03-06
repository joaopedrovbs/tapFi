/*
 * Object representing a tapFi device, that can:
 *  1. Get info from the device, parse with the ledger,
 *     and return an info object (prototype.getInfo.js)
 *  2. Authorize a payment, by getting a token from the 
 *     device. (prototype.authorize.js)
 *  3. Make a payment to a desired destination account from
 *     the tapFi account (prototype.makePayment.js)
 */
var TapFiDevice = module.exports = function (device) {
  // Make a shorter name
  let shortName = device.advertisement.localName
  let nameParts = shortName.split(' ')
  shortName = nameParts.shift()
  shortName += ' ' + nameParts.map(p => p.substring(0, 1).toUpperCase() + '.').join(' ')

  this.TAG = newTAG('💳  '+shortName)+' '
  this.device = device;
}