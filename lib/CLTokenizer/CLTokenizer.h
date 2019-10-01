#ifndef CLTokenizer_h
#define CLTokenizer_h
#include "Arduino.h"

class CLTokenizer {

public:
  int Nelements;
  String Tokens[32];

  char TokenDelimiter1[32];

  char str[200];
  int idx;

  CLTokenizer();
  void tokenize();
  void buildTokens(char);
  void showTokens() ;
  void clearTokens();
};

#endif
