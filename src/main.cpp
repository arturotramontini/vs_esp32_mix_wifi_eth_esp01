
/* #region include */
#include "main.h"
#include "Arduino.h"
#include <Ethernet.h>
#include <WiFi.h>

#define SS_PIN_DEFAULT 5
#include "externals.h"
#include "freertos/FreeRTOS.h"
#include <SPI.h>
//--- per lo watchgog ---
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
//-----------------------
/* #endregion */

/* #region utility */
//-------------------------------------- legge stack pointer
#include "xtensa/config/core.h"
#include "xtensa/corebits.h"

static portMUX_TYPE gpio_spinlock = portMUX_INITIALIZER_UNLOCKED;

static volatile uint32_t baseSP = 0;

static inline void *get_sp(void) {
  void *sp;
  asm volatile("mov %0, sp;" : "=r"(sp));
  return sp;
}

void print_SP() {
  uint32_t v;
  char s[200];
  v = (uint32_t)get_sp();
  sprintf(s, "SP:0x%08X (%d)", v, v - baseSP);
  Serial.println(s);
}
//---------------------------------------------------------------
/* #endregion */

/* #region ETH*/
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 10, 191);
EthernetServer server1(80);
#include <utility/w5100.h>
W5100Class W5500;

bool debugETH = true;

//======================================== eth section ===
void setup_eth() {
  // Open serial communications and wait for port to open:
  // Serial.begin(115200);

  Serial.print("Ethernet WebServer ");
  Serial.print("( CS pin:");
  Serial.print(W5100Class::ss_pin);
  Serial.println(" )");

  Ethernet.linkStatus();
  Ethernet.begin(mac, ip);
  Ethernet.linkStatus();
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without "
                   "hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  // start the server
  server1.begin();
  Serial.print("ETH server is at: ");
  Serial.println(Ethernet.localIP());
}

const int ac[8] = {36, 39, 34, 35, 32, 33, 0, 0};

void loopR_eth() {
  static uint32_t Time0;
  static bool Timeout = false;
  static int counter1 = 10000;
  static int counter2 = 20000;
  static int rxN = 1;
  char s[200];
  bool flagConnected = false;
  bool flagAvailable = false;

  EthernetClient client = server1.available();

  if (client) {

    if (debugETH) {
      Serial.println();
      Serial.print("New Client Eth: ");
      Serial.println(client.remoteIP());
    }

    flagConnected = false;
    flagAvailable = false;

    Time0 = millis();
    Timeout = false;

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    while (client.connected() || Timeout) {
      if (!flagConnected) {
        flagConnected = true;
        Serial.println("connected");
      }

      if (client.available() || Timeout) {
        if (!flagAvailable) {
          flagAvailable = true;
          Serial.println("available");
        }

        char c;
        if (!Timeout) {
          c = client.read();
          if (debugETH) {
            Serial.write(c);
          }
        } else {
          c = 13;
        }

        tk3.buildTokens(c);

        if (debugETH) {
          if (c == 13) {
            tk3.showTokens();
          }
        }

        if ((tk3.Tokens[0] == "GET") && (tk3.Tokens[1] == "HTTP")) {
          rxN = 1;
        }

        if ((tk3.Tokens[0] == "GET") && (tk3.Tokens[1] == "c123")) {
          rxN = 2;
        }

        if ((tk3.Tokens[0] == "GET") && (tk3.Tokens[1] == "c124")) {
          rxN = 3;
        }

        tk3.clearTokens();

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        if ((c == '\n' && currentLineIsBlank) || Timeout) {

          if (rxN == 1) {
            client.println(str_3);
          }

          if (rxN == 2) {
            htmlPage = "";
            if (!Timeout)
              htmlPage += "<html><p>";
            counter1++;
            sprintf(s, "%06d", counter1);
            htmlPage += s;
            if (!Timeout)
              htmlPage += "</p></html>";
            client.println(htmlPage);
          }

          if (rxN == 3) {
            htmlPage = "<html><p> counter2:";
            counter2++;
            sprintf(s, "%06d", counter2);
            htmlPage += s;
            htmlPage += "</p></html>";
            client.println(htmlPage);
          }

          rxN = 0;

          //  // original example that send analog values ...
          //  // send a standard http response header
          //  client.println("HTTP/1.1 200 OK");
          //  client.println("Content-Type: text/html");
          //  client.println("Connection: close"); // the connection
          //  will be closed
          //                                       // after completion of the
          //                                       // response
          //  // client.println("Refresh: 5");  // refresh the page
          //  automatically
          //  // every 5 sec
          //  // client.println("Refresh: 2");  // refresh the page
          //  automatically
          //  // every 1 sec
          //  client.println();
          //  client.println("<!DOCTYPE HTML>");
          //  client.println("<meta charset=\"UTF-8\">");
          //  client.println("<html>");
          //  // output the value of each analog input pin
          //  for (int analogChannel = 0; analogChannel < 6;
          //  analogChannel++) {
          //    int sensorReading = analogRead(ac[analogChannel]);
          //    client.print("analog input ");
          //    client.print(analogChannel);
          //    client.print(" is ");
          //    client.print(sensorReading);
          //    client.print("<br/>");
          //  }
          //  client.println("</html>");

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      } else {
        if ((int)(millis() - Time0) > 100) {
          if (!Timeout)
            if (debugETH) {
              Serial.println("\r\n-- no blank line for 100msec TIMEOUT --");
            }
          Timeout = true;
        }
      }
    }

    // close the connection when standard HTTP, else leave connection open.
    if (!Timeout) {
      client.stop();
      if (debugETH) {
        Serial.println("client disconnected");
      }
    }

    if(client){
        Serial.println("client again active");
    }else{
        Serial.println("client no more active");
    }

    if (client.connected()){
        Serial.println("client again connected");
    }else{
        Serial.println("client no more connected");
    }


  }
}
//=========================================================
/* #endregion */

/* #region wifi */
//======================================== wifi section ===
//#include <WiFi.h>
// const char* ssid     = "View Max";
// const char* password = "Borjakat75!";
const char *ssid = "Cudy-C50F";
const char *password = "06522165";
// Set your Static IP address
IPAddress local_IP(192, 168, 10, 190);
// Set your Gateway IP address
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional
WiFiServer server(80);

bool debugWIFI = true;

void setup_wifi() {
  // Serial.begin(115200);
  pinMode(21, OUTPUT); // set the LED pin mode
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi server connected at:");
  // Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loopR_wifi() {
  static uint32_t Time0;
  static bool Timeout = false;
  static int counter1 = 10000;
  static int counter2 = 20000;
  static int rxN = 1;
  char s[200];
  bool flagConnected = false;
  bool flagAvailable = false;
  
  WiFiClient client = server.available(); // listen for incoming clients

  if (client) { // if you get a client,

    if (debugWIFI) {
      Serial.println();
      Serial.print("New Client WiFi Connected:");
      Serial.println(client.remoteIP());
    }

    flagConnected = false;
    flagAvailable = false;

    Time0 = millis();
    Timeout = false;

    // make a String to hold incoming data from the client
    // String currentLine = "";
    boolean currentLineIsBlank = true;

    while (client.connected() || Timeout) {

      if (!flagConnected) {
        flagConnected = true;
        Serial.println("connected");
      }

      TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
      TIMERG0.wdt_feed = 1;
      TIMERG0.wdt_wprotect = 0;

      if (!flagAvailable){
        Time0 = millis();
        Timeout = false;
      }

      if (client.available() || Timeout) {
        if (!flagAvailable) {
          flagAvailable = true;
          Serial.println("available");
        }

        char c;
        if (!Timeout) {
          c = client.read(); // read a byte, then
          if (debugWIFI) {
            Serial.write(c); // print it out the serial monitor
          }
        } else {
          c = 13;
        }

        tk2.buildTokens(c);

        if (debugWIFI) {
          if (c == 13) {
            tk2.showTokens();
          }
        }

        if ((tk2.Tokens[0] == "GET") && (tk2.Tokens[1] == "HTTP")) {
          rxN = 1;
        }

        if ((tk2.Tokens[0] == "GET") && (tk2.Tokens[1] == "c123")) {
          rxN = 2;
        }

        if ((tk2.Tokens[0] == "GET") && (tk2.Tokens[1] == "c124")) {
          rxN = 3;
        }

        tk2.clearTokens();

        // if the current line is blank, you got two newline characters in a
        // row.
        // that's the end of the client HTTP request, so send a response:

        // if the byte is a newline character
        if (((c == '\n') && currentLineIsBlank) || Timeout) {

          if (rxN == 1) {
            client.println(str_3);
          }

          if (rxN == 2) {
            htmlPage = "<html><p> counter1:";
            counter1++;
            sprintf(s, "%06d", counter1);
            htmlPage += s;
            htmlPage += "</p></html>";
            client.println(htmlPage);
          }

          if (rxN == 3) {
            htmlPage = "<html><p> counter2:";
            counter2++;
            sprintf(s, "%06d", counter2);
            htmlPage += s;
            htmlPage += "</p></html>";
            client.print(htmlPage);
          }

          rxN = 0;

          // // --- the original example
          // //---------------------------------------------------------------------------------------------------
          // // HTTP headers always start with a response code (e.g. HTTP/1.1
          // 200 OK)
          // // and a content-type so the client knows what's coming, then a
          // blank line:

          // client.println("HTTP/1.1 200 OK");
          // client.println("Content-type:text/html");

          // client.println();

          // // the content of the HTTP response follows the header:

          // client.println("<!DOCTYPE HTML>");
          // client.println("<meta charset=\"UTF-8\">");
          // client.println("<html>");
          // client.print("Click <a href=\"/H\">here</a> to turn the LED on
          // pin 21 on.<br>");
          // client.print("Click <a href=\"/L\">here</a> to turn the LED on
          // pin 21 off.<br>");
          // client.println("</html>");

          // // The HTTP response ends with another blank line:
          // client.println();
          // // break out of the while loop:
          // //---------------------------------------------------------------------------------------------------
          
          flagAvailable = false;
          Timeout = false;
          Time0 = millis();
          break;
        }
        if (c == '\n') {
          // if you got a newline, then clear currentLine:
          currentLineIsBlank = true; // currentLine = "";
        } else if (c != '\r') {
          // if you got anything else but a carriage return character,
          currentLineIsBlank = false; // currentLine += c; // add it to the end
                                      // of the currentLine
        }
      } else {
        if ((int)(millis() - Time0) > 500) {
          if (!Timeout)
            if (debugETH) {
              Serial.println("\r\n-- no blank line for 100msec TIMEOUT --");
            }
          Timeout = true;
        }
      }
    }

    // close the connection:
    if (!Timeout) {
      client.stop();
      if (debugETH) {
        Serial.println("Client Disconnected.");
      }
    }

    if(client){
        Serial.println("client again active");
    }else{
        Serial.println("client no more active");
    }

    if (client.connected()){
        Serial.println("client again connected");
    }else{
        Serial.println("client no more connected");
    }

  }
}
//=========================================================
/* #endregion */

/* #region task_seriale */

//=========================================================
// TASKs FOR  AT COMMANDS of ESP01

String Esp01Rx = "";
String ATTok[32];
static volatile int seqAT1 = 1;

void resetESP01() {
  //--- reset del esp01  su gp0 ---
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);
  delay(10);
  digitalWrite(0, LOW);
  delay(10);
  digitalWrite(0, HIGH);
  //---------------------------------
}

void do_seqAT1() {

  if (seqAT1 == -1)
    return; // lo setto a mano per escludere in debug

  static int flagHttp = 0;
  static int sockNumber = 0;
  static int testNumber = 0;
  static char s1[300];
  char s[128];

  static volatile int last = 0;
  static volatile uint32_t T0 = 0;

  if (last != seqAT1) {
    last = seqAT1;
    T0 = millis();
    // sprintf(s, "seqAT1:%d\r\n", seqAT1);
    // Serial.print(s);
  }

  if ((seqAT1 != 1) && (last == seqAT1) && ((millis() - T0) > 2000)) {
    seqAT1 = 1;
    // Serial.println("reset seqAT1");
    for (int i = 0; i < 32; i++)
      tk1.Tokens[i] = ""; // clear all token strings
  }

  switch (seqAT1) {

  case 1:
    if (tk1.Tokens[0] == "+IPD")
      tk1.Tokens[3].toUpperCase();
    if (tk1.Tokens[3] == "GET") {
      seqAT1 = 101;
      sockNumber = tk1.Tokens[1].toInt();
      htmlGetCommand = tk1.Tokens[4];
    }
    break;

  case 101: //-------------------------------------------- prepara testo di
            // risposta
    if (tk1.Tokens[4] != "favicon") {

      testNumber++;
      sprintf(s1, "testNumber:%d\r\n", testNumber);

      memset(s1, 0x41, 260);
      s1[260 - 1] = 0;

      buildHtmlPage();

    } else {
      strcpy(s1, "\r\n");
    }

    // check if is an HTTP
    flagHttp = 0;
    for (int i = 0; i < 32; i++) {
      if (tk1.Tokens[i] == "HTTP") {
        flagHttp = 1;
        break;
      }
    }
    seqAT1 = 102;
    break;

  case 102: // wait the null string of the HTLM, or 500msec
    if (tk1.Tokens[0] == "") {
      // Serial.println("HTML null string for reply");
      seqAT1 = 103;
    }
    if ((millis() - T0) > 200) {
      Serial.println(
          "force HTML null string for reply for over 200 msec delay");
      seqAT1 = 103;
    }
    if (flagHttp == 0) { // just a socket , not an http protocol request, so do
                         // not wait blank line
      Serial.println("-------------just a socket, not an HTTP request");
      seqAT1 = 103;
    }
    break;

  case 103:
    sprintf(s, "AT+CIPSEND=%d,%d\r\n", sockNumber, htmlPage.length()); // s1));
    Serial1.print(s);
    Serial1.println(); // last cr lf with empty linex
    seqAT1 = 104;
    break;

  case 104:
    if (tk1.Tokens[0] == ">")
      seqAT1 = 2;
    break;

  case 2:
    // Serial1.print(s1);
    sprintf(s, "strlen(htmlPage):%d", htmlPage.length());
    Serial.println(s);
    Serial.println(htmlPage);
    Serial1.println(htmlPage);
    seqAT1 = 3;
    break;

  case 3:
    if (tk1.Tokens[0] == "SEND") {
      if (tk1.Tokens[1] == "OK") {
        if (flagHttp == 1) {
          seqAT1 = 4;
        } else {
          seqAT1 = 1;
        }
      }
    }
    break;

  case 4:
    sprintf(s, "AT+CIPCLOSE=%d\r\n", sockNumber);
    Serial1.print(s);
    seqAT1 = 5;
    break;

  case 5:
    if (tk1.Tokens[0] == "OK")
      seqAT1 = 1;
    break;
  }
}

//===============================================

void serialRx1() {

  static int delay1 = 0;

  static byte flag = 0;
  if (!flag) {
    flag = 1;
    Serial.print("stackPNT taskRx1 serialRx1:");
    Serial.println((uint32_t)get_sp(), 16);
    print_SP();
  }

  if (Serial1.available()) {
    char c = Serial1.read();

    delay1 = (int)millis() + (500);
    if (delay1 == 0)
      delay1++;

    if (c == 13) {
      Serial2.print("@");
    } else if (c == 10) {
      Serial2.print("&\r\n");
    } else {
      Serial2.print(c);
    }

    tk1.buildTokens(c);

    if (c == 13) {
      tk1.showTokens();
    }
  }

  // auto add CR if 500msec after last char
  if ((delay1 != 0) && ((delay1 - (int)millis()) < 0)) {
    delay1 = 0;
    tk1.buildTokens(13);
    tk1.showTokens();
  }

  checkAT();
}

//===============================================

void TaskRx1(void *pvParameters) {
  (void)pvParameters;

  portENTER_CRITICAL(&gpio_spinlock);
  Serial.print("stackPNT taskRx1:");
  Serial.println((uint32_t)get_sp(), 16);
  print_SP();

  portEXIT_CRITICAL(&gpio_spinlock);

  for (;;) {
    serialRx1();
    if (Serial2.available()) {
      char c = Serial2.read();
      Serial1.print(c);
    }
    // checkActions();
    do_seqAT1();
  }
}

//===============================================
//===============================================
//===============================================

void send001() {
  // char s[300];

  while ((flag_WiFiConnected == 0) || (flag_WiFiGotIp == 0)) {
    delay(10);
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_wprotect = 0;
  }
  Serial.println("-------------- wifi connected && got ip ---------------");
  Serial1.println("AT\r");
  delay(100);
  Serial1.println("ATE0\r");
  delay(100);
  Serial1.println("AT+CWJAP?\r");
  delay(100);
  Serial1.println("AT+CIFSR\r");
  delay(100);
  Serial1.println("AT+CWMODE=3\r");
  delay(100);
  Serial1.println("AT+CIPMUX=1\r");
  delay(100);
  Serial1.println("AT+CIPSERVER=1,80\r");
  delay(100);
  Serial1.println("AT+CIPSTATUS\r");
  delay(100);
}

//===============================================

void TaskRx0(void *pvParameters) {
  (void)pvParameters;

  portENTER_CRITICAL(&gpio_spinlock);
  Serial.print("stackPNT taskRx0:");
  Serial.println((uint32_t)get_sp(), 16);
  print_SP();

  portEXIT_CRITICAL(&gpio_spinlock);

  char s[128];

  for (;;) {
    ex.checkSerial();
    char c = ex.getCircBuf();
    if (c != 0) {

      Serial.print(c);

      if (toupper(c) == 'X') {
        send001();
      }

      ex.buildToken(c);

      if (ex.ReadyToProcess == 1) {
        ex.ReadyToProcess = 0;
        if (ex.P1[0] == 1) {
          resetESP01();
        }
        if (ex.P1[0] == 2) {
          print_SP();
        }
        if (ex.P1[0] == 3) {
          sprintf(s, "seqAT1:%d\r\n", seqAT1);
          Serial.print(s);
        }
        if (ex.P1[0] == 4) {
          seqAT1 = ex.P1[1];
        }
      }

      // tk1.buildTokens(c);
    }
  }
}
//===============================================
//===============================================
//===============================================
/* #endregion */

/* #region setup */
// -- per la dokit server reindirizzare pin tx e rx
#define RXD1 4
#define TXD1 2
BaseType_t Bt1;
BaseType_t Bt2;

void setup() {

  Serial.begin(921600);
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  Serial1.setRxBufferSize(2000);
  Serial2.begin(115200);

  Test001();

  Serial.println();
  Serial.println("--------------");
  Serial.println("from project: VS_E32_Mix_Wifi_Eth");

  int delay1;
  uint32_t base;
  // int result;

  base = 0xFffffff0;
  Serial.println(base, HEX);
  delay1 = base + 32;
  Serial.println(delay1, HEX);
  Serial.println(delay1 - base);
  base += 64;
  Serial.println((int)(delay1 - base));

  Serial.println("--------------");

  static byte flag = 0;
  if (!flag) {
    flag = 1;
    Serial.print("stackPNT task SETUP:");
    baseSP = (uint32_t)get_sp(), Serial.println(baseSP, 16);
    print_SP();
  }

  // set up two tasks to run independently.
  Bt1 =
      xTaskCreate(TaskRx0,
                  "TaskRx0", // A name just for humans
                  4000, // This stack size can be checked & adjusted by reading
                        // the Stack Highwater
                  NULL,
                  1, // 2 // Priority, with 3 (configMAX_PRIORITIES - 1)
                     // being the highest, and 0 being the lowest.
                  NULL);

  Bt2 = xTaskCreate(TaskRx1,
                    "TaskRx1", // A name just for humans
                    4000,      // This stack size can be checked & adjusted by
                               // reading the Stack Highwater
                    NULL,
                    1, // 2 // Priority, with 3 (configMAX_PRIORITIES - 1) //
                       // being the highest, and 0 being the lowest.
                    NULL);

  // Serial.println("-- prove myTokenizer --");
  // char s[200];
  // strcpy(s, "+IPD:0,1:qwe/sss\r\n aaaa");
  // myTokenizer(s);
  // Serial.println("-----------");

  setup_wifi();
  setup_eth();

  //------------- setup esp01 as server
  resetESP01();
  delay(500);

  send001();
  //-----------------------------
}
/* #endregion */

/* #region loop */

void loop() {

  static byte flag = 0;
  if (!flag) {
    flag = 1;
    Serial.print("stackPNT task loop:");
    Serial.println((uint32_t)get_sp(), 16);
    print_SP();
  }

  /*
  //=================================================================
  //
  // to reset watchdog, i've copy from here:
  // https://github.com/espressif/esp-idf/issues/1646
  //  akshar001 commented on 17 Aug 2018
  //-------------
  // put these 2 includes at top of this C source
  //#include "soc/timer_group_struct.h"
  //#include "soc/timer_group_reg.h"
  //-- these 3 lines must do repeteadly at shorter time of watchdog
  */
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
  //=================================================================

  // put your main code here, to run repeatedly:
  loopR_wifi();
  loopR_eth();
}
/* #endregion */
