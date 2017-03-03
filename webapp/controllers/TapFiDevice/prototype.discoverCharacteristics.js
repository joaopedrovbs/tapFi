/*
 * Loads all caracteristics in the device, and validates if the device has all necessary features
 */
module.exports = function discoverCharacteristics(next) {
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