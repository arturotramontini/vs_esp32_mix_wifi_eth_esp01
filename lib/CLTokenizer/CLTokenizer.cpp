#include "CLTokenizer.h"

CLTokenizer::CLTokenizer() {
  idx = 0;
  memset(str, 0, sizeof(str));
  Nelements = sizeof(Tokens) / sizeof(Tokens[0]);

  strcpy(TokenDelimiter1, " ,:/\"\\=."); // default delimiters
}

void CLTokenizer::buildTokens(char c) {
  if (c == 0)
    return;
  if (c == 10)
    return;
  if (idx > (sizeof(str) - 3)) {
    idx = sizeof(str) - 3;
  }
  str[idx++] = c;
  str[idx] = 0;
  if (c == 13) {
    tokenize();
    idx = 0;
  }
}

void CLTokenizer::tokenize() {

  int len;
  len = strlen(str);
  if (len >= 190) { // limit str at 190 chars
    str[190] = 0;
    len = strlen(str);
  }

  char token[200]; // len(token) must be greater then len(str)
  memset(token, 0, sizeof(token));
  int tokIdx = 0;

  for (idx = 0; idx < Nelements; idx++) { // clear Tokens
    Tokens[idx] = "";
  }

  int idx = 0;
  for (int i = 0; i < len; i++) {
    char c = str[i];
    if (strchr(TokenDelimiter1, c)) { // check if is a token delimiter char
      if (strlen(token) > 0) {        // acquire token only if not NULL
        Tokens[idx] = token;
        if (idx < (Nelements - 1))
          idx++;
        memset(token, 0, sizeof(token));
        tokIdx = 0;
      }
    } else { // collect token
      if ((c != 0) && (c != 13) && (c != 10))
        token[tokIdx++] = c;
    }
  }

  if (strlen(token) != 0) {
    Tokens[idx] = token;
  }
}

void CLTokenizer::showTokens() {
  //----------------------- // only for debug  -------------
  char s[200];
  // sprintf(s, "\r\n----------------------------------\r\n");
  // Serial.print("number of Tokens[] array elements: ");
  // Serial.print(Nelements);
  // Serial.print(",");
  // Serial.println(sizeof(Tokens) / sizeof(Tokens[0]));

  // if (str[0] == 13)
  //   return;
  if (Tokens[0] == "")
    return;

  sprintf(s, "\r\n------ Tokens -----------\r\n");
  Serial.print(s);
  Serial.println(str);
  for (int i = 0; i < Nelements; i++) {
    if (Tokens[i] == "")
      break;
    sprintf(s, "%-2d:%2d:%s\r\n", i, strlen(Tokens[i].c_str()),
            Tokens[i].c_str());
    Serial.print(s);
  }
  sprintf(s, "-------------------------\r\n");
  Serial.print(s);
  //---------------------------------------------------------
}

void CLTokenizer::clearTokens() {
  for (int i = 0; i < Nelements; i++) {
    Tokens[i]="";
  }
}

