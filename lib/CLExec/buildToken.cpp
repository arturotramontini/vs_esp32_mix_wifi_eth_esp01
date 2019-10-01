#include "CLExec.h"
//#include "CLTerminal.h"


void CLExec::buildToken (char c){

  // tokens are delimited by comma ','
  // execution char is point('.') except if preceded by anumber character 0..9
  
  // 'x' clear number and start entering in hexmode
  // '.' enter tokens in array
  
  // 0..9 build tokens (long, double, string(max 32char))

  static char cc=0;
  static char pc=0;
  
  static char cb[8];
  
  char ic[2];
  int cv;  
  //CLTerminal t1;
  static char is[40];

  // char s[200];

  static long lv;
  static double dv;
  static int hexMode = 0;
  static int floatMode = 0;
  static double  Kmult = 10;
  static int sign = 1;
  //----------

  if(c==0) return;
  
  //t1.sav();

  
  pc = cc;
  cc = c;


  for (int i = 0; i < 7; i++) cb[i] = cb[i + 1];
  cb[7]=c;
  
  
  // t1.gto(40,1);
  // for (int i = 0; i < 8; i++) {
    // sprintf(s, "%X ", cb[i]);
    // t1.prt(s);
  // }
  
  ic[0]=c;
  ic[1]=0;
  if(strlen(is)<KMAX)strcat(is,ic);
  
  cv = 0;
  if(c>='0' && c<='9') cv = (c & 0xF);
  if(hexMode==1){
    if(c>='a' && c<='f') cv = (c & 0xF)+9;;
    if(c>='A' && c<='F') cv = (c & 0xF)+9;;
  }

  
  switch(c){
    
  case ',':
  case '/':
    //------------ enter into array ----
    // Serial.println();
    // Serial.println(sign);
    // Serial.println(dv);
    // Serial.println();
    
    for(int i=MAX_P-1;i>=1;i--){
      P1[i]=P1[i-1];
      F1[i]=F1[i-1];
      //strcpy(S1[i],S1[i-1]);
    }
    P1[0]=lv*sign;
    F1[0]=dv*sign;
    is[KMAX]=0; // force max char
    
    // strcpy(S1[0],is);
    // for(int i=0;i<MAX_P;i++){
      // sprintf(s,"%2d: %-12ld  %-12g       ",i,P1[i],F1[i]);
      // t1.prt(32+i,1,s);
    // }
    
    //----------------------------------
    lv = 0;
    dv = 0;
    hexMode = 0;
    floatMode=0;
    Kmult = 10;
    is[0]=0;
    sign = 1;

    // Serial.println();
    // for(int i=0;i<8;i++){
    // Serial.println(P[i]);
    // }
    // Serial.println();    

    break;
  
  case '-':
    if (sign==1) sign = -1;  
    break;
    
  
    
  case '0'...'9':
    if(floatMode==0) {
      lv = lv * Kmult + cv;
      dv = dv * Kmult + cv;
    }
    if(floatMode==1){  
      dv = dv + (double)cv * Kmult;
      Kmult /= 10;
    }
    break;    
    
  case 'A'...'F':
    if(hexMode==1) {
      lv = lv * Kmult + cv;
      dv = dv * Kmult + cv;
    }
    break;
    
  case 'a'...'f':
    if(hexMode==1) {
      lv = lv * Kmult + cv;
      dv = dv * Kmult + cv;
    }
    break;
    
  case 'x':
    if(floatMode==0){
      hexMode=1;
      Kmult = 16;
    }
    break;
    
  case '.':
    if(pc>='0' && pc<='9'){
      floatMode=1;
      hexMode=0;
      Kmult = 0.1;
      pc = 0;
      cc = 0;
      // t1.prt(29,1,"enter float");
     
    }else{
      
      ReadyToProcess = 1;
      // t1.prt(29,1,"execute");  
        
    }
    break;
    
  }
  
  // sprintf(s,"pc:%X cc:%X hm:%d  fm:%d  sign:%d lv:%-ld  dv:%-f  is:%s           ",  pc,cc,hexMode, floatMode, sign,lv*sign, dv*sign, is);
            
            
  // t1.prt(30,1,s);
  
  
  //t1.rst();
  
}
