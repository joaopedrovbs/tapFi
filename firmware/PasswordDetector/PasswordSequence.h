#ifndef PASSWORD_SEQUENCE_H
#define PASSWORD_SEQUENCE_H

#include <LinkedList.h>
#include "SymbolInput.h"

class PasswordSequence {
private:
  // Saves the beggining of time of the sequence.
  unsigned long startTime;

public:
  LinkedList<SymbolInput> list;

public:
  /*
   * Constructor
   */
  PasswordSequence(unsigned long startTime = 0)
    : list(),
      startTime(startTime);

  /*
   * Adds a new symbol relative to the beginning of the sequence
   * Should be used in real-time recording
   */
  void addSymbol(SymbolType symbolType, unsigned long duration = 0);

  /*
   * Adds a new SymbolInput (with start/duration) in a "raw" manner
   * (Used to load a known password)
   * Returns true if succeded adding
   */
  bool addSymbol(SymbolInput symbol);

  /*
   * Checks if this sequence is full (MAXIMUM_EVENTS reached)
   */
  bool isFull();
};

#endif