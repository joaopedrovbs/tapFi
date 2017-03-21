#ifndef SYMBOL_INPUT_H
#define SYMBOL_INPUT_H

#define SYMBOL_TYPE_NUM   7

/*
 * SymbolType is a basic unit of representation of a symbol.
 * Since gestures do not provide 'chars' as output, we declare
 * each type here.
 */
enum SymbolType {
  TAP = 0,
  X_ROTATE,
  Y_ROTATE,
  Z_ROTATE,
  X_MOVE,
  Y_MOVE,
  Z_MOVE
};

/*
 * A SymbolInput is both the representation of the symbol itself,
 * and the moment is appeared (milliseconds since start of first symbol)
 */
typedef SymbolInput {
  // Type of symbol
  SymbolType type;

  // Start and end duration (`duration` is 0 for TAP event)
  unsigned long start;
  unsigned long duration;

  // Used internally in the algorithm to detect if a match has been found 
  bool matched;
};

#endif