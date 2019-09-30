#include "externals.h"

volatile int flag_WiFiConnected = 0;
volatile int flag_WiFiGotIp=0;

void checkAT(){ // da chiamare dopo la costruzione del Tokens[]

    if ((tk1.Tokens[0]=="WIFI") && (tk1.Tokens[1]=="CONNECTED")) {
        if(!flag_WiFiConnected) Serial.println("WIFI is CONNECTED");
        flag_WiFiConnected=1;
    }
    if ((tk1.Tokens[0]=="WIFI") && (tk1.Tokens[1]=="GOT")&& (tk1.Tokens[2]=="IP")) {
        if(!flag_WiFiGotIp) Serial.println("WIFI has GOT IP");
        flag_WiFiGotIp=1;
    }
}
