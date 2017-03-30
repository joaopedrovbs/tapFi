/******************************************************************************
arduino_mpu9250_i2c.cpp - MPU-9250 Digital Motion Processor Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

This library implements motion processing functions of Invensense's MPU-9250.
It is based on their Emedded MotionDriver 6.12 library.
	https://www.invensense.com/developers/software-downloads/

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
******************************************************************************/
#include "arduino_mpu9250_i2c.h"
#include <Arduino.h>
#include <Wire.h>

void halt(const char * tag, int code){
	if(!code)
		return;
		
	Serial.println("\n\n===== HALT! =====");
	Serial.print(tag);
	Serial.print(": ");
	Serial.println(code);
	Serial.println("=====   ;   =====\n\n");
	// while(1);
}

int arduino_i2c_write(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	// Serial.print(slave_addr, HEX);
	// Serial.print(" \033[35m"); Serial.print(reg_addr, HEX); Serial.print("\033[0m\t");
	// Serial.print("\033[34m > Out: \033[0m");

	Wire.beginTransmission(slave_addr);
	Wire.write(reg_addr);
	for (unsigned char i = 0; i < length; i++)
	{
		Wire.write(data[i]);
		// Serial.print(data[i], HEX);
		// Serial.print(" ");
	}
	Wire.endTransmission(true);
	// Serial.println(" ;");

	return 0;
}

int arduino_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	// Serial.print(slave_addr, HEX);
	// Serial.print(" \033[35m"); Serial.print(reg_addr, HEX); Serial.print("\033[0m\t");
	// Serial.print("\033[36m < Got:\033[0m ");
	// Serial.print("\033[31m [");

	Wire.beginTransmission(slave_addr);
	Wire.write(reg_addr);
	Wire.endTransmission(false);

	int bytesRead = Wire.requestFrom(slave_addr, length);
 
	// Serial.print(bytesRead); 
	// Serial.print(" of "); 
	// Serial.print(length); 
	// Serial.print("]\033[0m ");
	for (unsigned char i = 0; i < length; i++)
	{
		data[i] = Wire.read();
		// Serial.print(data[i], HEX);
		// Serial.print(" ");
	}
	
	// Serial.println(" ;");
	

	return 0;
}
