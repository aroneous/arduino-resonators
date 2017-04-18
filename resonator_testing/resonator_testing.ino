#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

#define L1 0xEE8800
#define L2 0xFF6600
#define L3 0xCC3300
#define L4 0x990000
#define L5 0xFF0033
#define L6 0xCC0066
#define L7 0x660066
#define L8 0x330033
#define GREEN 0x00FF00
#define BLUE  0x0000FF

#define L1D 0x332200
#define L2D 0x441500
#define L3D 0x240800
#define L4D 0x240000
#define L5D 0x330011
#define L6D 0x270015
#define L7D 0x150015
#define L8D 0x070007
#define GREEND 0x001500
#define BLUED  0x000015

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:

Adafruit_NeoPixel strip = Adafruit_NeoPixel(240, PIN, NEO_GRB + NEO_KHZ800);



void setup() {

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  resonatorDeployed(GREEN, L1);
  resonatorPulsing(GREEN, L1);
  resonatorPulsing(GREEN, L2);
  resonatorPulsing(GREEN, L3);
  resonatorPulsing(GREEN, L4);
  resonatorPulsing(GREEN, L5);
  resonatorPulsing(GREEN, L6);
  resonatorPulsing(GREEN, L7);
  resonatorPulsing(GREEN, L8);
  resonatorDeployed(BLUE, L8);
  resonatorPulsing(BLUE, L8);
  resonatorPulsing(BLUE, L7);
  resonatorPulsing(BLUE, L6);
  resonatorPulsing(BLUE, L5);
  resonatorPulsing(BLUE, L4);
  resonatorPulsing(BLUE, L3);
  resonatorPulsing(BLUE, L2);
  resonatorPulsing(BLUE, L1);
}

// Fill the dots one after the other with a color
void resonatorDeployed(uint32_t c, uint32_t resonatorLevel) {
  for(uint16_t i=120; i>0; i--) {
    strip.setPixelColor(i, 0);
    strip.show();   
  }
  delay(2000);
  for(uint16_t i=120; i>12; i--) {
    strip.setPixelColor(i, resonatorLevel);
    strip.show();
    delay(5);
  }
    for(uint16_t i=12; i>0; i--) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(5);
  }
}


//Theatre-style crawling lights.
void resonatorPulsing(uint32_t c, uint32_t resonatorLevel) {
  uint32_t df;
  uint32_t cd;
  uint8_t pulsewait = 250;
    for (int q=3; q > 0; q--) {
      for (uint16_t i=0; i < 12; i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
        strip.setPixelColor(i+q+12, resonatorLevel);
        strip.setPixelColor(i+q+24, resonatorLevel);
        strip.setPixelColor(i+q+36, resonatorLevel);
        strip.setPixelColor(i+q+48, resonatorLevel);
        strip.setPixelColor(i+q+60, resonatorLevel);
        strip.setPixelColor(i+q+72, resonatorLevel);
        strip.setPixelColor(i+q+84, resonatorLevel);
        strip.setPixelColor(i+q+96, resonatorLevel);
        strip.setPixelColor(i+q+108, resonatorLevel);
      }
      strip.show();

      delay(pulsewait);

      for (uint16_t i=0; i < 12; i=i+3) {
        switch (resonatorLevel) {
          case L1 : df = L1D;
            break;
          case L2 : df = L2D;
            break;
          case L3 : df = L3D;
            break;
          case L4 : df = L4D;
            break;
          case L5 : df = L5D;
            break;
          case L6 : df = L6D;
            break;
          case L7 : df = L7D;
            break;
          case L8 : df = L8D;
            break;
          default: df = 0;
            break;
      }
      switch (c) {
        case GREEN : cd = GREEND;
          break;
        case BLUE : cd = BLUED;
          break;
        default: cd = 0;
          break;
      }
        strip.setPixelColor(i+q, cd);
        strip.setPixelColor(i+q+12, df);
        strip.setPixelColor(i+q+24, df);
        strip.setPixelColor(i+q+36, df);
        strip.setPixelColor(i+q+48, df);
        strip.setPixelColor(i+q+60, df);
        strip.setPixelColor(i+q+72, df);
        strip.setPixelColor(i+q+84, df);
        strip.setPixelColor(i+q+96, df);
        strip.setPixelColor(i+q+108, df);
      }
  }
}


