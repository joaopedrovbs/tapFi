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

#define TAPFI_DEBUG

// Hardware pins of the Beacon Board
#define LED_R 16
#define LED_G 12
#define LED_B 15
#define RUMBLE 20
#define SW1 8
#define SW2 18

// Timeouts
#define CONNECTION_TIMEOUT 15000  //Timeout for connection  
#define PASSWORDENTER_TIMEOUT 5000  //Timeout for password  
#define PASSWORDTYPE_TIMEOUT 3000  //Timeout for password  
#define TAP_TIMEOUT 1200   //Timeout for Taps

// Store the ED255199 Keys
unsigned char public_key[32], seed[32],private_key[64];

// Set Ripple as Manufacturer Data
const unsigned char manufacturerData[6] = {0x52, 0x69, 0x70, 0x70, 0x6c, 0x65}; 
const char *name = "admin";
const char *domain = "john.jpvbs.com";
const char *key = "johnadmin";
const char *nAuth = "NOK";
const char *nOK = "NeM";
float maxValue = 50;

// Flags for toggling the system
bool tapfiOn = true;

// Longs for Timeout and Debouncing
unsigned long timeConnected = 0;   
unsigned long timeOn = 0;

// Stored Double Tap Timing
int doubleTap[] = {
  0,
  500
};

// Stored Password Timing 
int password[] = {
  0,           
  600,
  200,
  200,
  300
};

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
BLECharacteristic cSignature = BLECharacteristic("ccc2", BLERead | BLENotify | BLEIndicate | BLEBroadcast, 20);
 
// Create one or more descriptors
BLEDescriptor dName = BLEDescriptor("2901", "name");
BLEDescriptor dDomain = BLEDescriptor("2901", "domain");
BLEDescriptor dAmount = BLEDescriptor("2901", "amount");
BLEDescriptor dAuthorize = BLEDescriptor("2901", "authorize");
BLEDescriptor dSignature = BLEDescriptor("2901", "authorize");

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

// Function to control the RGB led and Rumble Motor in a simpler way
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
    RGB();
    delay(duration_ms);
  }
}

// Function that Returns true if a Tap happened in any of it's Axis (X, Y, Z)
bool tapped(void){
  if( imu.fifoAvailable() ){                    //Checks for messages in FIFO
    imu.dmpUpdateFifo();                        //Updates DMP values from FIFO
    if( imu.tapAvailable() ){                   //If a Tap Event is Available
      unsigned char tapDir = imu.getTapDir();   //Get the tap Direction and return true
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

// Read the tap entered and store it in a password sequence
void readTap(void){
  unsigned long lastTap = millis();

  // Always clear before re-using
  tapDone.list.clear();

  // Wait Double Tap
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
bool checkDoubleTap(void){
  MatchParams params;                    // Configure match parameters
  params.timeScaleMax             = 1.0; // 100% speedup/speedown allowed
  params.timeScaleThrustFactor    = 0.2; //  ↑  will increase 20% of total thrust
  params.tapDurationScaleMax      = 0.2; // 20% speedup/speedown allowed in each interval
  params.tapDurationThrustFactor  = 0.1; //  ↑  will decrease 10% of total thrust

  // Compare passwords
  float thrust = MatchSequence(&tapPattern, &tapDone, params);
  
  // Check thrust
  #ifdef TAPFI_DEBUG
  Serial.print("Tap Thrust: \033[34m");
  Serial.print(thrust);
  Serial.println("\033[0m");
  #endif
  // Convert Thrust Values to Boolean
  if (thrust > 0.7) {
    return true;
  } else {
    return false;
  }
}

// Read the taps and store them in a PasswordSequence
void readPassword(void){
  unsigned long lastTap = millis();
  
  // Always clear before re-using
  passwordEntered.list.clear();

  // Wait password get completed
  while(passwordEntered.list.size() < sizeof(password)/ sizeof(int) || millis() - lastTap > PASSWORDENTER_TIMEOUT){
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