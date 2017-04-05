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
#include <ed25519.h>

#define TAPFI_DEBUG

// Hardware pins of the Beacon Board
#define LED_R 16
#define LED_G 12
#define LED_B 15
#define RUMBLE 20
#define SW1 8
#define SW2 18

// Max size for transfer 
#define CHARACTERISTIC_MAX_SIZE 20

// Timeouts
#define CONNECTION_TIMEOUT 15000  //Timeout for connection  
#define PASSWORDENTER_TIMEOUT 5000  //Timeout for password  
#define PASSWORDTYPE_TIMEOUT 3000  //Timeout for password  
#define TAP_TIMEOUT 1200   //Timeout for Taps

// Set Device Data
const unsigned char manufacturerData[6] = {0x52, 0x69, 0x70, 0x70, 0x6c, 0x65}; 
const char * name = "admin";
const char * domain = "john.jpvbs.com";
const char * deviceName = "Joao Pedro";

// Tap Parameters
unsigned short xThresh = 200;   // Set z-axis tap thresh to 200 mg/ms
unsigned short yThresh = 200;   // Set z-axis tap thresh to 200 mg/ms
unsigned short zThresh = 180;   // Set z-axis tap thresh to 180 mg/ms
unsigned char taps = 1;         // Set minimum taps to 1
unsigned short tapTime = 100;   // Set tap time to 100ms
unsigned short tapMulti = 100; // Set multi-tap time to 1s

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

//Crypto variables
unsigned char public_key[32], seed[32], signature[64], private_key[64];

// Flags for toggling the system
bool tapfiOn = true;

// Longs for Timeout
unsigned long timeOn = 0;
unsigned long timeConnected = 0;   
// Tap Timing
unsigned long totalTime = 0;

// Create IMU Instance
MPU9250_DMP imu;

// Create peripheral instance
BLEPeripheral tapfi = BLEPeripheral();

// Create the main services
BLEService service = BLEService("1823");

// Create The characteristics
BLECharacteristic cName = BLECharacteristic("fff1", BLERead /*| BLEWrite*/, name);
BLECharacteristic cDomain = BLECharacteristic("aaa1", BLERead /*| BLEWrite*/, domain);
BLECharacteristic cAmount = BLECharacteristic("bbb1", BLEWrite, 20);
BLECharacteristic cSignature = BLECharacteristic("ccc1", BLERead | BLENotify | BLEIndicate , 20);

// Create one or more descriptors
BLEDescriptor dName = BLEDescriptor("2901", "name");
BLEDescriptor dDomain = BLEDescriptor("2901", "domain");
BLEDescriptor dAmount = BLEDescriptor("2901", "amount");
BLEDescriptor dSignature = BLEDescriptor("2901", "signature");

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
      //digitalWrite(RUMBLE, HIGH);
    }
    RGB(r, g, b);
    delay(duration_ms);
    if (motor){
      //digitalWrite(RUMBLE, LOW);
    }
    RGB();
    delay(duration_ms);
  }
}

// Function for stopping the microcontroller and displaying errors when they occur
void haltError(int errorId, int code){
#ifdef TAPFI_DEBUG
  Serial.print("Error: #");
  Serial.print(errorId);
  Serial.print(" ( ");
  Serial.print(code);
  Serial.println(" ) ");    
#endif
  while(1){
#ifdef TAPFI_DEBUG      
    Serial.print(".");
#endif
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
          #ifdef TAPFI_DEBUG
          Serial.println("TAPPED XUP");
          #endif
          return true;
          break;
        case TAP_X_DOWN:
          #ifdef TAPFI_DEBUG
          Serial.println("TAPPED XDOWN");
          #endif
          return true;
          break;
        case TAP_Y_UP:
          #ifdef TAPFI_DEBUG
          Serial.println("TAPPED YUP");
          #endif
          return true;
          break;
        case TAP_Y_DOWN:
          #ifdef TAPFI_DEBUG
          Serial.println("TAPPED YDOWN");
          #endif
          return true;
          break;
        case TAP_Z_UP:
          #ifdef TAPFI_DEBUG
          Serial.println("TAPPED ZUP");
          #endif
          return true;
          break;
        case TAP_Z_DOWN:
          #ifdef TAPFI_DEBUG
          Serial.println("TAPPED ZDOWN");
          #endif
          return true;
          break;
      }
    }
  }
  return false;
}

// Load Saved Tap Pattern
void loadTap(){ 
  for (unsigned int i = 0 ; i < sizeof(doubleTap) / sizeof(int) ; i++) {
    SymbolInput symb;
    
    // Increment start time
    totalTime += doubleTap[i];

    symb.type = TAP;
    symb.start = totalTime;

    tapPattern.addSymbol(symb);
  }
 } 

// Read the tap entered and store it in a password sequence
void readTap(void){
  unsigned long lastTap = millis();

  // Always clear before re-using
  tapDone.list.clear();

  // Wait timeout && (tapDone.list.size() > 1 && millis() - lastTap < TAP_TIMEOUT)
  while( tapDone.list.size() < sizeof(doubleTap)/ sizeof(int)) {
    if (tapped()){
      // It will account for current time and create SymbolInput automatically
      tapDone.addSymbol(TAP);
      #ifdef TAPFI_DEBUG
        Serial.print("\033[36m TAP \033[0m");
        Serial.print(tapDone.list.size());
        Serial.print(" Time: \033[31m");
        Serial.print(tapDone.list.get(tapDone.list.size() - 1).start);
        Serial.println("\033[0m");
      #endif
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

  // Check thrust
  #ifdef TAPFI_DEBUG
    Serial.print("Thrust: \033[34m");
    Serial.print(thrust);
    Serial.println("\033[0m");
  #endif

  // Convert Thrust Values to Boolean
  if (thrust > 0.7) {
    return true;
  } 
  else {
    return false;
  }

}

// Load Saved Password 
  void loadPassword(){
  #ifdef TAPFI_DEBUG
    Serial.print("Password Size: ");
    Serial.println(sizeof(password)/ sizeof(int));
  #endif
  
  totalTime = 0;
  
  for (unsigned int i = 0 ; i < sizeof(password) / sizeof(int) ; i++) {
    SymbolInput symb;
    
    // Increment start time
    totalTime += password[i];

    symb.type = TAP;
    symb.start = totalTime;

    passwordSaved.addSymbol(symb);
  }
}
// Read the taps and store them in a PasswordSequence
void readPassword(void){
  unsigned long lastTap = millis();
  
  // Always clear before re-using
  passwordEntered.list.clear();

  // Wait timeout millis() - lastTap < PASSWORD_TIMEOUT
  while(passwordEntered.list.size() < sizeof(password)/ sizeof(int)){
    if (tapped()){
      // It will account for current time and create SymbolInput automatically
      passwordEntered.addSymbol(TAP);
      #ifdef TAPFI_DEBUG
        Serial.print("\033[36m TAP \033[0m");
        Serial.print(" Time: \033[31m");
        Serial.print(passwordEntered.list.get(passwordEntered.list.size() - 1).start);
        Serial.println("\033[0m");
      #endif
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
 
  #ifdef TAPFI_DEBUG
    Serial.print("Password Thrust: \033[34m");
    Serial.print(thrust);
    Serial.println("\033[0m");
  #endif
 
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
    #ifdef TAPFI_DEBUG
      Serial.println("Try Password");
    #endif
    RGB(1,1,1);
    readPassword();
    if(didPassword()){
      RGB();
      #ifdef TAPFI_DEBUG
        Serial.println("Password OK");
      #endif
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

void signatureWrite(){
  cSignature.setValue(signature+0, CHARACTERISTIC_MAX_SIZE);
  delay(50);
  cSignature.setValue(signature+20, CHARACTERISTIC_MAX_SIZE);
  delay(50);
  cSignature.setValue(signature+40, CHARACTERISTIC_MAX_SIZE);
  delay(50);
  cSignature.setValue(signature+60,4);
  delay(50);
  cSignature.setValue(public_key, CHARACTERISTIC_MAX_SIZE);
  delay(50);
  cSignature.setValue(public_key+20, 12);
  delay(50);
  cSignature.setValue("");
  #ifdef TAPFI_DEBUG
    Serial.println("Signature Written!");
  #endif
}

// void signatureErase(){
//   cSignature.setValue("");
//   #ifdef TAPFI_DEBUG
//     Serial.println("Signature Erased!");
//   #endif
// }

void generateKeys(void){
  ed25519_create_seed(seed);
  ed25519_create_keypair(public_key, private_key, seed);
  #ifdef TAPFI_DEBUG
    Serial.print("Public Key Size: ");
    Serial.println(sizeof(public_key));
    Serial.println("Public Key:");
    for (int i = 0; i < sizeof(public_key); i++){
      Serial.print(public_key[i],HEX);
    }
    Serial.println();
  #endif
}

void sign(){
  //const unsigned char message[] = "Hello, world!";
  unsigned const char* message = cAmount.value();
  const int message_len = sizeof(message);
  ed25519_sign(signature, message, message_len, public_key, private_key);
  #ifdef TAPFI_DEBUG
    Serial.print("Message Size: ");
    Serial.println(sizeof(message));
    Serial.print("Signature: ");
    Serial.println(sizeof(signature));
    for (int i = 0; i < sizeof(signature); i++){
      if(signature[i] < 16){
        Serial.print("0");
        Serial.print(signature[i],HEX);
      }
      else{
        Serial.print(signature[i],HEX);
      }
      Serial.print(" ");
    }
    Serial.println();
  #endif
}

//Check for connection from Central, do value checking and exchange 
void onConnect(){
 BLECentral central = tapfi.central();
  if(central){
    timeConnected=millis();
    while (central.connected() && millis() - timeConnected < CONNECTION_TIMEOUT) { 
      if(cAmount.written()){
        Serial.println("Amount Written!");
        if(passwordCheck(3)){
          sign();
          signatureWrite();
          break;
        }
      }
    }
  }
}

//////////////////////////////////// SETUP BEGIN ///////////////////////////////////// 

void setup() {
  // Serial Initialization
  int code;
  delay(200);

  #ifdef TAPFI_DEBUG
    // Just for Debug
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.println("Start");
  #endif

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
  tapfi.setLocalName(deviceName); 
  tapfi.setDeviceName(deviceName);
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

  tapfi.addAttribute(cSignature);
  tapfi.addAttribute(dSignature);

  //Starts the radio and the device
  tapfi.begin();
  
  #ifdef TAPFI_DEBUG
    Serial.println("TapFi Begin");
  #endif
  
  generateKeys();
  loadTap();
  loadPassword();

  // Start IMU
  if (code = imu.begin())
    haltError(1, code);
  // Enables Tap Recognition at 200hz
  if (code = imu.dmpBegin(DMP_FEATURE_TAP, 200))
    haltError(2, code);
  
  // Initializes Tap with Parameters
  if(code = imu.dmpSetTap(xThresh, yThresh, zThresh, taps, tapTime, tapMulti))
    haltError(3, code);
  #ifdef TAPFI_DEBUG
    Serial.println("Imu Begin");
  #endif
} // End Setup

//////////////////////////////////// LOOP BEGIN ///////////////////////////////////// 

void loop() {
  if(tapfiOn){
    RGB(1,0,0);
    #ifdef TAPFI_DEBUG
      Serial.println("BLE Should be OFF");
    #endif
    //keyErase();
    //signatureErase();
    tapfi.disconnect();
    tapfiOn=false;
  }

  // Read taps and record them to check
  readTap();
  
  //Check if there was a double-tap event
  if(didDoubleTap()){
    RGB();
    tapfi.poll();
    userAlert(1,500,1,0,0,0);
    #ifdef TAPFI_DEBUG
      Serial.println("BLE Should be ON");
    #endif
    RGB(0,1,0);
    tapfiOn=true;
    timeOn=millis();
    // Awaits for connection with a certain TIMEOUT
    while(millis() - timeOn < CONNECTION_TIMEOUT){
      onConnect();
    }
  }
}