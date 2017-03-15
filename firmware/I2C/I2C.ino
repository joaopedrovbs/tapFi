#include <BLEPeripheral.h>
#include "MPU9250.h"
#include <SparkFunMPU9250-DMP.h>

#define LEDR 16
#define LEDG 12
#define LEDB 15

MPU9250_DMP imu;

void haltError(int errorId, int code){
  Serial.print("Error: #");
  Serial.print(errorId);
  Serial.print(" ( ");
  Serial.print(code);
  Serial.println(" ) ");    
  while(1){
    Serial.print(".");
    digitalWrite(LEDR, !digitalRead(LEDR));    
    delay(100);
  }  
}

void setup(){
  int code;
  delay(2000);
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println("Start");
  
  pinMode(LEDR,OUTPUT);
  pinMode(LEDG,OUTPUT);
  pinMode(LEDB,OUTPUT);
  
  digitalWrite(LEDR,true);
  digitalWrite(LEDG,true);
  digitalWrite(LEDB,true);

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

  Serial.println("All Ok");
}

void loop(){   
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
          break;
      case TAP_Z_DOWN:
          Serial.println("TAPPED DOWN");
          break;
      }
    }
  }
}
