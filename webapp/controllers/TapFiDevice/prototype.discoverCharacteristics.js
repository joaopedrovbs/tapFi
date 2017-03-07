const CONSTS = require('./CONSTS')

/*
 * Loads all caracteristics in the device, and validates if the device has all necessary features
 */
module.exports = function discoverCharacteristics(next) {
  async.waterfall([
    // Find Services
    async.timeout((next) => {
      // console.log(this.TAG, 'find services')

      this.device.discoverServices([
        CONSTS.SERVICE_INFO_UUID
      ], next)
    }, CONSTS.DEFAULT_TIMEOUT_MS),

    // Validate Services, find Characteristics
    async.timeout((services, next) => {
      // console.log(this.TAG, 'validate services. find characteristics')

      let service = services.find(service => service && service.uuid === CONSTS.SERVICE_INFO_UUID)

      if (!service)
        return next('Invalid tapFi device. service not found: ' + (service && service.uuid))
      
      service.discoverCharacteristics(CONSTS.CHARACTERISTICS, next)
    }, CONSTS.DEFAULT_TIMEOUT_MS),

    // Validate Characteristics, read values
    async.timeout((characteristics, next) => {
      // console.log(this.TAG, 'validate characteristics. read values')

      let expectedCharacteristicsLength = CONSTS.CHARACTERISTICS.length
      if (characteristics.length !== expectedCharacteristicsLength)
        return next('Invalid tapFi device. Characteristics dont match: ' +
                    characteristics.length + ' != ' + expectedCharacteristicsLength)

      // Read values
      async.map(characteristics, (charac, next) => charac.read(next), next)
    }, CONSTS.DEFAULT_TIMEOUT_MS),
  ], next)
}