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

// Stored Double Tap Timing
int doubleTap[] = {
  0,
  500
};

// Stored Password Timing 
int password[] = {
  0,             // 0, 600, 800, 1000, 1300 
  600,
  200,
  200,
  300
};

// Hardware pins of the Beacon Board
#define LED_R 16
#define LED_G 12
#define LED_B 15
#define RUMBLE 20
#define SW1 8
#define SW2 18

// Timeouts
#define CONNECTION_TIMEOUT 20000  //Timeout for connection  
#define PASSWORDENTER_TIMEOUT 5000  //Timeout for password  
#define PASSWORDTYPE_TIMEOUT 3000  //Timeout for password  
#define TAP_TIMEOUT 1200   //Timeout for Taps

// Set Ripple as Manufacturer Data
const unsigned char manufacturerData[6] = {0x52, 0x69, 0x70, 0x70, 0x6c, 0x65}; 
const char * name = "admin";
const char * domain = "john.jpvbs.com";
const char * key = "johnadmin";
const char * nAuth = "NOK";
const char * nOK = "NeM";
float maxValue = 50;

// Flags for toggling the system
bool tapfiOn = true;
bool connected = false;

// Longs for Timeout and Debouncing
unsigned long timeConnected = 0;   
unsigned long timeOn = 0;

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
BLECharacteristic signature = BLECharacteristic("ccc2", BLERead | BLENotify | BLEIndicate | BLEBroadcast, 20);
 
// Create one or more descriptors
BLEDescriptor dName = BLEDescriptor("2901", "name");
BLEDescriptor dDomain = BLEDescriptor("2901", "domain");
BLEDescriptor dAmount = BLEDescriptor("2901", "amount");
BLEDescriptor dAuthorize = BLEDescriptor("2901", "authorize");

// Instantiate Password Sequences
PasswordSequence tapPattern;
PasswordSequence tapDone;
PasswordSequence passwordSaved;
PasswordSequence passwordEntered;

// Function to control the RGB led in a simpler way
void RGB(bool r = false,bool g = false, bool b = false){
  digitalWrite(LED_R, !r);
  digitalWrite(LED_G, !g);
  digitalWrite(LED_B, !b);
}

void userAlert(short rep, short duration_ms, bool motor = true, bool r = false, bool g = false, bool b = false){
  RGB();
  for(int a = 0; a < rep; a++){
    if (motor){
      digitalWrite(RUMBLE, HIGH);
    }
    RGB(r, g, b);
    delay(duration_ms);
    if (motor){
      digitalWrite(RUMBLE, LOW);
    }
    RGB(r, g, b);
    delay(duration_ms);
  }
}

// Function for stopping the microcontroller and displaying errors when they occur
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

// Returns True if a TAP happened on the Z-Axis, either up or down
bool tapped(){
  if( imu.fifoAvailable() ){
    imu.dmpUpdateFifo();
    if( imu.tapAvailable() ){
      unsigned char tapDir = imu.getTapDir();
      switch (tapDir){
        case TAP_X_UP:
          Serial.println("TAPPED XUP");
          return true;
          break;
        case TAP_X_DOWN:
          Serial.println("TAPPED XDOWN");
          return true;
          break;
        case TAP_Y_UP:
          Serial.println("TAPPED YUP");
          return true;
          break;
        case TAP_Y_DOWN:
          Serial.println("TAPPED YDOWN");
          return true;
          break;
        case TAP_Z_UP:
          Serial.println("TAPPED ZUP");
          return true;
          break;
        case TAP_Z_DOWN:
          Serial.println("TAPPED ZDOWN");
          return true;
          break;
      }
    }
  }
  return false;
}

// Read the tap entered and store it in a password sequence
void readTap(bool debug = false){
  unsigned long lastTap = millis();

  // Always clear before re-using
  tapDone.list.clear();

  // Wait timeout && (tapDone.list.size() > 1 && millis() - lastTap < TAP_TIMEOUT)
  while( tapDone.list.size() < sizeof(doubleTap)/ sizeof(int)) {
    if (tapped()){
      // It will account for current time and create SymbolInput automatically
      tapDone.addSymbol(TAP);
      if(debug){
        Serial.print("\033[36m TAP \033[0m");
        Serial.print(tapDone.list.size());
        Serial.print(" Time: \033[31m");
        Serial.print(tapDone.list.get(tapDone.list.size() - 1).start);
        Serial.println("\033[0m");
      }
      lastTap = millis();
    }
    if(tapDone.list.size() == 1 && millis() - lastTap > TAP_TIMEOUT)
      break;
  }
}

// Check if the double tap entered is equal to the one stored returns true if ok 
bool didDoubleTap(){
  MatchParams params;                    // Configure match parameters
  params.timeScaleMax             = 1.0; // 50% speedup/speedown allowed
  params.timeScaleThrustFactor    = 0.2; //  ↑  will decrease 20% of total thrust
  params.tapDurationScaleMax      = 0.2; // 20% speedup/speedown allowed in each interval
  params.tapDurationThrustFactor  = 0.1; //  ↑  will decrease 10% of total thrust

  // Compare passwords
  float thrust = MatchSequence(&tapPattern, &tapDone, params);
  
  // Check thrust (0.7 might be something else.. need to test)
  Serial.print("Thrust: \033[34m");
  Serial.print(thrust);
  Serial.println("\033[0m");
  
  // Convert Thrust Values to Boolean
  if (thrust > 0.7) {
    return true;
  } else {
    return false;
  }
}


// Read the taps and store them in a PasswordSequence
void readPassword(bool debug = false){
  unsigned long lastTap = millis();
  
  // Always clear before re-using
  passwordEntered.list.clear();

  // Wait timeout millis() - lastTap < PASSWORD_TIMEOUT
  while(passwordEntered.list.size() < sizeof(password)/ sizeof(int) || millis() - lastTap > PASSWORDENTER_TIMEOUT){
    if (tapped()){
      // It will account for current time and create SymbolInput automatically
      passwordEntered.addSymbol(TAP);
      if(debug){
        Serial.print("\033[36m TAP \033[0m");
        Serial.print(" Time: \033[31m");
        Serial.print(passwordEntered.list.get(passwordEntered.list.size() - 1).start);
        Serial.println("\033[0m");
      }
      lastTap = millis();
    }
  if(passwordEntered.list.size() >= 1 && millis() - lastTap > PASSWORDTYPE_TIMEOUT)
      break;
  }
}

// Check the password the user entered against the recorded one, true if ok
bool didPassword(){
  MatchParams params;                    // Configure match parameters
  params.timeScaleMax             = 1.0; // 50% speedup/speedown allowed
  params.timeScaleThrustFactor    = 0.2; //  ↑  will decrease 20% of total thrust
  params.tapDurationScaleMax      = 0.4; // 20% speedup/speedown allowed in each interval
  params.tapDurationThrustFactor  = 0.1; //  ↑  will decrease 10% of total thrust
  // Compare passwords
  float thrust = MatchSequence(&passwordSaved, &passwordEntered, params);
  // Check thrust (0.7 might be something else.. need to test)
  Serial.print("Thrust 2: \033[34m");
  Serial.print(thrust);
  Serial.println("\033[0m");
  //  Converts trust to boolean
  if (thrust > 0.7) {
    return true;
  } else {
    return false;
  }
}

// Password retries and matching
bool passwordCheck(int retries){
  RGB(0,0,1);
  delay(200);
  for(int s=0; s < retries; s++){
    RGB();
    Serial.println("Try Password");
    RGB(1,1,1);
    readPassword(true);
    if(didPassword()){
      RGB();
      Serial.println("Password OK");
      userAlert(1,500,1,0,1,0);
      return true;
      break;
    }
    else{
      RGB();
      userAlert(2,500,1,1,1,0);
    }
  }

  return false;
}

void longWrite(){
  signature.setValue("1 PRIMEIRO VALOR");
  signature.setValue("2 SEGUNDO VALOR");
  signature.setValue("3 TERCEIRO VALOR");
  signature.setValue("4 QUARTO VALOR");
  signature.setValue("5 QUINTO VALOR");
  signature.setValue("6 QUINTO VALOR");
  signature.setValue("");
}

char input[128];
void longRead(){
  int offset = 0;
  unsigned long start = millis()
  // While not full, not timedout
  while(offset < 128 && millis() - start < 500) {
    if (signature.written()) {
      int len = signature.valueLength()

      // Stop if empty
      if (len == 0) {
        break;
      }
      Serial.print("GOT [part]: ")
      Serial.println(signature.value());
      memcpy(input + offset, signature.value(), len);
      offset += len;
    }
  }
  input[offset + 1] = '\0';

  Serial.print("GOT [all]: ")
  Serial.println(input);
}


void setup() {
  // Serial Initialization
  int code;
  delay(200);
  Serial.begin(115200);

  // Just for Debug
  Serial.println();
  Serial.println();
  Serial.println("Start");
  
  //Hardware Initialization
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(RUMBLE, OUTPUT); 
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP); 
  RGB();
  
  // Setting Up BLE
  tapfi.setConnectable(true);
  tapfi.setAppearance(0x0080);
  tapfi.setTxPower(-30);
  tapfi.setLocalName("João Pedro"); 
  tapfi.setDeviceName("João Pedro");
  tapfi.setManufacturerData(manufacturerData, manufacturerData[6]);
  
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

  //Starts the radio and the device
  tapfi.begin();
 
  Serial.println("TapFi Begin");
  
  // Load Saved Tap Pattern
  unsigned long totalTime = 0;
  for (unsigned int i = 0 ; i < sizeof(doubleTap) / sizeof(int) ; i++) {
    SymbolInput symb;
    
    // Increment start time
    totalTime += doubleTap[i];

    symb.type = TAP;
    symb.start = totalTime;

    tapPattern.addSymbol(symb);
  }
  Serial.print("Password Size: ");
  Serial.println(sizeof(password)/ sizeof(int));
  // Load Saved Password 
  totalTime = 0;
  for (unsigned int i = 0 ; i < sizeof(password) / sizeof(int) ; i++) {
    SymbolInput symb;
    
    // Increment start time
    totalTime += password[i];

    symb.type = TAP;
    symb.start = totalTime;

    passwordSaved.addSymbol(symb);
  }

  // Start IMU
  if (code = imu.begin())
    haltError(1, code);
  // Enables Tap Recognition at 200hz
  if (code = imu.dmpBegin(DMP_FEATURE_TAP, 200))
    haltError(2, code);

  // Tap Parameters
  unsigned short zThresh = 180;   // Set z-axis tap thresh to 100 mg/ms
  unsigned char taps = 1;         // Set minimum taps to 1
  unsigned short tapTime = 100;   // Set tap time to 100ms
  unsigned short tapMulti = 100; // Set multi-tap time to 1s
  
  // Initializes Tap with Parameters
  if(code = imu.dmpSetTap(200, 200, zThresh, taps, tapTime, tapMulti))
    haltError(3, code);

  Serial.println("Imu Begin");
} // End Setup

void loop() {
  //
  if(tapfiOn){
    RGB(1,0,0);
    Serial.println("BLE Should be OFF");
    tapfi.disconnect();
    cAuthorize.setValue(nAuth);
    tapfiOn=false;
  }
  // Read taps and record them to check
  readTap(true);
  //Check if there was a double-tap event
  if(didDoubleTap()){
    RGB();
    tapfi.poll();
    userAlert(1,500,1,0,1,0);
    Serial.println("BLE Should be ON");
    tapfiOn=true;
    timeOn=millis();
    RGB(0,1,0);
    // Awaits for connection with a certain TIMEOUT
    while(millis() - timeOn < CONNECTION_TIMEOUT){
      didConnect();
    }
  }
}

//Check for connection from Central, do value checking and exchange 
void didConnect(){
 BLECentral central = tapfi.central();
  if(central){
    timeConnected=millis();
    while (central.connected() && millis() - timeConnected < CONNECTION_TIMEOUT) { 
      if(cAmount.written()){
        longWrite();
        Serial.print("Amount is: ");
        Serial.println(cAmount.value());
        if(maxValue - cAmount.value() >= 0 && (float) cAmount.value() > 0 && passwordCheck(3)){
          Serial.print("Key written: ");
          Serial.println(key);
          cAuthorize.setValue(key);



          tapfi.disconnect();
          break;
        }
        else{
          cAuthorize.setValue(nOK);
          break;
        }
      }
    }
    //tapfi.disconnect();
  }
  // else{
  //   tapfi.disconnect();
  // }
}
