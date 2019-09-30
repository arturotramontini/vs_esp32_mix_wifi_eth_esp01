#include "externals.h"

const String str_c123 = "<html>\
<p> HELLO <br> BOYS ! </p>\
</html>";

// <head>
// <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
// </head>

const String str_3 =
#include "html_000.html"
    ;

// std::string dataBaseLeaf = R"~~~~(
String dataBaseLeaf = R"~x~3~!~( 
    ... the file data "cippolo" \fine/ ... \r\n cioppi ~~~(test)~~~ xyz
    123
    456
    ---end---)~x~3~!~";
String prova;

String htmlPage;
String htmlGetCommand;

void buildHtmlPage() {

  prova = dataBaseLeaf;
  Serial.print(prova);
  Serial.println("XXXX");

  static int counter1 = 0;
  static int counter2 = 0;
  char s[128];

  htmlPage = str_3;

  if (htmlGetCommand == "c123") {
    htmlPage = "<html><p> counter1:";
    counter1++;
    sprintf(s, "%06d", counter1);
    htmlPage += s;
    htmlPage += "</p></html>";
  }

  if (htmlGetCommand == "c124") {
    htmlPage = "<p> counter2:";
    counter2++;
    sprintf(s, "%06d", counter2);
    htmlPage += s;
    htmlPage += "</p>\r\n";
    // sprintf(s, "\r\nCOUNTER2:%06d\r\n", counter2++);
    // htmlPage = s;
  }

  sprintf(s, "length(htmlPage):%d", htmlPage.length());
  Serial.println(s);
}
