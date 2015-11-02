void loop() {
  
}
#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#include "display_driver.h"

#define MAX_NAMES_ALLOCATED 5
#define MAX_NAME_LENGTH 15

#define NAME_TEXT_SIZE 1
#define TEXT_COLOR WHITE

char firstName[MAX_NAMES_ALLOCATED][MAX_NAME_LENGTH] = { "GARRISON", "ZHENG HAO", "TZU FEI", "LOREN", "JOHN" };
char lastName[MAX_NAMES_ALLOCATED][MAX_NAME_LENGTH] = { "BELLACK", "TAN", "YU", "WANG", "CONNOLY" };

void setup()   {                
  Serial.begin(9600);

  begin();

  // Clear the buffer.
  clearDisplay();

  // text display tests
  setTextSize(NAME_TEXT_SIZE);
  setTextColor(WHITE);
  setCursor(0,0);

  uint8_t i = 0;
  for (i = 0; i < MAX_NAMES_ALLOCATED; ++i) {
    println(firstName[i]);
    println(lastName[i]);
  }
  display();

  testscrolltext();
}

// TODO: Scrolling stuff.
void testscrolltext() {
  
  setTextSize(1);
  setTextColor(WHITE);
  setCursor(10,0);
  clearDisplay();
  println("scroll");
  display();
 
  startscrollright(0x00, 0x0F);
  delay(2000);
  stopscroll();
  delay(1000);
  startscrollleft(0x00, 0x0F);
  delay(2000);
  stopscroll();
}
