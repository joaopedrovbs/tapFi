#ifndef MATCH_SEQUENCE_H
#define MATCH_SEQUENCE_H

#include "PasswordSequence.h"

/*
 * For each kind of error, we set a custom almost-zero output (for debugging purposes)
 */
#define ER_SYMBOLS_NOT_MATCH               0.05
#define ER_MAX_TIME_SCALE                  0.06
#define ER_MAX_TAP_DURATION_TIME_SCALE     0.07

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
  float timeScaleMax;

  /*
   * How much wil the difference in total time (timeScale),
   * affect the total probability?
   * 
   * 0.0 --> Disconsider timeScale. Thurst wont decrease
   * 1.0 --> 1 to 1 in probability decrease (example: doubling the speed would reduce 50% of thrust)
   * 2.0 --> 2 to 1 in probability decrease
   */
  float timeScaleThrustFactor;

  /*
   * Same thing as timeScaleMax, but used in the comparision for each symbol duration.
   * Note: will be regularized with timeScale automatically
   */ 
  float tapDurationScaleMax;

  /*
   * Same thing as timeScaleThrustFactor, but used in the comparision for each symbol duration.
   * Note: will be regularized with timeScale automatically
   */ 
  float tapDurationThrustFactor;
};


/*
 * Given a sequence `s1`, and a test sequence `s2`, 
 * return how close s2 is to s1.
 *
 * Pass in a MatchParameters object to be used
 */
float MatchSequence(PasswordSequence s1, PasswordSequence s2, MatchParams params);

#endif