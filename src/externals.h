#ifndef externals_h
#define externals_h

// #include "constants.h"

// senza gli include delle classi usate dà errore
#include "CLExec.h"
#include "CLTokenizer.h"
// #include "CLMotion.h"
// #include "CLTerminal.h"
// #include "CLPacket.h" 

//-- classes
extern class  CLExec ex;
extern class  CLTokenizer tk1;
extern class  CLTokenizer tk2;
extern class  CLTokenizer tk3;
// extern class  CLMotion m1;
// extern class  CLTerminal t1;
// extern class  CLpacket p1,p2;

// extern class  CLTerminal t2;

// //--functions
// extern void executeP();
// extern void showData();
// extern void Help();

// //--- data
// extern double counter;

//-------------------- from file checkAT.cpp ----
extern volatile int flag_WiFiConnected;
extern volatile int flag_WiFiGotIp;
extern void checkAT();
//-----------------------------------------------


//-------------------- from file cbuildHtmlPage.cpp ----
extern String htmlPage;
extern String htmlGetCommand;
void buildHtmlPage();
extern const String str_3;
//------------------------------------------------------    


void Test001();

#endif
