//////////////////////////////////////////////////////////////////////////
////////              TapFi Payment Device Firmware               ////////
////////              João Pedro Vilas & Ivan Seidel              ////////
////////                     Luxembourg, 2017                     ////////
//////////////////////////////////////////////////////////////////////////

// Main BLE Control Library
#include <BLEPeripheral.h>
#include <LinkedList.h>
#include "MatchSequence.h"
#include "SymbolInput.h"
#include "PasswordSequence.h"
#include <SparkFunMPU9250-DMP.h>

int savedPassword[] = {
  0,
  500
};

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
unsigned long time = 0;   
unsigned long timeOn = 0;
unsigned long debounce = 200;
unsigned long cTimeout = 15000;  //Timeout for connection  

// Create IMU Instance
MPU9250_DMP imu;

// Create peripheral instance
BLEPeripheral tapfi = BLEPeripheral();

// Create the main services
BLEService service = BLEService("1823");

// Create The characteristics
BLECharacteristic cName = BLECharacteristic("fff1", BLEBroadcast | BLERead /*| BLEWrite*/, name);
BLECharacteristic cDomain = BLECharacteristic("aaa1", BLEBroadcast | BLERead /*| BLEWrite*/, domain);
BLEFloatCharacteristic cAmount = BLEFloatCharacteristic("bbb1", BLEWrite);
BLECharacteristic cAuthorize = BLECharacteristic("ccc1", BLERead | BLENotify | BLEIndicate | BLEBroadcast, 20);
 
// Create one or more descriptors
BLEDescriptor dName = BLEDescriptor("2901", "name");
BLEDescriptor dDomain = BLEDescriptor("2901", "domain");
BLEDescriptor dAmount = BLEDescriptor("2901", "amount");
BLEDescriptor dAuthorize = BLEDescriptor("2901", "authorize");

PasswordSequence passwordSaved();
PasswordSequence passwordEntered();

void haltError(int errorId, int code){
  Serial.print("Error: #");
  Serial.print(errorId);
  Serial.print(" ( ");
  Serial.print(code);
  Serial.println(" ) ");    
  while(1){
    Serial.print(".");
    digitalWrite(LED_R, !digitalRead(LED_R));    
    delay(100);
  }  
}

int getTap(){
  if( imu.fifoAvailable() ){
    Serial.println("Got FIFO Available ");
    imu.dmpUpdateFifo();
    if( imu.tapAvailable() ){
      Serial.println("Got tapAvailable ");
      unsigned char tapDir = imu.getTapDir();
      unsigned char tapCnt = imu.getTapCount();
      switch (tapDir){
      case TAP_X_UP:
          break;
      case TAP_X_DOWN:
          break;
      case TAP_Y_UP:
          break;
      case TAP_Y_DOWN:
          break;
      case TAP_Z_UP:
         Serial.println("TAPPED UP");
          return 1;
          break;
      case TAP_Z_DOWN:
          Serial.println("TAPPED DOWN");
          return 1;
          break;
      }
    }
  }
}

bool checkPassword(){
  /*
   * Compares two sequences and return the thrust in being the same
   */

  // Configure match parameters
  MatchParams params;
  params.timeScaleMax             = 0.0005; // 50% speedup/speedown allowed
  params.timeScaleThrustFactor    = 0.0002; //  ↑  will decrease 20% of total thrust
  params.tapDurationScaleMax      = 0.0002; // 20% speedup/speedown allowed in each interval
  params.tapDurationThrustFactor  = 0.0001; //  ↑  will decrease 10% of total thrust

  // Compare passwords
  float thrust = MatchSequence(&passwordSaved, &passwordEntered, params);
  // Check thrust (0.7 might be something else.. need to test)
  if (thrust > 0.7) {
    return false;
  } else {
    return true;
  }
}

void getPasswordTap(){

  // Always clear before re-using
  passwordEntered.clear();

  // Wait timeout
  while(true) {
    if (getTap()) {
      // It will account for current time and create SymbolInput automatically
      passwordEntered.addSymbol(TAP);
    }
  }
}

void setup() {
  //Hardware Initialization
  pinMode(LED_R,OUTPUT);
  pinMode(LED_G,OUTPUT);
  pinMode(LED_B,OUTPUT); 
  pinMode(SW1,INPUT_PULLUP);
  pinMode(SW2,INPUT_PULLUP); 
  
  // Serial Initialization
  int code;
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println("Start");
  
  // Loading Saved Password
  unsigned long totalTime = 0;
  for (unsigned int i = 0 ; i < sizeof(savedPassword) / sizeof(int) ; i++) {
    SymbolInput symb;
    
    // Increment start time
    totalTime += savedPassword[i];

    symb.type = TAP;
    symb.start = totalTime;

    passwordSaved.addSymbol(symb);
  }
  
  // Start IMU with TAP enabled
  if (code = imu.begin())
    haltError(1, code);

  if (code = imu.dmpBegin(DMP_FEATURE_TAP, 10))
    haltError(2, code);
 
  unsigned short xThresh = 0;     // Disable x-axis tap
  unsigned short yThresh = 0;     // Disable y-axis tap
  unsigned short zThresh = 100;   // Set z-axis tap thresh to 100 mg/ms
  unsigned char taps = 1;         // Set minimum taps to 1
  unsigned short tapTime = 100;   // Set tap time to 100ms
  unsigned short tapMulti = 1000; // Set multi-tap time to 1s
  
  if(code = imu.dmpSetTap(xThresh, yThresh, zThresh, taps, tapTime, tapMulti))
    haltError(3, code);
  
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


  Serial.println("All Ok");
}

void loop() {
  getPasswordTap();
  bool activate = checkPassword();
  if(lastState == false && activate == false){
    if(isOn){
      isOn=false;
      tapfi.end();
    } else {
      isOn=true;
      tapfi.begin();
      timeOn=millis();
    }
  }
  lastState=activate;
  
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