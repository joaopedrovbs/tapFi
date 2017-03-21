#include "PasswordSequence.h"

// Maximum number of events (tap/rotations) that can happen
#define MAXIMUM_EVENTS      15

/*
 * Constructor
 */
PasswordSequence(unsigned long startTime = 0)
{
  // 
}

/*
 * Adds a new symbol relative to the beginning of the sequence
 * Should be used in real-time recording
 */
void PasswordSequence::addSymbol(SymbolType symbolType, unsigned long duration = 0) {
  if (isFull()) {
    return false
  }

  // Checks if it's the first symbol being inserted. 
  // If so, get current time and stores it
  if (list.size() == 0) {
    startTime = millis()
  }

  SymbolInput symbol;
  symbol.type = symbolType;
  symbol.start = millis() - startTime;
  symbol.duration = duration;

  return list.add(symbol)
}

/*
 * Adds a new SymbolInput (with start/duration) in a "raw" manner
 * (Used to load a known password)
 * Returns true if succeded adding
 */
bool PasswordSequence::addSymbol(SymbolInput symbol) {
  if (isFull()) {
    return false
  }

  return list.add(symbol);
}

/*
 * Checks if this sequence is full (MAXIMUM_EVENTS reached)
 */
bool PasswordSequence::isFull() {
  return list.size() >= MAXIMUM_EVENTS;
}