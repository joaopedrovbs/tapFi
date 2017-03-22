#include "curve25519.h"
#include "handy.h"
#include "testutil.h"

// Copyright (C) 2010 Kairama Inc
void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
   char tmp[16];
   for (int i=0; i<length; i++) { 
     sprintf(tmp, "%.2X",data[i]); 
     Serial.print(tmp);
   }
}

void printKey(unsigned char *key){
  PrintHex8(key, 32);
  Serial.println();
}

static void test_base_mul(void)
{
  unsigned long start;
  uint8_t secret[32];
  uint8_t pub[32];
  uint8_t expect[32];

  // =========
  start = millis();
  Serial.println("\n\nTEST: test_base_mul[0]");
  
  unhex(secret, 32, "77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a");
  unhex(expect, 32, "8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a");
  cf_curve25519_mul_base(pub, secret);

  Serial.print("Secret: "); printKey(secret);
  Serial.print("Expect: "); printKey(expect);
  Serial.print("Result: "); printKey(pub);
  Serial.print("TimeMs: "); Serial.println(millis() - start);
  //TEST_CHECK(memcmp(expect, public, 32) == 0);

  // =========
  start = millis();
  Serial.println("\n\nTEST: test_base_mul[1]");
  
  unhex(secret, 32, "5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
  unhex(expect, 32, "de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f");
  cf_curve25519_mul_base((uint8_t *)pub, (uint8_t *)secret);

  Serial.print("Secret: "); printKey(secret);
  Serial.print("Expect: "); printKey(expect);
  Serial.print("Result: "); printKey(pub);
  Serial.print("TimeMs: "); Serial.println(millis() - start);
  //TEST_CHECK(memcmp(expect, public, 32) == 0);
  //TEST_CHECK(memcmp(expect, public, 32) == 0);
}

static void test_mul(void)
{
  unsigned long start;
  uint8_t scalar[32];
  uint8_t pub[32];
  uint8_t shared[32];
  uint8_t expect[32];

  // =========
  start = millis();
  Serial.println("\n\nTEST: test_mul[0]");
  
  unhex(scalar, 32, "77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a");
  unhex(pub,    32, "de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f");
  unhex(expect, 32, "4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742");
  cf_curve25519_mul(shared, scalar, pub);
  //TEST_CHECK(memcmp(expect, shared, 32) == 0);

  Serial.print("Scalar: "); printKey(scalar);
  Serial.print("Public: "); printKey(pub);
  Serial.print("Expect: "); printKey(expect);
  Serial.print("Result: "); printKey(shared);
  Serial.print("TimeMs: "); Serial.println(millis() - start);
}

//TEST_LIST = {
//  { "base-mul", test_base_mul },
//  { "mul", test_mul },
//  { 0 }
//};
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\nStarting\n");
  // put your setup code here, to run once:
  test_base_mul();
  test_mul();
  Serial.println("\nFinished.\n\n");
}

void loop() {
  // put your main code here, to run repeatedly:

}
