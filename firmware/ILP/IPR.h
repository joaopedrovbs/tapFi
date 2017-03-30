#ifndef ILP_H
#define ILP_H

#define TYPE_ILP_PAYMENT

#define AMOUNT_SCALE 0.01
typedef {
  uint64_t amount,
  uint8_t accountLength,
  char* account,
} ipr_t;

/*
 * Parses a ipr packet exctracting amount and account from it.
 * Returns false if it failed, true on success
 */
bool ipr_parse(char* buffer, uint8_t length, ipr_t &ipr);

/*
 * Validates an ipr packet.
 */
bool ipr_validate(char* buffer, uint8_t length);

/*
 * Signs an ipr packet with ed25519
 */
bool ipr_sign(char *buffer, uint8_t length, char *signature);

#endif