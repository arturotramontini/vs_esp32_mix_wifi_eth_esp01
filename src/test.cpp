
#include "arduino.h"
#include <iostream>
#include <string>

void Test001() {
  const String str_4 =
#include "html_001.html"
      ;

  // std::cout  << str_4 << std::endl;
  Serial.println(str_4);
}
