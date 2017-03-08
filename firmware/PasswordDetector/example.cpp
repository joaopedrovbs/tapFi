#include <LinkedList.h>
#include <SymbolInput.h>
#include <MatchSequence.h>
#include <PasswordSequence.h>

void main() {
  // Load some password here
  PasswordSequence password();

  // Four taps with equal interval
  int intervals[] = {
    0    // [First tap]
    300, // [Second tap...]
    300,
    300,
  }

  unsigned long totalTime = 0;
  for(int i = 0; i < sizeof(intervals) / sizeof(int); i++) {
    SymbolInput symb;
    
    // Increment start time
    totalTime += intervals[i];

    symb.type = TAP;
    symb.start = totalTime;

    password.addSymbol(symb)
  }


  /*
   * Read some taps, and save to testPassword
   */
  PasswordSequence testPassword();

  // Always clear before re-using
  testPassword();

  // Wait timeout
  while(NOT_FINISHED) {
    if (DID_TAP) {
      // It will account for current time and create SymbolInput automatically
      testPassword.addSymbol(TAP)
    }
  }

  /*
   * Compares two sequences and return the thrust in being the same
   */

  // Configure match parameters
  MatchParams params;
  params.timeScaleMax             = 0.5; // 50% speedup/speedown allowed
  params.timeScaleThrustFactor    = 0.2; //  ↑  will decrease 20% of total thrust
  params.tapDurationScaleMax      = 0.2; // 20% speedup/speedown allowed in each interval
  params.tapDurationThrustFactor  = 0.1; //  ↑  will decrease 10% of total thrust

  // Compare passwords
  float thrust = MatchSequence(&password, &testPassword, params)

  // Check thrust (0.7 might be something else.. need to test)
  if (thrust > 0.7) {
    // ✔ Password match
  } else {
    // ✖ Password do no match
  }


}