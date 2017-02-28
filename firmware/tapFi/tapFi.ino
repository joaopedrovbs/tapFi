//////////////////////////////////////////////////////////////////////////
////////              TapFi Payment Device Firmware               ////////
////////              João Pedro Vilas & Ivan Seidel              ////////
////////                     Luxembourg, 2017                     ////////
//////////////////////////////////////////////////////////////////////////

// Main BLE Control Library
#include <BLEPeripheral.h> 

// Set Ripple as Manufacturer Data
const unsigned char manufacturerData[6] = {0x52, 0x69, 0x70, 0x70, 0x6c, 0x65}; 
const char * name = "admin";
const char * domain = "john.jpvbs.com";
const char * auth = "pay";
const char * password = "admin";

// Create peripheral instance
BLEPeripheral tapfi = BLEPeripheral();

// Create the main services
BLEService service = BLEService("1823");

// Create The characteristics
BLECharacteristic characteristic = BLECharacteristic("fff1", BLEBroadcast | BLERead /*| BLEWrite*/, name);
BLECharacteristic characteristicD = BLECharacteristic("aaa1", BLEBroadcast | BLERead /*| BLEWrite*/, domain);
BLECharacteristic characteristicA = BLECharacteristic("bbb1", BLERead | BLEWrite, 20);

// create one or more descriptors
BLEDescriptor descriptor = BLEDescriptor("ffff", "name");
BLEDescriptor descriptorD = BLEDescriptor("aaaa", "domain");
BLEDescriptor descriptorA = BLEDescriptor("bbbb", "authentication");

void setup() {
  // Setting Up BLE
  tapfi.setConnectable(true);
  tapfi.setAppearance(0x0080);
  tapfi.setLocalName("TapFi"); 
  tapfi.setDeviceName("TapFi");
  tapfi.setManufacturerData(manufacturerData, sizeof(manufacturerData));
  //Interval in ms
  tapfi.setAdvertisingInterval(500);
  //UUID for this Device
  tapfi.setAdvertisedServiceUuid("e0b76bd0-fda4-11e6-bc64-92361f002671"); 
  // Add attributes (services, characteristics, descriptors) to peripheral
  tapfi.addAttribute(service);
  tapfi.addAttribute(characteristic);
  tapfi.addAttribute(descriptor);
  tapfi.addAttribute(characteristicD);
  tapfi.addAttribute(descriptorD);
  tapfi.addAttribute(characteristicA);
  tapfi.addAttribute(descriptorA);
  // Begin initialization
  tapfi.begin();
}
void loop() {
  BLECentral central = tapfi.central();
  if (central){
    while (central.connected()) {
      const unsigned char * got = characteristicA.value();
      if(got == (const unsigned char *) auth) {
        // central still connected to peripheral
        characteristicA.setValue(password);
      }
    }
  }
}