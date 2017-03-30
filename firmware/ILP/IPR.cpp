#include "IPR.h"

bool ipr_parse(char* buffer, uint8_t length, ipr_t &ipr) {
  ipr
}

bool ipr_validate(char* buffer, uint8_t length) {
  uint8_t cursor = 0;

  // Validate minimum size
  if (length < 4)
    return false;

  // Validate Type
  uint8_t type = buffer[cursor++];
  if (type != TYPE_ILP_PAYMENT)
    return false;

  // Validate total size
  uint32_t expectLength = buffer[cursor++];
  
  if (expectLength & 0x80) {
    // If it's MSB is set, it's actually the length of the length
    uint8_t lengthOfLength = expectLength & (~0x80);
    
    // Check length
    if (lengthOfLength + cursor >= length || lengthOfLength > 4) {
      return false;
    }

    expectLength = 0x00;
    for (int i = 0; i < lengthOfLength; i++){
      // Join length together
      uint8_t thisByte = buffer[cursor++];

      expectLength |= thisByte << (i * 8);
    }
  }

  // Length length
  if (expectLength + cursor > length) {
    return false;
  }

  return true
}

bool ipr_sign(char *buffer, uint8_t length, char *signature) {

}

