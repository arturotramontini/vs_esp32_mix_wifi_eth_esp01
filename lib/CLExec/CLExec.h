/*
  Morse.h - Library for flashing Morse code.
  Created by Arturo Tramontini. August 20 2019
  Released into public domain
*/

#ifndef CLExec_h
#define CLExec_h


#include "Arduino.h"


#define MAX_P 8
#define KMAX 32

class CLExec {

  
  
  public:
  
    CLExec();


    //long    P[MAX_P];
    long    P1[MAX_P];
    double  F1[MAX_P];
    char    S1[MAX_P][KMAX+1];  

    
    char sA[257] = "";
    char sCMD[257] = "";
    byte ReadyToProcess = 0;


    char _Buffer[60] = ""; 
    int _wp = 0;
    int _rp = 0;
    char _Tname[32] = "";


    void checkSerial();
    void checkSerial1();
    
    
    void putCircBuf(char c);
    char getCircBuf();
    

    void buildToken (char c);
    
  private:
  
    //-- used by buildToken
    char _pc;
    int _sign;
    int _hexmode;
    double  _dv;
    long    _lv;
    char    _tokstr;
    char    _typechar;
    
    //--------------------


};



#endif
