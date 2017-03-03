global.NOT_ENOUGH_MONEY               = 'NeM'

global.PAYMENT_TIMEOUT_MS             = 9000
global.DEFAULT_TIMEOUT_MS             = 5000

global.SERVICE_INFO_UUID              = '1823'
global.SERVICE_INFO_DOMAIN_UUID       = 'aaa1'
global.SERVICE_INFO_USERNAME_UUID     = 'fff1'
          
global.SERVICE_PAY_UUID               = '1823'
global.SERVICE_PAY_VALUE_UUID         = 'bbb1'
global.SERVICE_PAY_AUTHORIZE_UUID     = 'ccc1'

global.CHARACTERISTICS = [
  SERVICE_INFO_USERNAME_UUID,
  SERVICE_INFO_DOMAIN_UUID,

  SERVICE_PAY_VALUE_UUID,
  SERVICE_PAY_AUTHORIZE_UUID,
]

var TapFiDevice = module.exports = function (device) {
  this.TAG = newTAG('TapFi: '+device.advertisement.localName)
  this.device = device;
}