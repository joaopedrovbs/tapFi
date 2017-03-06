//////////////////////////////////////////////////////////////////////////
////////              TapFi Payment Device Firmware               ////////
////////              João Pedro Vilas & Ivan Seidel              ////////
////////                     Luxembourg, 2017                     ////////
//////////////////////////////////////////////////////////////////////////

// Main BLE Control Library
#include <BLEPeripheral.h> 

// Hardware pins of the Beacon Board
#define LED_R 16
#define LED_G 12
#define LED_B 15
#define SW1 8
#define SW2 18

// Set Ripple as Manufacturer Data
const unsigned char manufacturerData[6] = {0x52, 0x69, 0x70, 0x70, 0x6c, 0x65}; 
const char * name = "admin";
const char * domain = "john.jpvbs.com";
const char * key = "johnadmin";
const char * nAuth = "NOK";
const char * nOK = "NeM";
float maxValue = 50;

// Flags for toggling the system
bool readingSW1 = false;
bool isOn = false;
bool lastState = false;

// Longs for Timeout and Debouncing
long time = 0;   
long timeOn = 0;
long debounce = 200;
long cTimeout = 15000;  //Timeout for connection  

// Create peripheral instance
BLEPeripheral tapfi = BLEPeripheral();

// Create the main services
BLEService service = BLEService("1823");

// Create The characteristics
BLECharacteristic cName = BLECharacteristic("fff1", BLEBroadcast | BLERead /*| BLEWrite*/, name);
BLECharacteristic cDomain = BLECharacteristic("aaa1", BLEBroadcast | BLERead /*| BLEWrite*/, domain);
BLEFloatCharacteristic cAmount = BLEFloatCharacteristic("bbb1", BLEWrite);
BLECharacteristic cAuthorize = BLECharacteristic("ccc1", BLERead | BLENotify | BLEIndicate | BLEBroadcast, 20);

// create one or more descriptors
BLEDescriptor dName = BLEDescriptor("2901", "name");
BLEDescriptor dDomain = BLEDescriptor("2901", "domain");
BLEDescriptor dAmount = BLEDescriptor("2901", "amount");
BLEDescriptor dAuthorize = BLEDescriptor("2901", "authorize");

void setup() {
  // Setting Up BLE
  tapfi.setConnectable(true);
  tapfi.setAppearance(0x0080);
  tapfi.setTxPower(-30);
  tapfi.setLocalName("João Pedro Vilas Boas"); 
  tapfi.setDeviceName("João Pedro Vilas Boas");
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
  pinMode(LED_R,OUTPUT);
  pinMode(LED_G,OUTPUT);
  pinMode(LED_B,OUTPUT); 
  pinMode(SW1,INPUT);
  pinMode(SW2,INPUT);
   
  // Begin initialization
 
}
void loop() {
  bool readingSW1 = digitalRead(SW1);
  
  if(lastState == false && readingSW1 == false  && millis() - time > debounce){
    if(isOn){
      isOn=false;
      tapfi.end();
    }else{
      isOn=true;
      tapfi.begin();
      timeOn=millis();
    }
    time = millis();  
  }
  lastState=readingSW1;
  if(isOn){
    digitalWrite(LED_G,LOW);
    digitalWrite(LED_R,HIGH);
    digitalWrite(LED_B,HIGH);
    tapfi.poll();
    BLECentral central = tapfi.central();
    if (central){
      while (central.connected()) {
        if(cAmount.written()){
          if(maxValue - cAmount.value() >= 0 && (float) cAmount.value() > 0 && digitalRead(SW1)){
            cAuthorize.setValue(key);
            cAuthorize.canNotify();
          }else{
            cAuthorize.setValue(nOK);
          }
        }
      }
    }
  }else{
    digitalWrite(LED_G,HIGH);
    digitalWrite(LED_B,HIGH);
    digitalWrite(LED_R,LOW);
  }
  if(millis() - timeOn > cTimeout){
      isOn=false;
      timeOn=millis();
      tapfi.end();
  }
}