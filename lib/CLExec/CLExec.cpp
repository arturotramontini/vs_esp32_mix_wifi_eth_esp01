/*
  Morse.cpp - Library for flashing Morse code.
  Created by Arturo Tramontini, 20/agosto/2019
  Released into the public domain.
*/

//#include "Arduino.h"
#include "CLExec.h"



CLExec::CLExec() {
}


void CLExec::putCircBuf(char c) {
  // put in circular buffer if not full, else discard incoming char
  byte nextWp;
  nextWp = _wp + 1;
  if (nextWp >= sizeof(_Buffer))  nextWp = 0;
  _wp = nextWp;
  if (_wp == _rp) return; // buffer is full, discard incoming byte
  _Buffer[_wp] = c;
}


char CLExec::getCircBuf() {
  // get from circular buffer if not empty, else return 0
  if (_rp == _wp) return (char)0; // buffer is empty, return 0
  byte nextRp;
  nextRp = _rp + 1;
  if (nextRp >= sizeof(_Buffer))  nextRp = 0;
  _rp = nextRp;
  char c = _Buffer[_rp];
  return c;
}

void CLExec::checkSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    //if ((c == 0x0d) || (c == 0x0a)) break;
    putCircBuf(c);
  }
}

void CLExec::checkSerial1() {
  while (Serial1.available()) {
    char c = Serial1.read();
    //Serial1.write(c); //echo
    //if ((c == 0x0d) || (c == 0x0a)) break;
    putCircBuf(c);
  }
}

