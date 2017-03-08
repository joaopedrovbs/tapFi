#include <LinkedList.h>
#include "SequenceInput.h"
#include "PasswordSequence.h"

/*
 * For each kind of error, we set a custom almost-zero output (for debugging purposes)
 */
#define ER_SYMBOLS_NOT_MATCH               0.05
#define ER_MAX_TIME_SCALE                  0.06
#define ER_MAX_TAP_DURATION_TIME_SCALE     0.07


/*
 * Given a sequence `s1`, and a test sequence `s2`, 
 * return how close s2 is to s1.
 *
 * Pass in a MatchParameters object to be used
 */
float MatchSequence(PasswordSequence s1, PasswordSequence s2, MatchParams params) {
  /* This is the total probability. It begins with 1.0, and will decrease with each "wrong" step */
  float totalProb = 1.0;

  /* Check for the number of each kind of event, and see if they match */
  int s1Events[SYMBOL_TYPE_NUM] = 0;
  int s2Events[SYMBOL_TYPE_NUM] = 0;

  for (int i = 0; i < s1.size(); i++) {
    int symbol = static_cast<int>(s1.get(i).type);
    s1Events[symbol] ++;
  }

  for (int i = 0; i < s2.size(); i++) {
    int symbol = static_cast<int>(s2.get(i).type);
    s2Events[symbol] ++;
  }

  for (int i = 0; i < SYMBOL_TYPE_NUM; i++) {
    if (s1Events[i] != s2Events[i]) {
      return ER_SYMBOLS_NOT_MATCH;
    }
  }

  /* Next, check for total time taken, and see if they are inside timeScaleMax */
  unsigned long s1Duration = s1.get(s1.size() - 1).start - s1.get(0).start;
  unsigned long s2Duration = s2.get(s2.size() - 1).start - s2.get(0).start;

  // timeScale increases if test sequence duration is greater
  float timeScale = (float) s2Duration / (float) s1Duration;
  float timeScaleAbs = fabs(1.0 - timeScale);

  if (timeScaleAbs > params.timeScaleMax) {
    return ER_MAX_TIME_SCALE;
  }

  // Decrease thrust probability proportionaly to the timeScaleAbs and timeScaleThrustFactor
  totalProb -= (timeScaleAbs * params.timeScaleThrustFactor);

  /* Save TAP symbol events to a separate list */
  LinkedList<SymbolInput> s1Taps = LinkedList<SymbolInput>();
  LinkedList<SymbolInput> s2Taps = LinkedList<SymbolInput>();

  for (int i = 0; i < s1.size(); i ++) {
    SymbolInput symbol = s1.get(i);
    if(symbol.type == TAP) {
      s1Taps.add(symbol);
    }
  }

  for (int i = 0; i < s2.size(); i ++) {
    SymbolInput symbol = s2.get(i);
    if(symbol.type == TAP) {
      s2Taps.add(symbol);
    }
  }

  /* Goes through all TAP events, and decrease totalProb proportionally to it's displacement */
  unsigned long lastTap1 = 0;
  unsigned long lastTap2 = 0;
  for (int i = 0; i < s1.size(); i++) {
    SymbolInput tap1 = s1.get(i);
    SymbolInput tap2 = s2.get(i);

    // Compute duration between taps
    float tap1Duration = lastTap1 - tap1.start;
    float tap2Duration = lastTap2 - tap2.start;
    
    // Save lastTaps
    lastTap1 = tap1.start;
    lastTap2 = tap2.start;

    // Skip first sample. It doesn't have duration yet
    if (i == 0) {
      continue;
    }

    // Get relative duration, computed with already accepted timeScale
    float durationScale = (tap2Duration / tap1Duration) * (1.0 / timeScale);
    float durationScaleAbs = fabs(1.0 - durationScale);

    // Check within limits
    if (durationScale > tapDurationScaleMax) {
      return ER_MAX_TAP_DURATION_TIME_SCALE;
    }

    // Decrease total probability proportionally
    totalProb -= (durationScaleAbs * tapDurationThrustFactor);
  }

  return totalProb;
}

struct MatchParams {
  /*
   * Maximum scaling time allowed (computed from start/end of sequence) 
   * If the testing sequence needs more than timeScaleMax factor to fit 
   * into the control sequence, will return with ER_MAX_TIME_SCALE
   * 
   * 0.0 --> Means there can be NO divergence at all. (1ms disparity will make it fail)
   * 0.5 --> Half way between above and below
   * 1.0 --> Means that up to doubling the duration or reducing it by half will be ok
   */
  float timeScaleMax,

  /*
   * How much wil the difference in total time (timeScale),
   * affect the total probability?
   * 
   * 0.0 --> Disconsider timeScale. Thurst wont decrease
   * 1.0 --> 1 to 1 in probability decrease (example: doubling the speed would reduce 50% of thrust)
   * 2.0 --> 2 to 1 in probability decrease
   */
  float timeScaleThrustFactor,

  /*
   * Same thing as timeScaleMax, but used in the comparision for each symbol duration.
   * Note: will be regularized with timeScale automatically
   */ 
  float tapDurationScaleMax,

  /*
   * Same thing as timeScaleThrustFactor, but used in the comparision for each symbol duration.
   * Note: will be regularized with timeScale automatically
   */ 
  float tapDurationThrustFactor,
}