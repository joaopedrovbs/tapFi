/*
 * Get's a single characteristic from the device with the service and characteristic UUID's 
 * Note: Must already be discovered
 *       this function is sync.
 *       No requests are made to BLE.
 */
module.exports = function getCharacteristic(serviceUUID, characteristicUUID) {
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