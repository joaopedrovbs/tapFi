#include <LinkedList.h>
#include "SymbolInput.h"

// Maximum number of events (tap/rotations) that can happen
#define MAXIMUM_EVENTS      15

/* 
 * Stores each SymbolInput with it's timestamp
 */
class PasswordSequence : LinkedList<SymbolInput> {
private:
  // Saves the beggining of time of the sequence.
  unsigned long startTime;
public:
  /*
   * Constructor
   */
  PasswordSequence(unsigned long startTime = 0)
    : LinkedList<SymbolInput>(),
      startTime(startTime)
  {
    // 
  }

  /*
   * Adds a new symbol relative to the beginning of the sequence
   * Should be used in real-time recording
   */
  void addSymbol(SymbolType symbolType, unsigned long duration = 0) {
    if (isFull()) {
      return false
    }

    // Checks if it's the first symbol being inserted. 
    // If so, get current time and stores it
    if (this.size() == 0) {
      startTime = millis()
    }

    SymbolInput symbol;
    symbol.type = symbolType;
    symbol.start = millis() - startTime;
    symbol.duration = duration;

    return this.add(symbol)
  }

  /*
   * Adds a new SymbolInput (with start/duration) in a "raw" manner
   * (Used to load a known password)
   * Returns true if succeded adding
   */
  bool addSymbol(SymbolInput symbol) {
    if (isFull()) {
      return false
    }

    return this.add(symbol);
  }

  /*
   * Checks if this sequence is full (MAXIMUM_EVENTS reached)
   */
  bool isFull() {
    return this.size() >= MAXIMUM_EVENTS;
  }
}