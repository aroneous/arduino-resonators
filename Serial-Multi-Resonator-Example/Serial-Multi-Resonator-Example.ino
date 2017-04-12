/*
  Resonator Controll via USB Serial Port
  Released under the MIT license.
  TeamNorCal, 2017
*/

#include <Adafruit_NeoPixel.h>
#include "WS2812_Definitions.h"

#define RESO0 2
#define RESO1 3
#define RESO2 4
#define RESO3 5
#define LED_COUNT 1

Adafruit_NeoPixel resonator0 = Adafruit_NeoPixel(LED_COUNT, RESO0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel resonator1 = Adafruit_NeoPixel(LED_COUNT, RESO1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel resonator2 = Adafruit_NeoPixel(LED_COUNT, RESO2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel resonator3 = Adafruit_NeoPixel(LED_COUNT, RESO3, NEO_GRB + NEO_KHZ800);

int commandChar;
int resonatorLocation;

void setup()
{
  Serial.begin(9600);
  Serial.println("Ready");
  resonator0.begin();
  resonator1.begin();
  resonator2.begin();
  resonator3.begin();
  
  clearLEDs();
  resonator0.show();
  resonator1.show();
  resonator2.show();
  resonator3.show();
}

void loop()
{
    clearLEDs(); 
  if(Serial.available()) {
    commandChar = Serial.read();

    if(commandChar == '0') {
      resonatorLocation = 0;
    }
    if(commandChar == '1') {
      resonatorLocation = 1;
    }
    if(commandChar == '2') {
      resonatorLocation = 2;
    }
    if(commandChar == '3') {
      resonatorLocation = 3;
    }

    if(commandChar == ' ') {
      setResonatorColor(0x000000, 1000, resonatorLocation);  // L0 - OFF
    }
    if(commandChar == 'a') {
      setResonatorColor(0xEE8800, 1000, resonatorLocation);  // L1 - YELLOW
    }
    if(commandChar == 'b') {
      setResonatorColor(0xFF6600, 1000, resonatorLocation);  // L2 - ORANGE
    }  
    if(commandChar == 'c') {
      setResonatorColor(0xCC3300, 1000, resonatorLocation);  // L3 - OrangeRed
    }
    if(commandChar == 'd') {
      setResonatorColor(RED, 1000, resonatorLocation);  // L4
    }
    if(commandChar == 'e') {      
      setResonatorColor(0xFF0033, 1000, resonatorLocation);  // L5 - RedPurple
    }
    if(commandChar == 'f') {      
      setResonatorColor(0xCC0066, 1000, resonatorLocation);  // L6 - PurpleRed
    }
    if(commandChar == 'g') {
      setResonatorColor(0x660066, 1000, resonatorLocation);  // L7 - Light Purple
    }
    if(commandChar == 'h') {      
      setResonatorColor(0x330033, 1000, resonatorLocation);  // L8 - Dark Purple
    }
  }
}


void setResonatorColor(unsigned long color, byte wait, byte resonator)
{
    byte red = (color & 0xFF0000) >> 16;
    byte green = (color & 0x00FF00) >> 8;
    byte blue = (color & 0x0000FF);  
    
    if ( resonator == 0) {
      resonator0.setPixelColor(0, red, green, blue);
      resonator0.show();  // Turn the LEDs on
    }
    if ( resonator == 1) {
      resonator1.setPixelColor(0, red, green, blue);
      resonator1.show();  // Turn the LEDs on
    }
    if ( resonator == 2) {
      resonator2.setPixelColor(0, red, green, blue);
      resonator2.show();  // Turn the LEDs on
    }
    if ( resonator == 3) {
      resonator3.setPixelColor(0, red, green, blue);
      resonator3.show();
    }
    delay(wait);  // Delay for visibility
}


// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++)
  {
    resonator0.setPixelColor(i, 0);
    resonator1.setPixelColor(i, 0);
    resonator2.setPixelColor(i, 0);
    resonator3.setPixelColor(i, 0);
  }
}

