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
const char * key = "johnadmin";
const char * nAuth = "NOK";
const char * nOK = "NeM";
const char * password = "admin";
float maxValue = 100;

// Create peripheral instance
BLEPeripheral tapfi = BLEPeripheral();

// Create the main services
BLEService service = BLEService("1823");

// Create The characteristics
BLECharacteristic cName = BLECharacteristic("fff1", BLEBroadcast | BLERead /*| BLEWrite*/, name);
BLECharacteristic cDomain = BLECharacteristic("aaa1", BLEBroadcast | BLERead /*| BLEWrite*/, domain);
BLEFloatCharacteristic cAmount = BLEFloatCharacteristic("bbb1", BLEWrite);
BLECharacteristic cAuthorize = BLECharacteristic("ccc1", BLERead | BLENotify | BLEIndicate | BLEBroadcast, 26);

// create one or more descriptors
BLEDescriptor dName = BLEDescriptor("2901", "name");
BLEDescriptor dDomain = BLEDescriptor("2901", "domain");
BLEDescriptor dAmount = BLEDescriptor("2901", "amount");
BLEDescriptor dAuthorize = BLEDescriptor("2901", "authorize");

void setup() {
  // Setting Up BLE
  tapfi.setConnectable(true);
  tapfi.setAppearance(0x0080);
  tapfi.setLocalName("João Pedro Vilas Boas"); 
  tapfi.setDeviceName("TapFi");
  tapfi.setManufacturerData(manufacturerData, sizeof(manufacturerData));
  //Interval in ms
  tapfi.setAdvertisingInterval(500);
  //UUID for this Device
  tapfi.setAdvertisedServiceUuid("e0b76bd0-fda4-11e6-bc64-92361f002671"); 
  // Add attributes (services, characteristics, descriptors) to peripheral
  tapfi.addAttribute(service);
  tapfi.addAttribute(cName);
  tapfi.addAttribute(dName);
  tapfi.addAttribute(cDomain);
  tapfi.addAttribute(dDomain);
  tapfi.addAttribute(cAmount);
  tapfi.addAttribute(dAmount);
  tapfi.addAttribute(cAuthorize);
  tapfi.addAttribute(dAuthorize);
  
  // Begin initialization
  tapfi.begin();
}
void loop() {
  tapfi.poll();
  BLECentral central = tapfi.central();
  if (central){
    while (central.connected()) {
      if(cAmount.written()){
        if(maxValue - cAmount.value() >= 0 && (float) cAmount.value() > 0){   
          cAuthorize.setValue(key);
          cAuthorize.canNotify();
        }else{
          cAuthorize.setValue(nOK);
        }
      }
    }
  }else{
    cAuthorize.setValue(nAuth);
  }
}