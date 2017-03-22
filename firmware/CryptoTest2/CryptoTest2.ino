#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* #define ED25519_DLL */
#include <ed25519.h>

#include <ge.h>
#include <sc.h>


int test() {
  unsigned char public_key[32], private_key[64], seed[32], scalar[32];
  unsigned char other_public_key[32], other_private_key[64];
  unsigned char shared_secret[32], other_shared_secret[32];
  unsigned char signature[64];

  unsigned long start;
  unsigned long end;
  int i;

  const unsigned char message[] = "Hello, world!";
  const int message_len = strlen((char*) message);

  /* create a random seed, and a keypair out of that seed */
  ed25519_create_seed(seed);
  ed25519_create_keypair(public_key, private_key, seed);

  /* create signature on the message with the keypair */
  ed25519_sign(signature, message, message_len, public_key, private_key);

  /* verify the signature */
  if (ed25519_verify(signature, message, message_len, public_key)) {
    Serial.print("valid signature\n");
  } else {
    Serial.print("invalid signature\n");
  }

  /* create scalar and add it to the keypair */
  ed25519_create_seed(scalar);
  ed25519_add_scalar(public_key, private_key, scalar);

  /* create signature with the new keypair */
  ed25519_sign(signature, message, message_len, public_key, private_key);

  /* verify the signature with the new keypair */
  if (ed25519_verify(signature, message, message_len, public_key)) {
    Serial.print("valid signature\n");
  } else {
    Serial.print("invalid signature\n");
  }

  /* make a slight adjustment and verify again */
  signature[44] ^= 0x10;
  if (ed25519_verify(signature, message, message_len, public_key)) {
    Serial.print("did not detect signature change\n");
  } else {
    Serial.print("correctly detected signature change\n");
  }

  /* generate two keypairs for testing key exchange */
  ed25519_create_seed(seed);
  ed25519_create_keypair(public_key, private_key, seed);
  ed25519_create_seed(seed);
  ed25519_create_keypair(other_public_key, other_private_key, seed);

  /* create two shared secrets - from both perspectives - and check if they're equal */
  ed25519_key_exchange(shared_secret, other_public_key, private_key);
  ed25519_key_exchange(other_shared_secret, public_key, other_private_key);

  for (i = 0; i < 32; ++i) {
    if (shared_secret[i] != other_shared_secret[i]) {
        Serial.print("key exchange was incorrect\n");
        break;
    }
  }

  if (i == 32) {
    Serial.print("key exchange was correct\n");
  }

  /* test performance */
  Serial.print("testing seed generation performance: ");
  start = millis();
  for (i = 0; i < 10000; ++i) {
    ed25519_create_seed(seed);
  }
  end = millis();

  Serial.print(end - start);
  Serial.print("ms per seed\n");


  Serial.print("testing key generation performance: ");
  start = millis();
  for (i = 0; i < 10000; ++i) {
    ed25519_create_keypair(public_key, private_key, seed);
  }
  end = millis();

  Serial.print(end - start);
  Serial.print("ms per keypair\n");

  Serial.print("testing sign performance: ");
  start = millis();
  for (i = 0; i < 10000; ++i) {
    ed25519_sign(signature, message, message_len, public_key, private_key);
  }
  end = millis();

  Serial.print(end - start);
  Serial.print("ms per signature\n");


  Serial.print("testing verify performance: ");
  start = millis();
  for (i = 0; i < 10000; ++i) {
    ed25519_verify(signature, message, message_len, public_key);
  }
  end = millis();

  Serial.print(end - start);
  Serial.print("ms per signature\n");


  Serial.print("testing keypair scalar addition performance: ");
  start = millis();
  for (i = 0; i < 10000; ++i) {
    ed25519_add_scalar(public_key, private_key, scalar);
  }
  end = millis();

  Serial.print(end - start);
  Serial.print("ms per keypair\n");


  Serial.print("testing public key scalar addition performance: ");
  start = millis();
  for (i = 0; i < 10000; ++i) {
    ed25519_add_scalar(public_key, NULL, scalar);
  }
  end = millis();

  Serial.print(end - start);
  Serial.print("ms per key\n");


  Serial.print("testing key exchange performance: ");
  start = millis();
  for (i = 0; i < 10000; ++i) {
    ed25519_key_exchange(shared_secret, other_public_key, private_key);
  }
  end = millis();

  Serial.print(end - start);
  Serial.print("ms per sharedsecret\n");


  return 0;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\nStarting\n");
  test();
  Serial.println("\n\nEnded\n");
}

void loop() {

}
